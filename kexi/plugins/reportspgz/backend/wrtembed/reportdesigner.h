/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#ifndef REPORTDESIGNER_H
#define REPORTDESIGNER_H

#include <qwidget.h>
#include <qstring.h>

#include <qcolor.h>
#include <qmap.h>
#include <QVBoxLayout>
#include <QCloseEvent>

//#include "parsexmlutils.h"

#include <krreportdata.h>
#include <kexidb/connection.h>
#include <koproperty/set.h>
#include <koproperty/property.h>
#include <kdebug.h>
#include <krobjectdata.h>

class ReportGridOptions;
class QDomDocument;
class QGraphicsScene;
class KoRuler;
class KoZoomHandler;
class QGridLayout;
class ReportSectionDetail;
class ReportSection;
class KoUnit;
class ReportScene;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneContextMenuEvent;
class ReportSceneView;
class ReportWriterSectionData;
class KexiView;

//
// Class ReportDesigner
//     The ReportDesigner is the main widget for designing a report
//
class ReportDesigner : public QWidget
{
    Q_OBJECT
public:


    ReportDesigner(QWidget *, KexiDB::Connection *);
    ReportDesigner(QWidget *, KexiDB::Connection *, const QString&);
    ~ReportDesigner();

    ReportSection* getSection(KRSectionData::Section) const;
    void removeSection(KRSectionData::Section);
    void insertSection(KRSectionData::Section);

    ReportSectionDetail* detailSection() {
        return detail;
    }
    void setDetail(ReportSectionDetail *rsd);
    void deleteDetail();

    void setReportTitle(const QString &);
    void setReportDataSource(const QString &);
    void setGridOptions(bool, int);
    QString reportTitle();
    QString reportDataSource();

    QDomDocument document();

    bool isModified();

    void setConn(KexiDB::Connection *c) {
        conn = c;
    }
    KexiDB::Connection *theConn() {
        return conn;
    }
    bool isConnected() {
        return conn &&  conn->isConnected();
    }

    /**
    \return a list of queries that the report can be based on
    */
    QStringList queryList();

    /**
    \return a list of fields in the selected query
    */
    QStringList fieldList();

    /**
    \return the page width in pixels for the current paper size
    */
    int pageWidthPx() const;

    QGraphicsScene* activeScene();
    void setActiveScene(QGraphicsScene* a);
    KoProperty::Set* propertySet() {
        return set;
    }

    virtual QSize sizeHint() const;

    KoZoomHandler* zoomHandler();

    KoUnit pageUnit() const;

    void sectionContextMenuEvent(ReportScene *, QGraphicsSceneContextMenuEvent * e);
    void sectionMouseReleaseEvent(ReportSceneView *, QMouseEvent * e);

    void changeSet(KoProperty::Set *);
    KoProperty::Set* itemPropertySet() {
        kDebug(); return _itmset;
    }

    void setModified(bool = true);

    void showScriptEditor();

    /**Return a unique name that can be used by the entity*/
    QString suggestEntityName(const QString &) const;

    /**Checks if the supplied name is unique among all entities*/
    bool isEntityNameUnique(const QString &, KRObjectData* = 0) const;
public slots:

    void slotEditDelete();
    void slotEditCut();
    void slotEditCopy();
    void slotEditPaste();
    void slotEditPaste(QGraphicsScene *, const QPointF &);

/*
    void slotItemLabel();
    void slotItemField();
    void slotItemText();
    void slotItemLine();
    void slotItemBarcode();
    void slotItemImage();
    void slotItemChart();
    void slotItemShape();
    void slotItemCheck();*/
    void slotItem(KRObjectData::EntityTypes);

    void slotSectionEditor();

    void slotRaiseSelected();
    void slotLowerSelected();

protected:
    virtual void closeEvent(QCloseEvent * e);

    ReportSection * reportHead;
    ReportSection * pageHeadFirst;
    ReportSection * pageHeadOdd;
    ReportSection * pageHeadEven;
    ReportSection * pageHeadLast;
    ReportSection * pageHeadAny;

    ReportSection * pageFootFirst;
    ReportSection * pageFootOdd;
    ReportSection * pageFootEven;
    ReportSection * pageFootLast;
    ReportSection * pageFootAny;
    ReportSection * reportFoot;
    ReportSectionDetail * detail;

private:
    class Private;
    Private * const d;

    void init();
    bool _modified; // true if this document has been modified, false otherwise
    KexiDB::Connection *conn;
    QStringList pageFormats();

    virtual void resizeEvent(QResizeEvent * event);

    //Properties
    void createProperties();
    KoProperty::Set* set;
    KoProperty::Set* _itmset;
    KoProperty::Property* _title;
    KoProperty::Property* _dataSource;
    KoProperty::Property* _pageSize;
    KoProperty::Property* _orientation;
    KoProperty::Property* _unit;
    KoProperty::Property* _customHeight;
    KoProperty::Property* _customWidth;
    KoProperty::Property* _leftMargin;
    KoProperty::Property* _rightMargin;
    KoProperty::Property* _topMargin;
    KoProperty::Property* _bottomMargin;
    KoProperty::Property* _showGrid;
    KoProperty::Property* _gridDivisions;
    KoProperty::Property* _gridSnap;
    KoProperty::Property* _labelType;
    KoProperty::Property* _interpreter;

    QString editorText(const QString&);
    QString _script;

    ReportWriterSectionData * sectionData;
    unsigned int selectionCount();

private slots:
    void slotPropertyChanged(KoProperty::Set &s, KoProperty::Property &p);
    void slotPageButton_Pressed();

signals:
    void pagePropertyChanged(KoProperty::Set &s);
    void propertySetChanged();
    void dirty();
};

#endif

