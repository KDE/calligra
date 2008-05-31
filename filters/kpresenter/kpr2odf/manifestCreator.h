/* This file is part of the KDE project
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.net>

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

//forward declarations
class QString;
class KoXmlWriter;
class QByteArray;
class QBuffer;

#ifndef MANIFESTCREATOR_H
#define MANIFESTCREATOR_H

/**
*   manifestCreator is a small class to automatize the creation of the manifest
*   in an ODF file. Just add their paths and their types with addFile and close it
*   to get the manifest, you are responsible to write it in your file.
*   @Note once manifestCreator is out of scope it'll delete its created manifest don't store it for later use
**/

class manifestCreator {
public:
    manifestCreator();
    ~manifestCreator();
    void addFile( const QString& path, const QString& type = "" );

    /**
    *   End the current manifest and return it to write it
    *   @Warning don't close it more than once, you'll get a null pointer, use getEndedManifest instead
    **/
    const QByteArray& endManifest();

    /**
    *   Get the manifest if for some reason you need another "copy"
    *   @Warning Don't call this if the manifest is not already ended, you'll get a null pointer
    *   @Note You just get another const pointer to its inner manifest
    **/
    const QByteArray& getEndedManifest();
private:
    bool m_manifestEnded;
    KoXmlWriter* m_manifest;
    QByteArray* m_manifestData;
    QBuffer* m_buffer;
};

#endif //MANIFESTCREATOR_H
