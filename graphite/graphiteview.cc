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

#include <kaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <koRuler.h>
#include <koPageLayoutDia.h>

#include <graphitepart.h>
#include <graphitefactory.h>
#include <graphiteview.h>
#include <graphiteshell.h>


GraphiteView::GraphiteView(GraphitePart *doc, QWidget *parent,
			   const char *name) : KoView(doc, parent, name),
					       m_oldX(1), m_oldY(1) {
    setInstance(GraphiteFactory::global());
    setXMLFile(QString::fromLatin1("graphite.rc"));

    (void) new KAction(i18n("&New View"), 0, this, SLOT(slotViewNew()),
		       actionCollection(), "view_newview");
    KSelectAction *zoom=new KSelectAction(i18n("&Zoom"), 0,
					  actionCollection(), "view_zoom");
    connect(zoom, SIGNAL(activated(int)), this, SLOT(slotViewZoom(int)));
    QStringList lst;
    lst << i18n("50%");
    lst << i18n("100%");
    lst << i18n("150%");
    lst << i18n("200%");
    lst << i18n("250%");
    lst << i18n("300%");
    lst << i18n("Other...");
    zoom->setItems(lst);

    m_canvas=new GCanvas(this, doc);
    m_canvas->setGeometry(20, 20, m_canvas->viewport()->width()-20,
			  m_canvas->viewport()->height()-20);

    KoPageLayout layout=KoPageLayoutDia::standardLayout();

    m_vert=new KoRuler(this, m_canvas->viewport(), KoRuler::VERTICAL, layout, 0);
    m_vert->showMousePos(true);

    m_horiz=new KoRuler(this, m_canvas->viewport(), KoRuler::HORIZONTAL, layout, 0);
    m_horiz->showMousePos(true);

    m_canvas->setRulers(m_vert, m_horiz);
    connect(m_canvas, SIGNAL(contentsMoving(int, int)), this,
	    SLOT(recalcRulers(int, int)));
    recalcRulers(0, 0);
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

void GraphiteView::slotViewNew() {

    if(koDocument()==0L)
	kdFatal(37001) << "Huh! No doc?" << endl;

    KoMainWindow *shell = new GraphiteShell;
    shell->show();
    shell->setRootDocument(koDocument());
}

void GraphiteView::slotViewZoom(int item) {
    kdDebug(37001) << "GraphiteView::slotViewZoom(): item=" << item << endl;
}

void GraphiteView::recalcRulers(int x, int y) {

    // TODO: respect the zoom factor
    if(x!=m_oldX)
	m_horiz->setOffset(x, y);
    if(y!=m_oldY)
	m_vert->setOffset(x, y);
    m_oldX=x;
    m_oldY=y;
}

void GraphiteView::resizeEvent(QResizeEvent *e) {

    m_canvas->resize(e->size().width()-20, e->size().height()-20);
    m_horiz->setGeometry(20, 0, m_canvas->viewport()->width()+20, 20);
    m_vert->setGeometry(0, 20, 20, m_canvas->viewport()->height()+20);
    recalcRulers(m_canvas->contentsX(), m_canvas->contentsY());
}

void GraphiteView::updateReadWrite(bool /*readwrite*/) {
}
#include <graphiteview.moc>
