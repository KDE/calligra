/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <david@mandrakesoft.com>

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

#ifndef KPRKWORD_H
#define KPRKWORD_H

#include <koFilter.h>
#include <koStore.h>
#include <qdom.h>

class KprKword : public KoFilter {

    Q_OBJECT

public:
    KprKword(KoFilter *parent, const char *name);

    virtual ~KprKword() {}

    virtual bool filter(const QString &fileIn, const QString &fileOut,
                        const QString &from, const QString &to,
                        const QString &config = QString::null);
protected:
    void convert();
    QDomDocument inpdoc;
    QDomDocument outdoc;
    QDomElement frameset;
    QString titleStyleName;
    QString aFont;
};
#endif // KPRKWORD_H
