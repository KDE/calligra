/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <trobin@kde.org>

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

#include <kaction.h>
#include <klocale.h>

#include <ruler.h>
#include <graphitepart.h>
#include <graphitefactory.h>
#include <graphiteview.h>

GraphiteView::GraphiteView(GraphitePart *doc, QWidget *parent,
                           const char *name) : KoView(doc, parent, name),
                                               m_doc(doc), m_oldX(1), m_oldY(1) {
    setInstance(GraphiteFactory::global());
    setXMLFile(QString::fromLatin1("graphite.rc"));
    setupActions();
    m_canvas=new GCanvas(this, doc);
    m_canvas->setGeometry(20, 20, m_canvas->viewport()->width()-20,
                          m_canvas->viewport()->height()-20);
    // *never* remove from here -- will result in a crash (hen <--> egg)
    setupRulers();
    recalcRulers(0, 0);
}

GraphiteView::~GraphiteView() {
    delete m_canvas;
    m_canvas=0L;
}

void GraphiteView::slotViewZoom(const QString &t) {

    QString text=t;        // we need a copy we can manipulate
    QString corrected;
    QChar dot('.');
    QChar colon(',');
    double zoomValue=1.0;

    // analyze the new text and decide it we can use it
    unsigned int start=0;
    bool comma=false;
    // get rid of leading garbage
    while(!text[start].isDigit() && text[start]!=dot && text[start]!=colon && start<text.length())
        ++start;

    // QString::toDouble() doesn't like colons
    if(text[start]==dot)
        comma=true;
    if(text[start]==colon) {
        text[start]=dot;
        comma=true;
    }

    // now get the number and determine whether we use percent values or not
    unsigned int end=start+1;
    bool percent=false;
    while(end<text.length()) {
        if(text[end].isDigit())
            ++end;
        else if(!comma && text[end]==dot) {
            ++end;
            comma=true;
        }
        else if(!comma && text[end]==colon) {
            text[end]=dot;  // QString::toDouble...
            ++end;
            comma=true;
        }
        else if(text[end]==QChar('%')) {
            percent=true;
            break;
        }
        else
            break;
    }

    // get hold of the number and perform some sanity checks
    if(start<text.length()) {
        corrected=text.mid(start, end-start);
        zoomValue=corrected.toDouble();

        if(corrected.startsWith(QString::fromLatin1(".")))
            corrected.prepend(QString::fromLatin1("0"));

        if(percent) {
            corrected+=QChar('%');
            zoomValue/=100.0;
        }

        // further sanity checks (safe zoom range: 10% - 1000%)
        if(zoomValue<0.1) {
            zoomValue=0.1;
            if(percent)
                corrected=i18n("10%");
            else
                corrected=QString::fromLatin1("0.1");
        }
        else if(zoomValue>10.0) {
            zoomValue=10.0;
            if(percent)
                corrected=i18n("1000%");
            else
                corrected=QString::fromLatin1("10.0");
        }
    }

    if(corrected!=text) {
        QStringList items=m_zoomAction->items();
        // get rid of the "wrong" text
        items.remove(items.at(m_zoomAction->currentItem()));
        // and if we got a valid text append this one
        int index=items.findIndex(corrected);
        if(!corrected.isEmpty() && index==-1)
            items.append(corrected);
        m_zoomAction->setItems(items);
        // set the correct item as current item (can't do that earlier)
        if(!corrected.isEmpty())
            m_zoomAction->setCurrentItem(index==-1 ? items.count()-1 : index);
        else
            m_zoomAction->setCurrentItem(1); // 100%
    }

    // propagate the new zoom (*res) value
    double zr=zoomValue*GraphiteGlobal::self()->resolution();
    m_horiz->setZoomedRes(zr);
    m_vert->setZoomedRes(zr);
    setZoom(zoomValue);
}

void GraphiteView::recalcRulers(int x, int y) {

    if(x!=m_oldX)
        m_horiz->setOffset(x, y);
    if(y!=m_oldY)
        m_vert->setOffset(x, y);
    m_oldX=x;
    m_oldY=y;
}

void GraphiteView::rulerUnitChanged(GraphiteGlobal::Unit unit) {
    m_vert->setUnit(unit);
    m_horiz->setUnit(unit);
}

void GraphiteView::resizeEvent(QResizeEvent *e) {

    m_canvas->resize(e->size().width()-20, e->size().height()-20);
    m_horiz->setGeometry(20, 0, e->size().width(), 20);
    m_vert->setGeometry(0, 20, 20, e->size().height());
    recalcRulers(m_canvas->contentsX(), m_canvas->contentsY());
}

void GraphiteView::updateReadWrite(bool readwrite) {
    m_vert->setEditable(readwrite);
    m_vert->showMousePos(false);
    m_horiz->setEditable(readwrite);
    m_horiz->showMousePos(false);
}

void GraphiteView::setupActions() {

    m_zoomAction=new KSelectAction(i18n("&Zoom"), 0,
                                   actionCollection(), "view_zoom");
    connect(m_zoomAction, SIGNAL(activated(const QString &)), this, SLOT(slotViewZoom(const QString &)));
    // don't change that order!
    QStringList lst;
    lst << i18n("50%");
    lst << i18n("100%");
    lst << i18n("150%");
    lst << i18n("200%");
    lst << i18n("250%");
    lst << i18n("300%");
    m_zoomAction->setItems(lst);
    m_zoomAction->setCurrentItem(1);
    m_zoomAction->setEditable(true);
}

void GraphiteView::setupRulers() {

    m_vert=new Ruler(this, m_canvas->viewport(), Qt::Vertical,
                     m_doc->pageLayout(), GraphiteGlobal::self()->zoomedResolution());
    m_vert->showMousePos(true);
    connect(m_vert, SIGNAL(unitChanged(GraphiteGlobal::Unit)), this,
            SLOT(rulerUnitChanged(GraphiteGlobal::Unit)));

    m_horiz=new Ruler(this, m_canvas->viewport(), Qt::Horizontal,
                     m_doc->pageLayout(), GraphiteGlobal::self()->zoomedResolution());
    m_horiz->showMousePos(true);
    connect(m_horiz, SIGNAL(unitChanged(GraphiteGlobal::Unit)), this,
            SLOT(rulerUnitChanged(GraphiteGlobal::Unit)));

    m_canvas->setRulers(m_horiz, m_vert);
    connect(m_canvas, SIGNAL(contentsMoving(int, int)), this,
            SLOT(recalcRulers(int, int)));
}

#include <graphiteview.moc>
