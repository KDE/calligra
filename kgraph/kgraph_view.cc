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

#include <qpainter.h>
#include <qevent.h>

#include <klocale.h>
#include <kdebug.h>

#include <kgraph_global.h>
#include <kgraph_part.h>
#include <kgraph_view.h>


KGraphView::KGraphView(KGraphPart *part, const KGObjectPool * const objectPool,
		       QWidget *parent, const char *name) : KoView(part, parent, name) {

    setInstance(KGraphFactory::global());
    setXMLFile("kgraph.rc");

    m_canvas=new KGCanvas(this, objectPool, "canvas");
}

KGraphView::~KGraphView() {

    delete m_canvas;
    m_canvas=0L;
}

/*void KGraphView::paintEvent(QPaintEvent *ev) {

    kdDebug(37001) << "KGraphView::painEvent()" << endl;

    QPainter painter;
    painter.begin(this);

    // ### TODO: Scaling

    // Let the document do the drawing
    koDocument()->paintEverything(painter, ev->rect(), false, this);

    painter.end();
}*/

void KGraphView::resizeEvent(QResizeEvent *ev) {
    m_canvas->resize(ev->size());
}

void KGraphView::updateReadWrite(bool /*readwrite*/) {
}
#include <kgraph_view.moc>
