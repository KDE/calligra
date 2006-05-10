/*
   This file is part of the KDE project
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWORD_1_3_PICTURE
#define KWORD_1_3_PICTURE

class KTempFile;
class KoStore;

#include <QString>

class KWord13Picture
{
public:
    KWord13Picture( void );
    ~KWord13Picture( void );
public:
    bool loadPicture( KoStore* store );
    QString getOasisPictureName( void ) const;
public:
    QString m_storeName; ///< Name of the file in store
    KTempFile* m_tempFile; ///< Temporary file
    bool m_valid; ///< Is the picture valid?
};

#endif // KWORD_1_3_PICTURE
