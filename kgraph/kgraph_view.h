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

#ifndef KGRAPH_VIEW_H
#define KGRAPH_VIEW_H

#include <koView.h>
#include <kgcanvas.h>

class QResizeEvent;
class KGraphPart;
class KGObjectPool;


class KGraphView : public KoView {

    Q_OBJECT

public:
    KGraphView(KGraphPart *part, const KGObjectPool * const objectPool,
	       QWidget *parent=0, const char *name=0);
    virtual ~KGraphView();

    virtual QWidget *canvas() { return m_canvas; }

protected:
    //void paintEvent(QPaintEvent *ev);
    void resizeEvent(QResizeEvent *ev);

    virtual void updateReadWrite(bool readwrite);

private:
    KGCanvas *m_canvas;
};
#endif
