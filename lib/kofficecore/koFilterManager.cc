/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
                 2000 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qfile.h>
#include <qapplication.h>

#include <kmimetype.h>
#include <kurl.h>
#include <kapp.h>
#include <kprocess.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <ktempfile.h>
#include <kfiledialog.h>
#include <koDocument.h>
#include <koFilter.h>
#include <koFilterDialog.h>
#include <koFilterManager.h>

#include <unistd.h>
#include <config.h>

class KoFilterManagerPrivate {

public:
    KoFilterManagerPrivate() { prepare=false; exitCode=0; }
    ~KoFilterManagerPrivate() {}

    QString tmpFile;
    QString exportFile;
    QString native_format, mime_type;
    bool prepare;
    KoDocument *document;
    QValueList<KoFilterEntry> m_vec;
    PreviewStack *ps;
    mutable QMap<QString, int> dialogMap;
    QMap<int, KoFilterDialog*> originalDialogs;
    QString config;  // stores the config information
    int exitCode; // the exit code of the external filter process
    QString tempfname; // yes, ugly :)
};


KoFilterManager* KoFilterManager::s_pSelf = 0;
unsigned long KoFilterManager::s_refCnt = 0;

KoFilterManager* KoFilterManager::self()
{
    if( s_pSelf == 0 )
    {
      if ( s_refCnt == 0 ) // someone forgot to call incRef
          s_refCnt++;

        s_pSelf = new KoFilterManager;
    }
    return s_pSelf;
}

KoFilterManager::KoFilterManager() {
    d=new KoFilterManagerPrivate;
}

KoFilterManager::~KoFilterManager()
{
  delete d;
}

QString KoFilterManager::fileSelectorList( const Direction &direction,
                                           const char *_format,
                                           const QString & _native_pattern,
                                           const QString & _native_name,
                                           bool allfiles ) const
{
    QString nativeFormat = QString::fromLatin1(_format);
    QString constraint;
    if ( !nativeFormat.isEmpty() ) // format is empty for koshell
    {
        constraint = "'";
        constraint += QString::fromLatin1(_format);
        constraint += "' in ";
        if ( direction == Import )
            constraint += "Export";
        else
            constraint += "Import";
    }

    QValueList<KoFilterEntry> vec = KoFilterEntry::query( constraint );

    QString ret;

    if ( !_native_pattern.isEmpty() && !_native_name.isEmpty() )
    {
        ret += _native_pattern;
        ret += "|";
        ret += _native_name;
        ret += " (";
        ret += _native_pattern;
        ret += ")";
    }

    QStringList mimes;
    for( unsigned int i = 0; i < vec.count(); ++i )
    {
        QStringList toadd = direction == Import ? vec[i].import : vec[i].export_;
        QStringList::ConstIterator it = toadd.begin();
        for ( ; it != toadd.end() ; ++it )
            if ( !mimes.contains( *it ) )
                mimes += *it;
    }

    QStringList::ConstIterator it = mimes.begin();
    for ( ; it != mimes.end() ; ++it )
    {
        QString mime ( *it );
        KMimeType::Ptr t = KMimeType::mimeType( mime );
        // Did we get exactly this mime type ?
        if ( t && mime == t->name() )
        {
            if ( !t->patterns().isEmpty() )
            {
                if ( !ret.isEmpty() )
                    ret += "\n";
                QString patterns = t->patterns().join(" ");
                ret += patterns;
                ret += "|";
                ret += t->comment();
                ret += " (";
                ret += patterns;
                ret += ")";
            }
        }
        else // The filter desktop file talks about an unknown mimetype... This shouldn't happen.
        {
            kdWarning() << "Unknown mimetype " << mime << endl;
            if ( !ret.isEmpty() )
                ret += "\n";
            ret += "*|";
            ret += "Unknown mimetype " + mime;
            ret += " (*)";
        }
    }
    if( allfiles )
    {
        if ( !ret.isEmpty() )
            ret += "\n";
        ret += "*|";
        ret += i18n( "All files (*)" );
    }
    return ret;
}

