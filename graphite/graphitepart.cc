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
#include <gbackground.h>

#include <pagelayoutdia_impl.h>


GraphitePart::GraphitePart(QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, bool singleViewMode)
    : KoDocument(parentWidget, widgetName, parent, name, singleViewMode), m_history(actionCollection()), m_zoom(1.0) {

    setInstance(GraphiteFactory::global());

    connect(&m_history, SIGNAL(documentRestored()), this, SLOT(documentRestored()));
    KStdAction::cut(this, SLOT(edit_cut()), actionCollection(), "edit_cut");

    // Settings -> Configure... (nice dialog to configure e.g. units)
    // ### KMessageBox::enableAllMessages -> dialog
    m_unit=Graphite::MM;  // ### load that from a rc file

    m_nodeZero=new GBackground(QString::fromLatin1("Background"));
    m_nodeZero->resize(m_pageLayout.fxRect());
    // test
    m_nodeZero->setBrush(Qt::red);
}

GraphitePart::~GraphitePart() {
    delete m_nodeZero;
    m_nodeZero=0;
}

void GraphitePart::setGlobalZoom(const double &zoom) {

    if(m_zoom==zoom)
        return;
    GraphiteGlobal::self()->setZoom(zoom);
    m_zoom=zoom;
    m_nodeZero->setDirty();
}

void GraphitePart::paintContent(QPainter &painter, const QRect &rect, bool transparent) {
    m_nodeZero->setTransparent(transparent);
    m_nodeZero->draw(painter, rect);
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
    if(m_pageLayout.size!=pageSize) {
        m_pageLayout.size=pageSize;
        m_pageLayout.layout=Graphite::PageLayout::Norm;
        updatePage();
    }
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
    updatePage();
}

void GraphitePart::setPageOrientation(const QPrinter::Orientation &orientation) {
    if(m_pageLayout.orientation!=orientation) {
        m_pageLayout.orientation=orientation;
        updatePage();
    }
}

void GraphitePart::setPageBorders(const Graphite::PageBorders &pageBorders, bool addCommand) {
    if(m_pageLayout.borders!=pageBorders) {
        if(addCommand)
            m_history.addCommand(new GBordersCmd(this, i18n("Changing Page Borders"),
                                                 m_pageLayout.borders, pageBorders), false);
        QValueList<FxRect> diff=Graphite::diff(m_pageLayout.borders, pageBorders, m_pageLayout.width(), m_pageLayout.height());
        m_pageLayout.borders=pageBorders;
        updatePage(diff);
    }
}

void GraphitePart::setPageLayout(const Graphite::PageLayout &pageLayout, bool addCommand) {
    if(m_pageLayout!=pageLayout) {
        if(addCommand)
            m_history.addCommand(new GLayoutCmd(this, i18n("Changing Page Layout"),
                                                m_pageLayout, pageLayout), false);
        QValueList<FxRect> diff=Graphite::diff(m_pageLayout, pageLayout);
        m_pageLayout=pageLayout;
        updatePage(diff);
    }
}

void GraphitePart::showPageLayoutDia(QWidget *parent) {
    Graphite::PageLayout layout(m_pageLayout);
    if(PageLayoutDiaImpl::pageLayoutDia(layout, this, parent))
        setPageLayout(layout);
}

void GraphitePart::mouseMoveEvent(QMouseEvent */*e*/, GraphiteView */*view*/) {
    // kdDebug(37001) << "MM x=" << e->x() << " y=" << e->y() << endl;
    // ### setGlobalZoom()
}

void GraphitePart::mousePressEvent(QMouseEvent *e, GraphiteView *view) {
    //kdDebug(37001) << "MP x=" << e->x() << " y=" << e->y() << endl;
    // test
    // ### Check the view - if it's the same as "before" -> ok :)
    // TEST -------------
    GObject *o=new GGroup(QString::fromLatin1("foo"));
    o->rotate(FxPoint(0, 0), 45.0*180.0*M_1_PI);
    GObjectM9r *m=o->createM9r(this, view);
    QRect r;
    m->mousePressEvent(e, r);
    delete m;
    delete o;
    // TEST -------------
    // ### setGlobalZoom()
}

void GraphitePart::mouseReleaseEvent(QMouseEvent */*e*/, GraphiteView */*view*/) {
    //kdDebug(37001) << "MR x=" << e->x() << " y=" << e->y() << endl;
    // ### setGlobalZoom()
}

void GraphitePart::mouseDoubleClickEvent(QMouseEvent */*e*/, GraphiteView */*view*/) {
    //kdDebug(37001) << "MDC x=" << e->x() << " y=" << e->y() << endl;
    // ### setGlobalZoom()
}

void GraphitePart::keyPressEvent(QKeyEvent */*e*/, GraphiteView */*view*/) {
    //kdDebug(37001) << "KP key=" << e->key() << endl;
    // ### setGlobalZoom()
}

void GraphitePart::keyReleaseEvent(QKeyEvent */*e*/, GraphiteView */*view*/) {
    //kdDebug(37001) << "KR key=" << e->key() << endl;
    // ### setGlobalZoom()
}

void GraphitePart::setUnit(Graphite::Unit unit) {
    if(m_unit==unit)
        return;
    m_unit=unit;
    emit unitChanged(unit);
}

KoView *GraphitePart::createViewInstance(QWidget *parent, const char *name) {
    return new GraphiteView(this, parent, name);
}

void GraphitePart::edit_undo() {
    m_history.undo();
}

void GraphitePart::edit_redo() {
    m_history.redo();
}

void GraphitePart::edit_cut() {
    kdDebug(37001) << "GraphitePart: edit_cut called" << endl;
}

void GraphitePart::documentRestored() {
    setModified(false);
}

void GraphitePart::updatePage(const QValueList<FxRect> &diff) {
    m_nodeZero->resize(m_pageLayout.fxRect());
    emit layoutChanged(diff);
}


GLayoutCmd::GLayoutCmd(GraphitePart *doc, const QString &name) : KCommand(name), m_doc(doc) {
}

GLayoutCmd::GLayoutCmd(GraphitePart *doc, const QString &name,
                       const Graphite::PageLayout &oldLayout, const Graphite::PageLayout &newLayout) :
    KCommand(name), m_doc(doc), m_oldLayout(oldLayout), m_newLayout(newLayout) {
}

void GLayoutCmd::execute() {
    if(m_doc)
        m_doc->setPageLayout(m_newLayout, false);
}

void GLayoutCmd::unexecute() {
    if(m_doc)
        m_doc->setPageLayout(m_oldLayout, false);
}


GBordersCmd::GBordersCmd(GraphitePart *doc, const QString &name) : KCommand(name), m_doc(doc) {
}

GBordersCmd::GBordersCmd(GraphitePart *doc, const QString &name,
            const Graphite::PageBorders &oldBorders, const Graphite::PageBorders &newBorders) :
    KCommand(name), m_doc(doc), m_oldBorders(oldBorders), m_newBorders(newBorders) {
}

void GBordersCmd::execute() {
    if(m_doc)
        m_doc->setPageBorders(m_newBorders, false);
}

void GBordersCmd::unexecute() {
    if(m_doc)
        m_doc->setPageBorders(m_oldBorders, false);
}

#include <graphitepart.moc>
