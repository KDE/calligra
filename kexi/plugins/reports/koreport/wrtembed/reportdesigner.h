/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
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
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
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
//#include <kexidb/connection.h>
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
    KoReportData *reportData(){return m_kordata;}

    ReportSection* section(KRSectionData::Section) const;
    void removeSection(KRSectionData::Section);
    void insertSection(KRSectionData::Section);

    ReportSectionDetail* detailSection() const {
        return m_detail;
    }
    void setDetail(ReportSectionDetail *rsd);
    void deleteDetail();

    void setReportTitle(const QString &);
    void setGridOptions(bool, int);
    QString reportTitle() const;

    QDomElement document() const;

    bool isModified() const;

    /**
    \return a list of field names in the selected query
    */
    QStringList fieldNames() const;
    /**
    \return a list of field keys in the selected query
    */
    QStringList fieldKeys() const;

    /**
    @brief Calculate the width of the page in pixels given the paper size, orientation, dpi and margin
    @return integer value of width in pixels
    */
    int pageWidthPx() const;

    QGraphicsScene* activeScene() const;
    void setActiveScene(QGraphicsScene* a);
    KoProperty::Set* propertySet() const {
        return m_set;
    }

    virtual QSize sizeHint() const;

    KoZoomHandler* zoomHandler() const;

    KoUnit pageUnit() const;

    void sectionContextMenuEvent(ReportScene *, QGraphicsSceneContextMenuEvent * e);
    void sectionMouseReleaseEvent(ReportSceneView *, QMouseEvent * e);

    void changeSet(KoProperty::Set *);
    KoProperty::Set* itemPropertySet() const {
        kDebug(); return m_itmset;
    }

    void setModified(bool = true);

    /**Return a unique name that can be used by the entity*/
    QString suggestEntityName(const QString &) const;

    /**Checks if the supplied name is unique among all entities*/
    bool isEntityNameUnique(const QString &, KRObjectData* = 0) const;

    static QList<QAction*> actions();

public slots:

    void slotEditDelete();
    void slotEditCut();
    void slotEditCopy();
    void slotEditPaste();
    void slotEditPaste(QGraphicsScene *);

    void slotItem(KRObjectData::EntityTypes);
    void slotItem(const QString&);

    void slotSectionEditor();

    void slotRaiseSelected();
    void slotLowerSelected();

protected:

    ReportSection *m_reportHeader;
    ReportSection *m_pageHeaderFirst;
    ReportSection *m_pageHeaderOdd;
    ReportSection *m_pageHeaderEven;
    ReportSection *m_pageHeaderLast;
    ReportSection *m_pageHeaderAny;

    ReportSection *m_pageFooterFirst;
    ReportSection *m_pageFooterOdd;
    ReportSection *m_pageFooterEven;
    ReportSection *m_pageFooterLast;
    ReportSection *m_pageFooterAny;
    ReportSection *m_reportFooter;
    ReportSectionDetail *m_detail;

private:
    class Private;
    Private * const d;

    void init();
    bool m_modified; // true if this document has been modified, false otherwise

    KoReportData *m_kordata;

    /**
    @brief Return a list of supported page formats
    @return A QStringList of page formats
    */
    QStringList pageFormats() const;

    virtual void resizeEvent(QResizeEvent * event);

    //Properties
    void createProperties();
    KoProperty::Set* m_set;
    KoProperty::Set* m_itmset;
    KoProperty::Property* m_title;
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

    ReportWriterSectionData *m_sectionData;
    unsigned int selectionCount() const;

private slots:
    void slotPropertyChanged(KoProperty::Set &s, KoProperty::Property &p);

    /**
    @brief When the 'page' button in the top left is pressed, change the property set to the reports properties.
    */
    void slotPageButton_Pressed();

signals:
    void pagePropertyChanged(KoProperty::Set &s);
    void propertySetChanged();
    void dirty();
    void reportDataChanged();
};

#endif

