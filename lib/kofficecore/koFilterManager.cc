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

#include "koFilterManager.h"
#include "koQueryTypes.h"
#include "koFilter.h"

#include <klocale.h>
#include <kmimetype.h>
#include <kdebug.h>

#include <qmessagebox.h>
#include <qstringlist.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ktempfile.h>
/*
#ifdef HAVE_PATHS_H
#include <paths.h>
#endif
#ifndef _PATH_TMP
#define _PATH_TMP "/tmp"
#endif
*/

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

const QString KoFilterManager::import( const QString & _url, const char *_native_format )
{
    KURL url( _url );

    KMimeType::Ptr t = KMimeType::findByURL( url, 0, url.isLocalFile() );
    QCString mimeType;
    if (t) {
        kdebug( KDEBUG_INFO, 30003, "######### FOUND MimeType %s", t->mimeType().data() );
        mimeType = t->mimeType();
    }
    else {
        kdebug( KDEBUG_INFO, 30003, "####### No MimeType found. findByURL returned 0. Setting text/plain" );
        mimeType = "text/plain";
    }

    if ( mimeType == _native_format )
    {
        kdebug( KDEBUG_INFO, 30003, "strcmp( mimeType, _native_format ) == 0 !! Returning without conversion. " );
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
        return _url;
    }

    KTempFile tempFile; // create with default file prefix, extension and mode
    if (tempFile.status() != 0)
        return _url;
    QString tempfname = tempFile.name();

    /*
    char tempfname[256];
    sprintf(tempfname, _PATH_TMP"/kofficefilterXXXXXX");
    if (mkstemp(tempfname) == -1)
      return _url;
    */

    unsigned int i=0;
    bool ok=false;
    while(i<vec.count() && !ok) {
        KoFilter* filter = vec[i].createFilter();
        ASSERT( filter );
        ok=filter->filter( QCString(_url), QCString(tempfname), QCString(mimeType), QCString(_native_format) );
        delete filter;
        ++i;
    }
    return tempfname;
}

const QString KoFilterManager::prepareExport( const QString & _url, const char *_native_format )
{
    exportFile=_url;
    native_format=_native_format;

    KTempFile tempFile; // create with default file prefix, extension and mode
    if (tempFile.status() != 0)
        return _url;
    tmpFile = tempFile.name();

    /*
    char tempfname[256];
    int fildes;
    sprintf(tempfname, _PATH_TMP"/kofficefilterXXXXXX");
    if ((fildes = mkstemp(tempfname)) == -1 )
        return _url;
    tmpFile=tempfname;
    */

    prepare=true;
    return tmpFile;
}

const bool KoFilterManager::export_() {

    prepare=false;

    KURL url( exportFile );

    KMimeType::Ptr t = KMimeType::findByURL( url, 0, url.isLocalFile() );
    QCString mimeType;
    if (t) {
        kdebug( KDEBUG_INFO, 30003, "######### FOUND MimeType %c", t->mimeType().ascii() );
        mimeType = t->mimeType();
    }
    else {
        kdebug( KDEBUG_INFO, 30003, "####### No MimeType found. findByURL returned 0. Setting text/plain" );
        mimeType = "text/plain";
    }

    if ( (strcmp( mimeType, native_format ) == 0) )
    {
        kdebug( KDEBUG_INFO, 30003, "strcmp( mimeType, _native_format ) == 0 !! Returning without conversion. " );
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
    return true;
}
