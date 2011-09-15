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

#ifndef WORDS_1_3_POST_PARSING
#define WORDS_1_3_POST_PARSING

class KoStore;
class Words13Document;

class Words13PostParsing
{
public:
    explicit Words13PostParsing(void);
    ~Words13PostParsing(void);
public:
    bool postParsePictures(KoStore* store);
    bool postParse(KoStore* store, Words13Document& doc);
protected:
    Words13Document* m_kwordDocument;
};

#endif // WORDS_1_3_POST_PARSING
