/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@mandrakesoft.com>

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

//#include <qpainter.h>
//#include <qevent.h>

//#include <klocale.h>
//#include <kdebug.h>

#include <graphitepart.h>
#include <graphitefactory.h>
#include <graphiteview.h>


GraphiteView::GraphiteView(GraphitePart *doc, QWidget *parent,
			   const char *name) : KoView(doc, parent, name) {

    setInstance(GraphiteFactory::global());
    setXMLFile(QString::fromLatin1("graphite.rc"));

    m_canvas=new GCanvas(this, doc);
}

GraphiteView::~GraphiteView() {

    delete m_canvas;
    m_canvas=0L;
}

// Is done in GCanvas :)
/*void GraphiteView::paintEvent(QPaintEvent *ev) {

    kdDebug(37001) << "GraphiteView::painEvent()" << endl;

    QPainter painter;
    painter.begin(this);

    // ### TODO: Scaling

    // Let the document do the drawing
    koDocument()->paintEverything(painter, ev->rect(), false, this);

    painter.end();
}*/

void GraphiteView::resizeEvent(QResizeEvent *ev) {
    m_canvas->resize(ev->size());
}

void GraphiteView::updateReadWrite(bool /*readwrite*/) {
}
#include <graphiteview.moc>
