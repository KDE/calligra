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

//#include <kglobal.h>
//#include <kconfig.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kdebug.h>
#include <koMainWindow.h>

#include <graphitepart.h>
#include <graphiteview.h>
#include <graphitefactory.h>

#include <pagelayoutdia_impl.h>

// test
#include <ggroup.h>


GraphitePart::GraphitePart(QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, bool singleViewMode)
    : KoDocument(parentWidget, widgetName, parent, name, singleViewMode), m_history(actionCollection()) {

    setInstance(GraphiteFactory::global());

    connect(&m_history, SIGNAL(documentRestored()), this, SLOT(documentRestored()));
    KStdAction::cut(this, SLOT(edit_cut()), actionCollection(), "edit_cut");

    // Settings -> Configure... (nice dialog to configure e.g. units)
    m_unit=Graphite::MM;  // ### load that from a rc file
}

GraphitePart::~GraphitePart() {
}

bool GraphitePart::initDoc() {
    // ### If nothing is loaded, do initialize here
    // Show the "template" dia?
    m_pageLayout.loadDefaults();
    return true;
}

void GraphitePart::addShell(KoMainWindow *shell) {
    connect(shell, SIGNAL(documentSaved()), &m_history, SLOT(documentSaved()));
    KoDocument::addShell(shell);
}

QPrinter::PageSize GraphitePart::pageSize() const {
    return m_pageLayout.layout==Graphite::PageLayout::Norm ? m_pageLayout.size : QPrinter::NPageSize;
}

void GraphitePart::pageSize(double &width, double &height) const {

    if(m_pageLayout.layout==Graphite::PageLayout::Norm) {
        if(m_pageLayout.orientation==QPrinter::Portrait) {
            width=Graphite::pageWidth[m_pageLayout.size];
            height=Graphite::pageHeight[m_pageLayout.size];
        }
        else {
            height=Graphite::pageWidth[m_pageLayout.size];
            width=Graphite::pageHeight[m_pageLayout.size];
        }
    }
    else {
        if(m_pageLayout.orientation==QPrinter::Portrait) {
            width=m_pageLayout.customWidth;
            height=m_pageLayout.customHeight;
        }
        else {
            height=m_pageLayout.customWidth;
            width=m_pageLayout.customHeight;
        }
    }
}

void GraphitePart::setPageSize(const QPrinter::PageSize &pageSize) {
    m_pageLayout.size=pageSize;
    m_pageLayout.layout=Graphite::PageLayout::Norm;
    // TODO -- update
}

void GraphitePart::setPageSize(const double &width, const double &height) {

    if(m_pageLayout.orientation==QPrinter::Portrait) {
        m_pageLayout.customWidth=width;
        m_pageLayout.customHeight=height;
    }
    else {
        m_pageLayout.customWidth=height;
        m_pageLayout.customHeight=width;
    }
    m_pageLayout.layout=Graphite::PageLayout::Custom;
    // TODO -- update
}

void GraphitePart::setPageOrientation(const QPrinter::Orientation &orientation) {
    m_pageLayout.orientation=orientation;
    // TODO -- update
}

void GraphitePart::setPageBorders(const Graphite::PageBorders &pageBorders) {
    if(m_pageLayout.borders!=pageBorders) {
        m_pageLayout.borders=pageBorders;
        emit layoutChanged();
    }
}

void GraphitePart::showPageLayoutDia(QWidget *parent) {
    if(PageLayoutDiaImpl::pageLayoutDia(m_pageLayout, this, parent))
        emit layoutChanged();
}

void GraphitePart::mouseMoveEvent(QMouseEvent */*e*/, GraphiteView */*view*/) {
    // kdDebug(37001) << "MM x=" << e->x() << " y=" << e->y() << endl;
    // TODO: setGlobalZoom()
}

void GraphitePart::mousePressEvent(QMouseEvent *e, GraphiteView *view) {
    kdDebug(37001) << "MP x=" << e->x() << " y=" << e->y() << endl;
    // test
    // TODO: Check the view - if it's the same as "before" -> ok :)
    // TEST -------------
    GObject *o=new GGroup(QString::fromLatin1("foo"));
    o->rotate(FxPoint(0, 0), 45.0*180.0*M_1_PI);
    GObjectM9r *m=o->createM9r(this, view);
    QRect r;
    m->mousePressEvent(e, r);
    delete m;
    delete o;
    // TEST -------------
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

void GraphitePart::setUnit(Graphite::Unit unit) {
    if(m_unit==unit)
        return;
    m_unit=unit;
    emit unitChanged(unit);
}

void GraphitePart::edit_undo() {
    kdDebug(37001) << "GraphitePart: edit_undo called" << endl;
    m_history.undo();
}

void GraphitePart::edit_redo() {
    kdDebug(37001) << "GraphitePart: edit_redo called" << endl;
    m_history.redo();
}

void GraphitePart::edit_cut() {
    kdDebug(37001) << "GraphitePart: edit_cut called" << endl;
}

void GraphitePart::documentRestored() {
    setModified(false);
}

void GraphitePart::setGlobalZoom(const double &zoom) {

    if(GraphiteGlobal::self()->zoom()==zoom)
        return;
    GraphiteGlobal::self()->setZoom(zoom);
    // nodeZero->setDirty();
}

#include <graphitepart.moc>
