/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
                 2000 Werner Trobin <wtrobin@mandrakesoft.com>

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
#include <qapp.h>

#include <kmimetype.h>
#include <kurl.h>
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
#include <assert.h>


class KoFilterManagerPrivate {

public:
    KoFilterManagerPrivate() { prepare=false; }
    ~KoFilterManagerPrivate() {}

    QString tmpFile;
    QString exportFile;
    QString native_format, mime_type;
    bool prepare;
    QValueList<KoFilterEntry> m_vec;
    PreviewStack *ps;
    mutable QMap<QString, int> dialogMap;
    QMap<int, KoFilterDialog*> originalDialogs;
    QString config;  // stores the config information
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

const QString KoFilterManager::fileSelectorList( const Direction &direction,
                                                 const char *_format,
                                                 const QString & _native_pattern,
                                                 const QString & _native_name,
                                                 const bool allfiles ) const
{
    QString service;
    if ( direction == Import )
        service = "Export == '";
    else
        service = "Import == '";
    service += _format;
    service += "'";

    QValueList<KoFilterEntry> vec = KoFilterEntry::query( service );

    QString ret;

    if ( _native_pattern && _native_name )
    {
        ret += _native_pattern;
        ret += "|";
        ret += _native_name;
        ret += " (";
        ret += _native_pattern;
        ret += ")";
    }

    for( unsigned int i = 0; i < vec.count(); ++i )
    {
        KMimeType::Ptr t;
        QString mime;
        if ( direction == Import )
            mime = vec[i].import;
        else
            mime = vec[i].export_;

        t = KMimeType::mimeType( mime );
        // Did we get exactly this mime type ?
        if ( t && mime == t->name() )
        {
            QStringList patterns = t->patterns();
            const char* s;
            for(unsigned int j = 0;j < patterns.count();j++)
            {
                s = patterns[j];
                if ( !ret.isEmpty() )
                    ret += "\n";
                ret += s;
                ret += "|";
                if ( direction == Import )
                    ret += vec[i].importDescription;
                else
                    ret += vec[i].exportDescription;
                ret += " (";
                ret += s;
                ret += ")";
            }
        }
        else
        {
            if ( !ret.isEmpty() )
                ret += "\n";
            ret += "*.*|";
            if ( direction == Import )
                ret += vec[i].importDescription;
            else
                ret += vec[i].exportDescription;
            ret += " (*.*)";
        }
    }
    if( allfiles )
    {
        if ( !ret.isEmpty() )
            ret += "\n";
        ret += "*.*|";
        ret += i18n( "All files (*.*)" );
    }
    return ret;
}

const bool KoFilterManager::prepareDialog( KFileDialog *dialog,
                                 const Direction &direction,
                                 const char *_format,
                                 const QString & _native_pattern,
                                 const QString & _native_name,
                                 const bool allfiles ) {

    QString service;
    if ( direction == Import )
        service = "Export == '";
    else
        service = "Import == '";
    service += _format;
    service += "'";

    d->config=QString::null;   // reset the config string

    dialog->setFilter(fileSelectorList(direction, _format, _native_pattern,
				       _native_name, allfiles));

    QValueList<KoFilterDialogEntry> vec1 = KoFilterDialogEntry::query( service );

    d->ps=new PreviewStack(0L, "preview stack", this);

    unsigned int id=1;                 // id for the next widget

    for(unsigned int i=0; i<vec1.count(); ++i) {
        KMimeType::Ptr t;
        QString mime;
        if ( direction == Import )
            mime = vec1[i].import;
        else
            mime = vec1[i].export_;

        t = KMimeType::mimeType( mime );
        // Did we get exactly this mime type ?
        if ( t && mime == t->name() )
        {
            KoFilterDialog *filterdia=vec1[i].createFilterDialog();
            ASSERT(filterdia);

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
                kdDebug(30003) << d->config << endl;
            }
            else
                kdWarning(30003) << "default dia - no config!" << endl;
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

const QString KoFilterManager::import( const QString & _file, const char *_native_format,
				       KoDocument *document )
{
    QString path( _file );
    KURL::encode( path ); // Encode local paths before creating a KURL from them
    KURL url( path );
    QCString file = QFile::encodeName( _file ); // The 8bit version of the filename

    KMimeType::Ptr t = KMimeType::findByURL( url, 0, true );
    QCString mimeType;
    if ( t && t->name()!="application/octet-stream" ) {
        kdDebug(30003) << "Found MimeType " << t->name() << endl;
        mimeType = t->name();
    }
    else {
        kdDebug(30003) << "No MimeType found. Setting text/plain" << endl;
        mimeType = "text/plain";
    }

    if ( mimeType == _native_format )
    {
        return _file;
    }

    QString constr = "Export == '";
    constr += _native_format;
    constr += "' and Import == '";
    constr += mimeType;
    constr += "'";

    QValueList<KoFilterEntry> vec = KoFilterEntry::query( constr );
    if ( vec.isEmpty() )
    {
        QString tmp = i18n("Could not import file of type\n%1").arg( t->name() );
        QApplication::restoreOverrideCursor();
        KMessageBox::error( 0L, tmp, i18n("Missing import filter") );
        return "";
    }

    unsigned int i=0;
    bool ok=false;
    QCString tempfname;
    // just in case that there are more than one filters
    while(i<vec.count() && !ok) {
	KoFilter* filter = vec[i].createFilter();
        ASSERT( filter );

	if(vec[i].implemented.lower()=="file") {
	    //kdDebug(30003) << "XXXXXXXXXXX file XXXXXXXXXXXXXX" << endl;
	    KTempFile tempFile; // create with default file prefix, extension and mode
	    if (tempFile.status() != 0)
		return "";
	    tempfname = QFile::encodeName(tempFile.name());
	    ok=filter->filter( file, tempfname, mimeType, _native_format, d->config );
	}
	else if(vec[i].implemented.lower()=="qdom" &&                // As long as some parts use KOML we have to make
		(strcmp(document->className(), "KSpreadDoc")==0 ||   // sure that we only allow parts which implement
		 strcmp(document->className(), "KChartPart")==0 ||   // the right loadXML :)
		 strcmp(document->className(), "KisDoc")==0 ||
		 strcmp(document->className(), "KImageDocument")==0)) {
	    //kdDebug(30003) << "XXXXXXXXXXX qdom XXXXXXXXXXXXXX" << endl;
	    QDomDocument qdoc;
	    ok=filter->I_filter( file, mimeType, qdoc, _native_format, d->config);
	    if(ok) {
		ok=document->loadXML(qdoc);
                if (!ok)
                  kdWarning(30003) << "loadXML FAILED !" << endl;
		document->changedByFilter();
	    }
	}
	else if(vec[i].implemented.lower()=="kodocument") {
	    //kdDebug(30003) << "XXXXXXXXXXX kodocument XXXXXXXXXXXXXX" << endl;
	    ok=filter->I_filter( file, document, mimeType, _native_format, d->config);
	    if(ok)
		document->changedByFilter();
	}
        delete filter;
        ++i;
    }
    if(ok && vec[i-1].implemented.lower()=="file")
	return QFile::decodeName(tempfname);
    return "";
}

const QString KoFilterManager::prepareExport( const QString & file, const char *_native_format,
					      const KoDocument *document )
{
    d->exportFile=file;
    d->native_format=_native_format;
    KURL url( d->exportFile );

    KMimeType::Ptr t = KMimeType::findByURL( url, 0, url.isLocalFile() );
    QCString mimeType;
    if (t && t->name() != "application/octet-stream") {
        kdDebug(30003) << "Found MimeType " << t->name() << endl;
        mimeType = t->name();
    }
    else {
        kdDebug(30003) << "No MimeType found. Setting text/plain" << endl;
        mimeType = "text/plain";
    }

    if ( mimeType == _native_format )
    {
        kdDebug(30003) << "Native format, returning without conversion. " << endl;
        return file;
    }

    d->mime_type=mimeType;   // needed for export_ :)

    QString constr = "Export == '";
    constr += mimeType;
    constr += "' and Import == '";
    constr += _native_format;
    constr += "'";

    QValueList<KoFilterEntry> vec = KoFilterEntry::query( constr );
    if ( vec.isEmpty() )
    {
        QString tmp = i18n("Could not export file of type\n%1").arg( t->name() );
        QApplication::restoreOverrideCursor();
        KMessageBox::error( 0L, tmp, i18n("Missing export filter") );
        return file;
    }

    d->m_vec=vec;

    unsigned int i=0;
    bool ok=false;
    bool tmpFileNeeded=false;

    while(i<vec.count() && !ok) {
        KoFilter* filter = vec[i].createFilter();
        ASSERT( filter );
	
	if(vec[i].implemented.lower()=="file")
	    tmpFileNeeded=true;
	else if(vec[i].implemented.lower()=="kodocument") {
	    ok=filter->E_filter(QCString(file), document, QCString(_native_format), QCString(mimeType), d->config);
	    // if(ok)
	    //	document->changedByFilter();
	}
        delete filter;
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

const bool KoFilterManager::export_() {

    d->prepare=false;

    unsigned int i=0;
    bool ok=false;
    while(i<d->m_vec.count() && !ok) {
	if(d->m_vec[i].implemented.lower()=="file") {
	    KoFilter* filter = d->m_vec[i].createFilter();
	    ASSERT( filter );
	    ok=filter->filter( QCString(d->tmpFile), QCString(d->exportFile), QCString(d->native_format),
			       QCString(d->mime_type), d->config );
	    delete filter;
	}
        ++i;
    }
    // Done, remove temporary file
    unlink( d->tmpFile.local8Bit() );
    return true;
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
