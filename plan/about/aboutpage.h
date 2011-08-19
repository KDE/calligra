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

namespace KPlato {
    class Project;
}

using namespace KPlato;

class KPlatoAboutPage
{
public:
    KPlatoAboutPage();
    ~KPlatoAboutPage();

    void generatePage( KHTMLPart &part, const KUrl &url );

    void setProject( Project *project ) { m_project = project; }

protected:
    QString loadFile( const QString& file );
    QString main();
    QString intro();
    QString tips();
    QString tutorial( const QString &header, const QString &text, const QString &nextpage, const QString &nexttext );
    QString tutorial1();
    QString tutorial2();

private:
    Project *m_project;
};

#endif
