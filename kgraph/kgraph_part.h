/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@carinthia.com>

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

#ifndef KGRAPH_PART_H
#define KGRAPH_PART_H

#include <koDocument.h>

#include <kgobjectpool.h>
#include <kggrouppool.h>

class QPainter;


class KGraphPart : public KoDocument {

    Q_OBJECT

public:
    KGraphPart(QObject *parent=0, const char *name=0, bool singleViewMode=false);
    virtual ~KGraphPart();

    virtual KoView *createView(QWidget *parent=0, const char *name=0);
    virtual KoMainWindow *createShell();

    virtual void paintContent(QPainter &painter, const QRect &rect, bool transparent=false);

    virtual bool initDoc();

    virtual QCString mimeType() const;

    // (TODO) add a virtual CTOR which creates the objects for the
    // ObjectPool from XML (=loading)

protected slots:
    void edit_cut();

private:
    KGraphPart &operator=(const KGraphPart &rhs);

    KGObjectPool *objectPool;
    KGGroupPool *groupPool;
};
#endif