bool KoFilterManager::prepareDialog( KFileDialog *dialog,
                                     const Direction &direction,
                                     const char *_format,
                                     const QString &
#if KDE_VERSION < 220
                                      _native_pattern
#endif
                                     , const QString &_native_name,
                                     bool
#if KDE_VERSION < 220
                                      allfiles
#endif
                                               ) {

    QString nativeFormat = QString::fromLatin1(_format);
    QString constraint;
    if ( !nativeFormat.isEmpty() ) // format is empty for koshell
    {
        constraint = "'";
        constraint += nativeFormat;
        constraint += "' in ";
        if ( direction == Import )
            constraint += "Export";
        else
            constraint += "Import";
    }

    d->config=QString::null;   // reset the config string

#if KDE_VERSION >= 220 // kdelibs > 2.1 -> use the nice setMimeFilter
    QValueList<KoFilterEntry> vec = KoFilterEntry::query( constraint );

    QStringList mimes;

    if ( !_native_name.isEmpty() )
        mimes += nativeFormat;
    for( unsigned int i = 0; i < vec.count(); ++i )
    {
        QStringList toadd = direction == Import ? vec[i].import : vec[i].export_;
        QStringList::ConstIterator it = toadd.begin();
        for ( ; it != toadd.end() ; ++it )
            if ( !mimes.contains( *it ) )
                mimes += *it;
    }
    if ( direction == Import )
        dialog->setMimeFilter( mimes );
    else
        dialog->setMimeFilter( mimes, nativeFormat );

#else // kdelibs == 2.1
    dialog->setFilter(fileSelectorList(direction, _format, _native_pattern,
                                       _native_name, allfiles));
#endif

    QValueList<KoFilterDialogEntry> vec1 = KoFilterDialogEntry::query( constraint );

    d->ps=new PreviewStack(0L, "preview stack", this);

    unsigned int id=1;                 // id for the next widget

    for(unsigned int i=0; i<vec1.count(); ++i) {
        KMimeType::Ptr t;
        QStringList mimes;
        if ( direction == Import )
            mimes = vec1[i].import;
        else
            mimes = vec1[i].export_;

        QStringList::ConstIterator it = mimes.begin();
        for ( ; it != mimes.end() ; ++it )
        {
            QString mime ( *it );
            t = KMimeType::mimeType( mime );
            // Did we get exactly this mime type ?
            if ( t && mime == t->name() )
            {
                KoFilterDialog *filterdia=vec1[i].createFilterDialog();
                if (!filterdia)
                    continue;
                QStringList patterns = t->patterns();
                QString tmp;
                unsigned short k;
                for(unsigned int j=0; j<patterns.count(); ++j) {
                    tmp=patterns[j];
                    k=0;

                    while(tmp[tmp.length()-k]!=QChar('.')) {
                        ++k;
                    }
                    d->dialogMap.insert(tmp.right(k), id);
                }
                d->ps->addWidget(filterdia, id);
                d->originalDialogs.insert(id, filterdia);
                ++id;
            }
        }
    }
    if(!d->dialogMap.isEmpty()) {
        dialog->setPreviewWidget(d->ps);
        if(direction==Export) {
            QObject::connect(dialog, SIGNAL(filterChanged(const QString &)),
                             d->ps, SLOT(filterChanged(const QString &)));
        }
    }
    return true;
}

void KoFilterManager::cleanUp() {

    if(!d->dialogMap.isEmpty() && d->ps!=0L && !d->ps->isHidden()) {
        int id=d->ps->id(d->ps->visibleWidget());
        if(id!=0) {
            KoFilterDialog *dia=d->originalDialogs.find(id).data();
            if(dia!=0L) {
                d->config=dia->state();
                kdDebug(s_area) << d->config << endl;
            }
            else
                kdWarning(s_area) << "default dia - no config!" << endl;
        }
    }
}

const int KoFilterManager::findWidget(const QString &ext) const {

    QMap<QString, int>::Iterator it=d->dialogMap.find(ext);

    if(it!=d->dialogMap.end())
        return it.data();
    else
        return 0;  // default Widget
}

