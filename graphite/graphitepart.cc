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

#include <qpainter.h>
#include <qevent.h>

#include <kaction.h>
#include <kstdaction.h>
#include <kdebug.h>
#include <kcommand.h>

#include <graphiteview.h>
#include <graphitefactory.h>
#include <graphitepart.h>
//#include <gobjectfactory.h>

// test
#include <gobject.h>
#include <ggroup.h>
#include <kdialogbase.h>
#include <math.h>

#include <bitset>


GraphitePart::GraphitePart(QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, bool singleViewMode)
    : KoDocument(parentWidget, widgetName, parent, name, singleViewMode) {

    setInstance(GraphiteFactory::global());
    m_history=new KCommandHistory(actionCollection());

    KStdAction::cut(this, SLOT(edit_cut()), actionCollection(), "edit_cut" );

    // Settings -> Configure... (nice dialog to configure e.g. units)
    // Tests ###################################
    FxValue v(100);
    kdDebug() << "value: " << v.value() << " (inch: " << v.valueInch() << " pt: "
              << v.valuePt() << ") -- px value: " << v.pxValue() << endl;
    FxValue w(v);
    if(w==v) kdDebug() << "equal" << endl;
    w.setValue(v.value()*10);
    kdDebug() << "value: " << w.value() << " (inch: " << w.valueInch() << " pt: "
              << w.valuePt() << ") -- px value: " << w.pxValue() << endl;
    w.setPxValue(1000);
    kdDebug() << "value: " << w.value() << " (inch: " << w.valueInch() << " pt: "
              << w.valuePt() << ") -- px value: " << w.pxValue() << endl;
    // #########################################
}

GraphitePart::~GraphitePart() {
}

bool GraphitePart::initDoc() {
    // If nothing is loaded, do initialize here (TODO)
    // Show the "template" dia
    return true;
}

void GraphitePart::mouseMoveEvent(QMouseEvent */*e*/, GraphiteView */*view*/) {
    // kdDebug(37001) << "MM x=" << e->x() << " y=" << e->y() << endl;
    // TODO: setGlobalZoom()
}

void GraphitePart::mousePressEvent(QMouseEvent *e, GraphiteView *view) {
    kdDebug(37001) << "MP x=" << e->x() << " y=" << e->y() << endl;
    // test
    // TODO: Check the view - if it's the same as "before" -> ok :)
    GObject *o=new GGroup(QString::fromLatin1("foo"));
    o->rotate(QPoint(10, 10), 45.0*180.0*M_1_PI);
    kdDebug(37001) << "atan(0): " << std::atan(0)
                   << "atan(pi/2)" << std::atan(M_PI_2) << endl;
    GObjectM9r *m=o->createM9r(this, view);
    QRect r;
    m->mousePressEvent(e, r);
    delete m;
    delete o;
    // TODO: setGlobalZoom()
}

void GraphitePart::mouseReleaseEvent(QMouseEvent *e, GraphiteView */*view*/) {
    kdDebug(37001) << "MR x=" << e->x() << " y=" << e->y() << endl;
    // TODO: setGlobalZoom()
}

void GraphitePart::mouseDoubleClickEvent(QMouseEvent *e, GraphiteView */*view*/) {
    kdDebug(37001) << "MDC x=" << e->x() << " y=" << e->y() << endl;
    // TODO: setGlobalZoom()
}

void GraphitePart::keyPressEvent(QKeyEvent *e, GraphiteView */*view*/) {
    kdDebug(37001) << "KP key=" << e->key() << endl;
    // TODO: setGlobalZoom()
}

void GraphitePart::keyReleaseEvent(QKeyEvent *e, GraphiteView */*view*/) {
    kdDebug(37001) << "KR key=" << e->key() << endl;
    // TODO: setGlobalZoom()
}

KoView *GraphitePart::createViewInstance(QWidget *parent, const char *name) {
    return new GraphiteView(this, parent, name);
}

void GraphitePart::paintContent(QPainter &/*painter*/, const QRect &/*rect*/, bool /*transparent*/) {
    kdDebug(37001) << "GraphitePart::painEvent()" << endl;
    // TODO: setGlobalZoom()
}

void GraphitePart::edit_undo() {
    kdDebug(37001) << "GraphitePart: edit_undo called" << endl;
    m_history->undo();
}

void GraphitePart::edit_redo() {
    kdDebug(37001) << "GraphitePart: edit_redo called" << endl;
    m_history->redo();
}

void GraphitePart::edit_cut() {
    kdDebug(37001) << "GraphitePart: edit_cut called" << endl;
}

void GraphitePart::setGlobalZoom(const double &zoom) {

    if(GraphiteGlobal::self()->zoom()==zoom)
        return;
    GraphiteGlobal::self()->setZoom(zoom);
    // nodeZero->recalculate();
}

#include <graphitepart.moc>
