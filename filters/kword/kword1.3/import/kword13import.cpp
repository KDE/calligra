//

/* This file is part of the KDE project
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

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

#include <qxml.h>

#include <kdebug.h>
#include <kgenericfactory.h>
#include <kimageio.h>

#include <koFilterChain.h>
#include <koStoreDevice.h>

#include "kword13parser.h"
#include "kword13document.h"
#include "kword13import.h"

typedef KGenericFactory<KWord13Import, KoFilter> KWord13ImportFactory;
K_EXPORT_COMPONENT_FACTORY( libkwordkword1dot3import, KWord13ImportFactory( "kofficefilters" ) )


KWord13Import::KWord13Import(KoFilter */*parent*/, const char */*name*/, const QStringList &)
     : KoFilter()
{
}

bool KWord13Import::parseRoot( QIODevice* io, KWord13Document& kwordDocument )
{
    KWordParser handler( &kwordDocument );

    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );
    //### TODO: reader.setErrorHandler( &handler );

    QXmlInputSource source( io ); // Read the file
    
    if (!reader.parse( source ))
    {
        kdWarning(30520) << "Parse Error" << endl;
        return false;
    }
    return true;
}

KoFilter::ConversionStatus KWord13Import::convert( const QCString& from, const QCString& to )
{
    if ( to != "application/vnd.sun.xml.writer"  // ### TODO: OASIS
        || from != "application/x-kword" )
    {
        return KoFilter::NotImplemented;
    }

    // We need KimageIO's help in OOWriterWorker::convertUnknownImage
    KImageIO::registerFormats();

    KoFilter::ConversionStatus result = KoFilter::StupidError;
    
    KWord13Document kwordDocument;
    
    KoStoreDevice* subFile;

    // ### TODO: process documentinfo.xml

    subFile = m_chain->storageFile( "root", KoStore::Read );
    kdDebug (30520) << "Processing root... " << ((void*) subFile) << endl;
    if ( ! parseRoot ( subFile, kwordDocument ) )
    {
        kdWarning(30520) << "Opening root has failed. Trying raw XML file!" << endl;

        const QString filename( m_chain->inputFile() );
        if ( filename.isEmpty() )
        {
            kdError(30520) << "Could not open document as raw XML! Aborting!" << endl;
            return KoFilter::StupidError;
        }
        else
        {
            QFile file( filename );
            file.open( IO_ReadOnly );
            if ( ! parseRoot( &file, kwordDocument ) )
            {
                kdError(30520) << "Could not process document! Aborting!" << endl;
                file.close();
                return KoFilter::StupidError;
            }
            file.close();
        }
    }
    
    // ### TODO
    
    return result;
}

#include "kword13import.moc"