QString KoFilterManager::import( const QString &_file, QString &mimeType,
                                 const QString &config, const QString &storePrefix )
{
    KURL url;
    url.setPath( _file );

    // Find the mime type for the file to be imported.
    KMimeType::Ptr t = KMimeType::findByURL( url, 0, true );
    if ( t && t->name()!="application/octet-stream" ) {
        mimeType = t->name();
    }
    else {
        kdError(s_area) << "No MimeType found for " << _file << endl;
        return QString::null;
    }

    // Now find the list of filters that can deal with this file.
    QString constr = "'";
    constr += mimeType;
    constr += "' in Import";
    QValueList<KoFilterEntry> vec = KoFilterEntry::query( constr );
    if ( vec.isEmpty() )
    {
        kdError(s_area) << "No filter found for " << mimeType << endl;
        return QString::null;
    }

    // Run the filters in turn until one is found that works.
    unsigned int i=0;
    QString ok=QString::null;
    d->config=config;
    while(i<vec.count()) {
        unsigned int j=0;
        while(j<vec[i].export_.count()) {
            ok=import( _file, vec[i].export_[j].local8Bit(), (KoDocument *)0L, storePrefix );
            if (ok != QString::null) {
                // Set the resulting mimeType to that of the filter output that worked.
                mimeType=vec[i].export_[j];
                return ok;
            }
            ++j;
        }
        ++i;
    }

    // Return failure.
    kdError(s_area) << "All filters failed." << endl;
    return QString::null;
}

QString KoFilterManager::import( const QString &_file, const char *_native_format,
                                 KoDocument *document, const QString &storePrefix )
{
    KURL url;
    url.setPath( _file );
    //QString file = QFile::encodeName( _file ); // The 8bit version of the filename
    QString file = _file;			//less is sometimes more, Werner :)

    KMimeType::Ptr t = KMimeType::findByURL( url, 0, true );
    QString mimeType;
    if ( t && t->name()!="application/octet-stream" ) {
        mimeType = t->name();
    }
    else {
        kdDebug(s_area) << "No MimeType found. Setting " << _native_format << endl;
        mimeType = QString::fromLatin1(_native_format);
    }

    if ( mimeType == _native_format || mimeType == "application/x-gzip" || mimeType == "text/xml" )
    {
        return _file;
    }

    QString constr = "'";
    constr += _native_format;
    constr += "' in Export and '";
    constr += mimeType;
    constr += "' in Import";

    QValueList<KoFilterEntry> vec = KoFilterEntry::query( constr );
    if ( vec.isEmpty() )
    {
        QString tmp = i18n("Could not import file of type\n%1").arg( t->name() );
        QApplication::restoreOverrideCursor();
        KMessageBox::error( 0L, tmp, i18n("Missing import filter") );
        return QString::null;
    }

    unsigned int i=0;
    bool ok=false;
    // just in case that there are more than one filters
    while(i<vec.count() && !ok) {
        // first check the "external" case
        if(vec[i].implemented.lower()=="bulletproof") {
            KProcess *process=new KProcess();
            *process << "filter_wrapper";
            *process << constr << file << storePrefix << mimeType
                     << _native_format << d->config << QString::number(i);
            connect(process, SIGNAL(processExited(KProcess *)), SLOT(processExited(KProcess *)));
            connect(process, SIGNAL(receivedStdout(KProcess *, char *, int)),
                                    SLOT(receivedStdout(KProcess *, char *, int)));
            kdDebug(s_area) << "################### starting wrapper process ###################" << endl;
            process->start(KProcess::NotifyOnExit, KProcess::Stdout);
            kapp->enter_loop();  // yay, l33t h4XOr :)
            ok=d->exitCode==0;
            ++i;
            continue;
        }

        KoFilter* filter = vec[i].createFilter();
        if( !filter )
        {
            QString tmp = i18n("Could not import file of type\n%1").arg( t->name() );
            QApplication::restoreOverrideCursor();
            // ### use KLibLoader::lastErrorMessage() here
            KMessageBox::error( 0L, tmp, i18n("Import filter failed") );
            // ## set busy cursor again
            ++i;
            continue;
        }

        // Allow for document to be null, since we might be invoked from a context where
        // it is not available (e.g. within an OLEfilter filter).
        if (document)
        {
            QObject::connect(filter, SIGNAL(sigProgress(int)), document, SIGNAL(sigProgress(int)));
            document->emitProgress(0);
        }
        if(vec[i].implemented.lower()=="file") {
            //kdDebug(s_area) << "XXXXXXXXXXX file XXXXXXXXXXXXXX" << endl;
	    //kdDebug(s_area) << "file: " << file << endl;
            KTempFile tempFile; // create with default file prefix, extension and mode
            if (tempFile.status() != 0)
                return QString::null;
            d->tempfname=tempFile.name();
            if (filter->supportsEmbedding())
                ok=filter->filter( file, d->tempfname, storePrefix, mimeType, _native_format, d->config );
            else
                ok=filter->filter( file, d->tempfname, mimeType, _native_format, d->config );
            // tempfname=tempFile.name(); // hack for -DQT_NO_BLAH stuff <-- huh??? (Werner)
        }
        else if(vec[i].implemented.lower()=="qdom") {
            //kdDebug(s_area) << "XXXXXXXXXXX qdom XXXXXXXXXXXXXX" << endl;
	    //kdDebug(s_area) << "file: " << file << endl;
            QDomDocument qdoc;
            ok=filter->filterImport( file, mimeType, qdoc, _native_format, d->config);
            if(ok) {
                ok=document->loadXML(0L,qdoc);
                if (!ok)
                  kdWarning(s_area) << "loadXML FAILED !" << endl;
                document->changedByFilter();
            }
        }
        else if(vec[i].implemented.lower()=="kodocument") {
            //kdDebug(s_area) << "XXXXXXXXXXX kodocument XXXXXXXXXXXXXX" << endl;
	    //kdDebug(s_area) << "file: " << file << endl;
            ok=filter->filterImport( file, document, mimeType, _native_format, d->config);
            if(ok)
                document->changedByFilter();
        }
        if (document)
        {
            document->emitProgress(-1);  // remove the bar
        }
        delete filter;
        ++i;
    }
    if(ok && (vec[i-1].implemented.lower()=="file" || vec[i-1].implemented.lower()=="bulletproof"))
        return d->tempfname;

    // Return failure.
    kdError(s_area) << "All filters failed." << endl;
    return QString::null;
}

