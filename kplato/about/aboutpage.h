/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <kplato@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KPLATO_ABOUTPAGE_H
#define KPLATO_ABOUTPAGE_H

class KHTMLPart;
class KUrl;
class QString;

class KPlatoAboutPage
{
public:
    KPlatoAboutPage();
    ~KPlatoAboutPage();

    static void generatePage( KHTMLPart &part, const KUrl &url );

protected:
    static QString loadFile( const QString& file );
    static QString intro();
    static QString tips();
    static QString tutorial( const QString &header, const QString &text, const QString &nextpage, const QString &nexttext );
    static QString tutorial1();
    static QString tutorial2();
};

#endif
