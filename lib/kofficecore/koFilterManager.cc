/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include <koFilterManager.h>
#include <koFilterManager.moc>

KoFilterManager* KoFilterManager::s_pSelf = 0;

KoFilterManager* KoFilterManager::self()
{
    if( s_pSelf == 0 )
    {
        s_pSelf = new KoFilterManager;
        s_pSelf->prepare=false;
    }
    return s_pSelf;
}

const QString KoFilterManager::fileSelectorList( const Direction &direction, const char *_format,
                                           const char *_native_pattern,
                                           const char *_native_name,
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
#ifndef USE_QFD
        ret += _native_pattern;
        ret += "|";
#endif
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
        // Did we get exact this mime type ?
        if ( t && mime == t->mimeType() )
        {
            QStringList patterns = t->patterns();
            const char* s;
            for(unsigned int j = 0;j < patterns.count();j++)
            {
                s = patterns[j];
                if ( !ret.isEmpty() )
                    ret += "\n";
#ifndef USE_QFD
                ret += s;
                ret += "|";
#endif
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
#ifndef USE_QFD
            ret += "*.*|";
#endif
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
#ifndef USE_QFD
        ret += "*.*|";
#endif
        ret += i18n( "All files (*.*)" );
    }
    return ret;
}

const bool KoFilterManager::prepareDialog( KFileDialog *dialog,
                                 const Direction &direction,
                                 const char *_format,
                                 const char *_native_pattern,
                                 const char *_native_name,
                                 const bool allfiles ) {

    unsigned i, j;
    QString service;
    if ( direction == Import )
        service = "Export == '";
    else
        service = "Import == '";
    service += _format;
    service += "'";

    QValueList<KoFilterEntry> vec = KoFilterEntry::query( service );

    QString filters;

    if ( _native_pattern && _native_name )
    {
#ifndef USE_QFD
        filters += _native_pattern;
        filters += "|";
#endif
        filters += _native_name;
        filters += " (";
        filters += _native_pattern;
        filters += ")";
    }

    for( i = 0; i < vec.count(); ++i )
    {
        KMimeType::Ptr t;
        QString mime;
        if ( direction == Import )
            mime = vec[i].import;
        else
            mime = vec[i].export_;

        t = KMimeType::mimeType( mime );
        // Did we get exact this mime type ?
        if ( t && mime == t->mimeType() )
        {
            QStringList patterns = t->patterns();
            const char* s;
            for( j = 0; j < patterns.count(); ++j )
            {
                s = patterns[j];
                if ( !filters.isEmpty() )
                    filters += "\n";
#ifndef USE_QFD
                filters += s;
                filters += "|";
#endif
                if ( direction == Import )
                    filters += vec[i].importDescription;
                else
                    filters += vec[i].exportDescription;
                filters += " (";
                filters += s;
                filters += ")";
            }
        }
        else
        {
            if ( !filters.isEmpty() )
                filters += "\n";
#ifndef USE_QFD
            filters += "*.*|";
#endif
            if ( direction == Import )
                filters += vec[i].importDescription;
            else
                filters += vec[i].exportDescription;
            filters += " (*.*)";
        }
    }
    if( allfiles )
    {
        if ( !filters.isEmpty() )
            filters += "\n";
#ifndef USE_QFD
        filters += "*.*|";
#endif
        filters += i18n( "All files (*.*)" );
    }

    dialog->setFilter(filters);

    QValueList<KoFilterDialogEntry> vec1 = KoFilterDialogEntry::query( service );

    ps=new PreviewStack(0L, "preview stack", this);
    QWidget *d=new QWidget(ps, "default preview");
    QString tmp=i18n("Sorry, no preview available.");
    QLabel *l=new QLabel(i18n(tmp), d);
    l->setMinimumSize(l->sizeHint());
    l->move(10, 5);
    d->setMinimumWidth(l->sizeHint().width()+20);
    ps->addWidget(d, 0);  // default Widget

    //QMap<QString, long> dialogMap;    // QString==pattern, long==id in Widgetstack
    unsigned long id;                 // id for the next widget

    for(i=0, id=1; i<vec1.count(); ++i) {
        KMimeType::Ptr t;
        QString mime;
        if ( direction == Import )
            mime = vec1[i].import;
        else
            mime = vec1[i].export_;

        t = KMimeType::mimeType( mime );
        // Did we get exactly this mime type ?
        if ( t && mime == t->mimeType() )
        {
            KoFilterDialog *filterdia=vec1[i].createFilterDialog();
            ASSERT(filterdia);

            QStringList patterns = t->patterns();
            QString tmp;
            unsigned short k;
            for(j=0; j<patterns.count(); ++j) {
                tmp=patterns[j];
                k=0;

                while(tmp[tmp.length()-k]!=QChar('.')) {
                    ++k;
                }
                kdebug(KDEBUG_INFO, 31000, "Extension:");
                kdebug(KDEBUG_INFO, 31000, tmp.right(k));
                dialogMap.insert(tmp.right(k), id);
            }
            ps->addWidget(filterdia, id);
            ++id;
        }
    }
    if(!dialogMap.isEmpty()) {
        kdebug(KDEBUG_INFO, 31000, "----------------------------- setPreviewWidget");
        ps->raiseWidget(0);
        dialog->setPreviewWidget(ps);
    }
    return true;
}

void KoFilterManager::cleanUp() {

    if(!dialogMap.isEmpty()) {
        QWidget *tmp=ps->visibleWidget();
        if(tmp!=ps->widget(0)) {
            KoFilterDialog *dia=dynamic_cast<KoFilterDialog*>(tmp);
            kdebug(KDEBUG_INFO, 31000, dia->status());
        }
    }
}

long KoFilterManager::findWidget(const QString &ext) const {

    QMap<QString, long>::Iterator it;
    it=dialogMap.find(ext);

    if(it!=dialogMap.end())
        return it.data();
    else
        return 0;  // default Widget
}

const QString KoFilterManager::import( const QString & _url, const char *_native_format )
{
    KURL url( _url );

    KMimeType::Ptr t = KMimeType::findByURL( url, 0, url.isLocalFile() );
    QCString mimeType;
    if (t) {
        kDebugInfo( 30003, "######### FOUND MimeType %s", t->mimeType().data() );
        mimeType = t->mimeType();
    }
    else {
        kDebugInfo( 30003, "####### No MimeType found. findByURL returned 0. Setting text/plain" );
        mimeType = "text/plain";
    }

    if ( mimeType == _native_format )
    {
        kDebugInfo( 30003, "strcmp( mimeType, _native_format ) == 0 !! Returning without conversion. " );
        assert( url.isLocalFile() );
        return _url;
    }

    QString constr = "Export == '";
    constr += _native_format;
    constr += "' and Import == '";
    constr += mimeType;
    constr += "'";

    QValueList<KoFilterEntry> vec = KoFilterEntry::query( constr );
    if ( vec.isEmpty() )
    {
        QString tmp;
        tmp.sprintf( i18n("Could not import file of type\n%s"), t->mimeType().ascii() );
        QMessageBox::critical( 0L, i18n("Missing import filter"), tmp, i18n("OK") );
        return "";
    }

    KTempFile tempFile; // create with default file prefix, extension and mode
    if (tempFile.status() != 0)
        return "";
    QString tempfname = tempFile.name();

    unsigned int i=0;
    bool ok=false;
    while(i<vec.count() && !ok) {
        KoFilter* filter = vec[i].createFilter();
        ASSERT( filter );
        ok=filter->filter( QCString(_url), QCString(tempfname), QCString(mimeType), QCString(_native_format) );
        delete filter;
        ++i;
    }
    return ok ? tempfname : QString("");
}

const QString KoFilterManager::prepareExport( const QString & _url, const char *_native_format )
{
    exportFile=_url;
    native_format=_native_format;

    KTempFile tempFile; // create with default file prefix, extension and mode
    if (tempFile.status() != 0)
        return _url;
    tmpFile = tempFile.name();
    prepare=true;
    return tmpFile;
}

const bool KoFilterManager::export_() {

    prepare=false;

    KURL url( exportFile );

    KMimeType::Ptr t = KMimeType::findByURL( url, 0, url.isLocalFile() );
    QCString mimeType;
    if (t) {
        kDebugInfo( 30003, "######### FOUND MimeType %c", t->mimeType().ascii() );
        mimeType = t->mimeType();
    }
    else {
        kDebugInfo( 30003, "####### No MimeType found. findByURL returned 0. Setting text/plain" );
        mimeType = "text/plain";
    }

    if ( (strcmp( mimeType, native_format ) == 0) )
    {
        kDebugInfo( 30003, "strcmp( mimeType, _native_format ) == 0 !! Returning without conversion. " );
        assert( url.isLocalFile() );
        return false;
    }

    QString constr = "Export == '";
    constr += mimeType;
    constr += "' and Import == '";
    constr += native_format;
    constr += "'";

    QValueList<KoFilterEntry> vec = KoFilterEntry::query( constr );
    if ( vec.isEmpty() )
    {
        QString tmp;
        tmp.sprintf( i18n("Could not export file of type\n%s"), t->mimeType().ascii() );
        QMessageBox::critical( 0L, i18n("Missing export filter"), tmp, i18n("OK") );
        return false;
    }

    unsigned int i=0;
    bool ok=false;
    while(i<vec.count() && !ok) {
        KoFilter* filter = vec[i].createFilter();
        ASSERT( filter );
        ok=filter->filter( QCString(tmpFile), QCString(exportFile), QCString(native_format), QCString(mimeType) );
        delete filter;
        ++i;
    }
    // Done, remove temporary file
    unlink( tmpFile.ascii() );
    return true;
}


//////////////////////////////////////////////////////////////////////////////
// PreviewStack

PreviewStack::PreviewStack(QWidget *parent, const char *name,
                           KoFilterManager *m) : QWidgetStack(parent, name),
                           mgr(m) {
}

PreviewStack::~PreviewStack() {
}

void PreviewStack::showPreview(const KURL &url) {

    QString tmp=url.url();
    QString extension;
    unsigned short k=0;
    unsigned long foo=tmp.length();

    kdebug(KDEBUG_INFO, 31000, "vorher --------- showPreview");

    while(tmp[foo-k]!=QChar('.') && k<=foo) {
        ++k;
    }
    kdebug(KDEBUG_INFO, 31000, "vor if --------- showPreview");
    if(tmp[foo-k]==QChar('.')) {
        extension=tmp.right(k);
        kdebug(KDEBUG_INFO, 31000, extension);
        raiseWidget(mgr->findWidget(extension));
        kdebug(KDEBUG_INFO, 31000, "im if --------- showPreview");
    }
    else
        raiseWidget(0);

    kdebug(KDEBUG_INFO, 31000, "ende --------- showPreview");
}