QString KoFilterManager::prepareExport( const QString & file,
                                        const QCString & _native_format,
                                        const QCString & outputFormat,
                                        KoDocument *document )
{
    d->exportFile=file;
    d->native_format=QString::fromLatin1(_native_format);
    d->document=document;
    KURL url( d->exportFile );

    ASSERT( outputFormat != _native_format );

    d->mime_type=outputFormat;   // needed for export_ :)

    QString constr = "'";
    constr += outputFormat;
    constr += "' in Export and '";
    constr += _native_format;
    constr += "' in Import";

    QValueList<KoFilterEntry> vec = KoFilterEntry::query( constr );
    if ( vec.isEmpty() )
    {
        QString tmp = i18n("Could not export file of type\n%1").arg( outputFormat );
        QApplication::restoreOverrideCursor();
        KMessageBox::error( 0L, tmp, i18n("Missing export filter") );
        return QString::null;
    }

    d->m_vec=vec;

    unsigned int i=0;
    bool ok=false;
    bool tmpFileNeeded=false;

    while(i<vec.count() && !ok) {
        KoFilter* filter = vec[i].createFilter();
        if ( !filter )
        {
            QApplication::restoreOverrideCursor();
            KMessageBox::error( 0L, i18n("Could not export file of type\n%1").arg( outputFormat ),
                                i18n("Export filter failure") );
            // ### use KLibLoader::lastErrorMessage() here. Was it in kdelibs-2.1 ?
            // ## set busy cursor again
        }
        else
        {
            QObject::connect(filter, SIGNAL(sigProgress(int)), document, SIGNAL(sigProgress(int)));
            if(vec[i].implemented.lower()=="file")
                tmpFileNeeded=true;
            else if(vec[i].implemented.lower()=="kodocument") {
                ok=filter->filterExport(file, document, _native_format, outputFormat, d->config);
                // if(ok)
                //  document->changedByFilter();
                document->emitProgress(-1);
            }
            QObject::disconnect(filter, SIGNAL(sigProgress(int)), document, SIGNAL(sigProgress(int)));
            delete filter;
        }
        ++i;
    }

    if(!ok && tmpFileNeeded) {
        KTempFile tempFile; // create with default file prefix, extension and mode
        if (tempFile.status() != 0)
            return file;
        d->tmpFile = tempFile.name();
        d->prepare=true;
        return d->tmpFile;
    }
    return file;
}

