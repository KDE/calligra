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

#include <krreportdata.h>
#include <kexidb/connection.h>
#include <koproperty/Set.h>
#include <koproperty/Property.h>
#include <kdebug.h>
#include <krobjectdata.h>
#include "koreport_export.h"
#include "koreportdata.h"

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
class KOREPORT_EXPORT ReportDesigner : public QWidget
{
    Q_OBJECT
public:
    
    ReportDesigner(QWidget *);
    ReportDesigner(QWidget *, QDomElement);

    ~ReportDesigner();
    
    void setReportData(KoReportData* kodata);
    
    ReportSection* getSection(KRSectionData::Section) const;
    void removeSection(KRSectionData::Section);
    void insertSection(KRSectionData::Section);

    ReportSectionDetail* detailSection() {
        return detail;
    }
    void setDetail(ReportSectionDetail *rsd);
    void deleteDetail();

    void setReportTitle(const QString &);
//    void setReportDataSource(const QString &);
    void setGridOptions(bool, int);
    QString reportTitle();
//    QString reportDataSource();

    QDomElement document();

    bool isModified();

    KexiDB::Connection *theConn() {
        return m_conn;
    }
    bool isConnected() {
        return m_conn &&  m_conn->isConnected();
    }

    /**
    \return a list of fields in the selected query
    */
    QStringList fieldList();

     /**
    \return a list of object scripts in the database
    */
    QStringList scriptList();
    
    /**
    \return the page width in pixels for the current paper size
    */
    int pageWidthPx() const;

    QGraphicsScene* activeScene();
    void setActiveScene(QGraphicsScene* a);
    KoProperty::Set* propertySet() {
        return m_set;
    }

    virtual QSize sizeHint() const;

    KoZoomHandler* zoomHandler();

    KoUnit pageUnit() const;

    void sectionContextMenuEvent(ReportScene *, QGraphicsSceneContextMenuEvent * e);
    void sectionMouseReleaseEvent(ReportSceneView *, QMouseEvent * e);

    void changeSet(KoProperty::Set *);
    KoProperty::Set* itemPropertySet() {
        kDebug(); return m_itmset;
    }

    void setModified(bool = true);

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
    bool m_modified; // true if this document has been modified, false otherwise

    KexiDB::Connection *m_conn;
    KoReportData *m_kordata;
    
    QStringList pageFormats();

    virtual void resizeEvent(QResizeEvent * event);

    //Properties
    void createProperties();
    KoProperty::Set* m_set;
    KoProperty::Set* m_itmset;
    KoProperty::Property* m_title;
//    KoProperty::Property* m_dataSource;
//    KoProperty::Property* m_externalData;
    KoProperty::Property* m_pageSize;
    KoProperty::Property* m_orientation;
    KoProperty::Property* m_unit;
    KoProperty::Property* m_customHeight;
    KoProperty::Property* m_customWidth;
    KoProperty::Property* m_leftMargin;
    KoProperty::Property* m_rightMargin;
    KoProperty::Property* m_topMargin;
    KoProperty::Property* m_bottomMargin;
    KoProperty::Property* m_showGrid;
    KoProperty::Property* m_gridDivisions;
    KoProperty::Property* m_gridSnap;
    KoProperty::Property* m_labelType;
    KoProperty::Property* m_interpreter;
    KoProperty::Property* m_script;

    ReportWriterSectionData * m_sectionData;
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

