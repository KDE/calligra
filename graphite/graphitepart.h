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

#ifndef GRAPHITE_PART_H
#define GRAPHITE_PART_H

#include <kcommand.h>
#include <koDocument.h>
#include <graphiteglobal.h>

class QPainter;
class QMouseEvent;
class QKeyEvent;
class KAction;
class KoMainWindow;

class GraphiteView;
class GObjectM9r;
class GBackground;

class GraphitePart : public KoDocument {

    Q_OBJECT

public:
    GraphitePart(QWidget *parentWidget = 0, const char *widgetName = 0, QObject *parent=0, const char *name=0, bool singleViewMode=false);
    virtual ~GraphitePart();

    void setGlobalZoom(const double &zoom=1.0); // set the global zoom factor (for this view)
    virtual void paintContent(QPainter &painter, const QRect &rect, bool transparent=false);
    void paintSelection(QPainter &painter, const QRect &rect, GraphiteView *view);
    void paintPageBorders(QPainter &painter, const QRect &rect);

    virtual bool initDoc();

    virtual bool loadXML(QIODevice *, const QDomDocument &) { return false; }

    virtual void addShell(KoMainWindow *shell);

    // will return NPageSize if "Custom"!
    KPrinter::PageSize pageSize() const;
    // returns(!) the height/width
    void pageSize(double &width, double &height) const;
    void setPageSize(const KPrinter::PageSize &pageSize); // implies Norm
    void setPageSize(const double &width, const double &height); // implies Custom

    KPrinter::Orientation pageOrientation() const { return m_pageLayout.orientation; }
    void setPageOrientation(const KPrinter::Orientation &orientation);

    Graphite::PageBorders pageBorders() const { return m_pageLayout.borders; }
    // note: only commands should use addCommand==false
    void setPageBorders(const Graphite::PageBorders &pageBorders, bool addCommand=true);

    const Graphite::PageLayout &pageLayout() const { return m_pageLayout; }
    // note: only commands should use addCommand==false
    void setPageLayout(const Graphite::PageLayout &pageLayout, bool addCommand=true);
    // This method is used to shot the page layout dialog (e.g. on a dbl click on the ruler)
    // emits layoutChanged if the layout has been changed :)
    void showPageLayoutDia(QWidget *parent);

    // The unit is a document wide setting!
    const Graphite::Unit &unit() const { return m_unit; }

    // The canvas forwards the Events to us. We test if any
    // object has been hit (z-order!) and handle the event.
    // (Normally we create a Manipulator (M9r) class for the
    // object which has been hit. This class is the first
    // one which gets the Events before testing the whole tree.)
    void mouseMoveEvent(QMouseEvent *e, GraphiteView *view);
    void mousePressEvent(QMouseEvent *e, GraphiteView *view);
    void mouseReleaseEvent(QMouseEvent *e, GraphiteView *view);
    void mouseDoubleClickEvent(QMouseEvent *e, GraphiteView *view);

    void keyPressEvent(QKeyEvent *e, GraphiteView *view);
    void keyReleaseEvent(QKeyEvent *e, GraphiteView *view);

    // ###
    //virtual DCOPObject * dcopObject();

public slots:
    void setUnit(Graphite::Unit);

signals:
    void unitChanged(Graphite::Unit);
    void layoutChanged(const QValueList<FxRect> &diff);

protected:
    virtual KoView *createViewInstance(QWidget *parent, const char *name);

private slots:
    void edit_undo();
    void edit_redo();
    void edit_cut();

    void documentRestored();

private:
    GraphitePart &operator=(const GraphitePart &rhs);

    void updatePage(const QValueList<FxRect> &diff=QValueList<FxRect>());  // updates the page layout and emits layoutChanged

    KCommandHistory m_history;
    QMap<GraphiteView*, GObjectM9r*> m_m9rMap; // map views to m9rs
    // ### If an object is selected, store a pointer
    //     to its M9r here and draw the handles after
    //     drawing the whole tree.
    // ### Do we need isLoading() like in KSpread?
    Graphite::PageLayout m_pageLayout;
    Graphite::Unit m_unit;
    GBackground *m_nodeZero;
    double m_zoom;  // keep track of the zoom factor and setDirty appropriately
};


class GLayoutCmd : public KCommand {

public:
    GLayoutCmd(GraphitePart *doc, const QString &name);
    GLayoutCmd(GraphitePart *doc, const QString &name,
               const Graphite::PageLayout &oldLayout, const Graphite::PageLayout &newLayout);
    virtual ~GLayoutCmd() {}

    virtual void execute();
    virtual void unexecute();

    void setOldLayout(const Graphite::PageLayout &layout) { m_oldLayout=layout; }
    const Graphite::PageLayout &oldLayout() const { return m_oldLayout; }
    void setNewLayout(const Graphite::PageLayout &layout) { m_newLayout=layout; }
    const Graphite::PageLayout &newLayout() const { return m_newLayout; }

private:
    GraphitePart *m_doc;
    Graphite::PageLayout m_oldLayout, m_newLayout;
};


class GBordersCmd : public KCommand {

public:
    GBordersCmd(GraphitePart *doc, const QString &name);
    GBordersCmd(GraphitePart *doc, const QString &name,
                const Graphite::PageBorders &oldBorders, const Graphite::PageBorders &newBorders);
    virtual ~GBordersCmd() {}

    virtual void execute();
    virtual void unexecute();

    void setOldBorders(const Graphite::PageBorders &borders) { m_oldBorders=borders; }
    const Graphite::PageBorders &oldBorders() const { return m_oldBorders; }
    void setNewBorders(const Graphite::PageBorders &borders) { m_newBorders=borders; }
    const Graphite::PageBorders &newBorders() const { return m_newBorders; }

private:
    GraphitePart *m_doc;
    Graphite::PageBorders m_oldBorders, m_newBorders;
};

#endif