bool KoFilterManager::export_() {
    d->prepare=false;

    unsigned int i=0;
    bool ok=false;
    while(i<d->m_vec.count() && !ok) {
        if(d->m_vec[i].implemented.lower()=="file") {
            KoFilter* filter = d->m_vec[i].createFilter();
            if( !filter )
                return false; // error already emitted in prepareExport
            QObject::connect(filter, SIGNAL(sigProgress(int)), d->document, SIGNAL(sigProgress(int)));
            ok=filter->filter(d->tmpFile, d->exportFile, d->native_format, d->mime_type, d->config );
            d->document->emitProgress(-1);
            QObject::disconnect(filter, SIGNAL(sigProgress(int)), d->document, SIGNAL(sigProgress(int)));
            delete filter;
        }
        ++i;
    }
    // Done, remove temporary file
    unlink( d->tmpFile.local8Bit() );
    return ok;
}

void KoFilterManager::incRef()
{
  s_refCnt++;
}

void KoFilterManager::decRef()
{
  s_refCnt--;
  if ( s_refCnt == 0 && s_pSelf )
  {
    delete s_pSelf;
    s_pSelf = 0;
  }
}

unsigned long KoFilterManager::refCnt()
{
  return s_refCnt;
}

void KoFilterManager::processExited(KProcess *p) {

    if(p->normalExit())
        d->exitCode=p->exitStatus();
    kdDebug(s_area) << "################### wrapper process exited: "
                    << d->exitCode << " ###################" << endl;
    kapp->exit_loop();
}

void KoFilterManager::receivedStdout(KProcess */*p*/, char *buffer, int buflen) {

    kdDebug() << "KoFilterManager::receivedStdout  -- len: " << buflen << "buffer: " << buffer << endl;
    if(d->document && buflen>0 && buffer[0]=='P') {
        QCString tmp(++buffer, buflen-1);
        bool ok=true;
        int progress=tmp.toInt(&ok);
        if(ok)
            d->document->emitProgress(progress);
    }
    else if(buflen>0 && buffer[0]=='F') {
        QCString tmp(++buffer, buflen-1);
        d->tempfname=tmp;
    }
}


//////////////////////////////////////////////////////////////////////////////
// PreviewStack

PreviewStack::PreviewStack(QWidget *parent, const char *name,
                           KoFilterManager *m) : QWidgetStack(parent, name),
                           mgr(m), hidden(false) {
}

PreviewStack::~PreviewStack() {
}

void PreviewStack::showPreview(const KURL &url) {

    QString tmp=url.url();
    unsigned short k=0;
    unsigned int foo=tmp.length();

    // try to find the extension
    while(tmp[foo-k]!=QChar('.') && k<=foo) {
        ++k;
    }
    change(tmp.right(k));
}

void PreviewStack::filterChanged(const QString &filter) {
    change(filter.mid(1));
}

void PreviewStack::change(const QString &ext) {

    if(ext.isNull() || ext[0]!='.') {
        if(!hidden) {
            hide();
            hidden=true;
        }
        return;
    }
    // do we have a dialog for that extension? (0==we don't have one)
    unsigned short id=mgr->findWidget(ext);

    if(id==0) {
        if(!hidden) {
            hide();
            hidden=true;
        }
        return;
    }
    else {
        raiseWidget(id);
        if(hidden) {
            show();
            hidden=false;
        }
    }
}

#include <koFilterManager.moc>
