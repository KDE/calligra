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
#include "koffice.h"

#include <kapp.h>
#include <klocale.h>
#include <kregistry.h>
#include <kregfactories.h>
#include <kmimetypes.h>
#include <kmimemagic.h>
#include <kdebug.h>

#include <qmsgbox.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qtextstream.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_PATHS_H
#include <paths.h>
#endif
#ifndef _PATH_TMP
#define _PATH_TMP "/tmp/"
#endif

KoFilterManager* KoFilterManager::s_pSelf = 0;

KoFilterManager* KoFilterManager::self()
{
    if( s_pSelf == 0 )
    {
        s_pSelf = new KoFilterManager;
    }

    return s_pSelf;
}

KRegistry * registry = 0L;

QString KoFilterManager::fileSelectorList( Direction direction, const char *_format,
                                           const char *_native_pattern,
                                           const char *_native_name,
                                           bool allfiles ) const
{
    QString constr;
    if ( direction == Import )
        constr = "Export == '";
    else
        constr = "Import == '";
    constr += _format;
    constr += "'";
    QValueList<KoFilterEntry> vec = KoFilterEntry::query( constr );

    if (!registry)
    {
        registry = new KRegistry;
        registry->addFactory( new KServiceTypeFactory );
        registry->load();
    }

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
        KMimeType *t;
        QString mime;
        if ( direction == Import )
            mime = vec[i].import;
        else
            mime = vec[i].export_;

        t = KMimeType::find( mime );
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
            ret += "*|";
            if ( direction == Import )
                ret += vec[i].importDescription;
            else
                ret += vec[i].exportDescription;
        }
    }

    if( allfiles )
    {
        if ( !ret.isEmpty() )
            ret += "\n";
        ret += "*|";
        ret += i18n( "All files" );
    }

    return ret;
}

QString KoFilterManager::import( const QString & _url, const char *_native_format )
{
    KURL url( _url );

    KMimeType *t = KMimeType::findByURL( url, 0, url.isLocalFile() );
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
        // TODO: fetch remote file!
        assert( url.isLocalFile() );

        return QString( url.url() );
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
        return QString();
    }

    KOffice::Filter::Data data;

    struct stat buff;
    stat( url.path(), &buff );
    unsigned int size = buff.st_size;

    FILE *f = fopen( url.path(), "rb" );
    if ( !f )
    {
        QString tmp;
        tmp.sprintf( i18n("Could not open file\n%s"), url.path().ascii() );
        QMessageBox::critical( 0L, i18n("Error"), tmp, i18n("OK") );
        return QString();
    }

    char *p = new char[ size ];
    int got = fread( p, 1, size, f );
    fclose( f );

    data.setRawData( p, got );

    KOffice::FilterFactory_var factory;
    factory = KOffice::FilterFactory::_narrow( vec[0].reference );
    assert( !CORBA::is_nil( factory ) );
    KOffice::Filter_var filter = factory->create();
    assert( !CORBA::is_nil( filter ) );

    filter->filter( data, mimeType, _native_format );

    filter->destroy();

    char tempfname[256];
    int fildes; 
    sprintf(tempfname, _PATH_TMP"/kofficefilterXXXXXX");
    if ((fildes = mkstemp(tempfname)) == -1 )
      return QString::null;
    f = fdopen(fildes, "w" );
    if ( !f )
    {
        QString tmp;
        tmp.sprintf( i18n("Could not write file\n%s"), "/tmp/kofficefilter" );
        QMessageBox::critical( 0L, i18n("Error"), tmp, i18n("OK") );
        return QString();
    }

    fwrite( data.data(), 1, data.size(), f );
    fclose( f );

    return tempfname;
}

void KoFilterManager::export_( const QString & _tmpFile, const QString & _url, const char *_native_format )
{
    KURL url( _url );

    KMimeType *t = KMimeType::findByURL( url, 0, url.isLocalFile() );
    QCString mimeType;
    if (t) {
        kdebug( KDEBUG_INFO, 30003, "######### FOUND MimeType %c", t->mimeType().ascii() );
        mimeType = t->mimeType();
    }
    else {
        kdebug( KDEBUG_INFO, 30003, "####### No MimeType found. findByURL returned 0. Setting text/plain" );
        mimeType = "text/plain";
    }

    if ( (strcmp( mimeType, _native_format ) == 0) )
    {
        kdebug( KDEBUG_INFO, 30003, "strcmp( mimeType, _native_format ) == 0 !! Returning without conversion. " );
        // TODO: fetch remote file!
        assert( url.isLocalFile() );

        return;
    }

    QString constr = "Export == '";
    constr += mimeType;
    constr += "' and Import == '";
    constr += _native_format;
    constr += "'";

    QValueList<KoFilterEntry> vec = KoFilterEntry::query( constr );
    if ( vec.isEmpty() )
    {
        QString tmp;
        tmp.sprintf( i18n("Could not export file of type\n%s"), t->mimeType().ascii() );
        QMessageBox::critical( 0L, i18n("Missing export filter"), tmp, i18n("OK") );
        return;
    }

    KOffice::Filter::Data data;

    struct stat buff;
    stat( _tmpFile, &buff );
    unsigned int size = buff.st_size;

    FILE *f = fopen( _tmpFile, "rb" );
    if ( !f )
    {
        QString tmp;
        tmp.sprintf( i18n("Could not open file\n%s"), _tmpFile.ascii() );
        QMessageBox::critical( 0L, i18n("Error"), tmp, i18n("OK") );
        return;
    }

    char *p = new char[ size ];
    int got = fread( p, 1, size, f );
    fclose( f );

    data.setRawData( p, got );
    
    /*
    data.length( size );
    for( unsigned long int l = 0; l < size; l++ ) 
    {
        // HACK....
        if ( p[ l ] == 0 )
            p[ l ] = 1;
        data[l] = (CORBA::Octet)p[ l ];
    }

    delete []p;
    */
    
    KOffice::FilterFactory_var factory;
    factory = KOffice::FilterFactory::_narrow( vec[0].reference );
    assert( !CORBA::is_nil( factory ) );
    KOffice::Filter_var filter = factory->create();
    assert( !CORBA::is_nil( filter ) );

    filter->filter( data, _native_format, mimeType );

    filter->destroy();

    f = fopen( _url, "w" );
    if ( !f )
    {
        data.resetRawData( p, got );
        delete []p;

        QString tmp;
        tmp.sprintf( i18n("Could not write file\n%s"), _url.ascii() );
        QMessageBox::critical( 0L, i18n("Error"), tmp, i18n("OK") );
        return;
    }

    fwrite( data.data(), 1, data.size(), f );
    fclose( f );

    data.resetRawData( p, got );
    delete []p;
}

KoFilterManager::KoFilterManager()
{
}
