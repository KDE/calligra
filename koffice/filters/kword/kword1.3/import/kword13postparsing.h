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

#ifndef KWORD_1_3_POST_PARSING
#define KWORD_1_3_POST_PARSING

class KoStore;
class KWord13Document;

class KWord13PostParsing
{
public:
    KWord13PostParsing(void);
    ~KWord13PostParsing(void);
public:
    bool postParsePictures( KoStore* store );
    bool postParse( KoStore* store, KWord13Document& doc );
protected:
    KWord13Document* m_kwordDocument;
};

#endif // KWORD_1_3_POST_PARSING
