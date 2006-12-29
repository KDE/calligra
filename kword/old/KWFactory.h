/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWORD_FACTORY_H
#define KWORD_FACTORY_H

#include <KoFactory.h>

class KAboutData;

class KWORD_EXPORT KWFactory : public KoFactory
{
    Q_OBJECT
public:
    KWFactory( QObject* parent = 0, const char* name = 0 );
    ~KWFactory();

    virtual KParts::Part* createPartObject( QWidget * = 0, QObject* parent = 0, const char* classname = "KoDocument", const QStringList &args = QStringList() );

    static KInstance* instance();

    // _Creates_ a KAboutData but doesn't keep ownership
    static KAboutData* aboutData();

private:
    static KInstance* s_instance;
    static KAboutData* s_aboutData;
};

#endif
