/* This file is part of the KDE project
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.net>
   code based on svgexport.cc from Inge Wallin <inge@lysator.liu.se>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA  02110-1301  USA.
*/

//Qt's includes
#include <QString>
#include <QByteArray>
#include <QBuffer>

//KOffice includes
#include <KoXmlWriter.h>
#include <kdebug.h>

#include "manifestCreator.h"

manifestCreator::manifestCreator()
: m_manifestEnded( false )
{
    //We write to a QByteArray so that we can return it when needed
    m_manifestData = new QByteArray();
    m_buffer = new QBuffer( m_manifestData );
    m_buffer->open( QIODevice::WriteOnly );

    m_manifest = new KoXmlWriter( m_buffer );
    m_manifest->startElement( "manifest:manifest" );
    //FIXME:in OD Essentials the manifest file has a DOCTYPE, nevertheless
    //if I open a ODP the DOCTYPE is not there, which behavior is right?
//     m_manifest->addAttribute( "xmlns:office", "http://openoffice.org/2000/office" );
}

manifestCreator::~manifestCreator()
{
    delete m_manifest;
    delete m_manifestData;
    delete m_buffer;
}

void manifestCreator::addFile( const QString& path, const QString& type )
{
    m_manifest->startElement( "manifest:file-entry" );
    m_manifest->addAttribute( "manifest:media-type", type );
    m_manifest->addAttribute( "manifest:full-path", path );
    m_manifest->endElement();
}

const QByteArray& manifestCreator::endManifest()
{
    if( m_manifestEnded ) {
        kDebug()<<"Error, manifest ended more than once";
        return 0;
    }
    m_manifestEnded = true;
    m_manifest->endElement();//end of manifest:manifest
    m_manifest->endDocument();
    return *m_manifestData;
}

const QByteArray& manifestCreator::getEndedManifest()
{
    if( !m_manifestEnded ) {
        kDebug()<<"Error: manifest not ended and requested getEndedManifest";
        return 0;
    }
    return *m_manifestData;
}
