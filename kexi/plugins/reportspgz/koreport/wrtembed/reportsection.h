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

#ifndef __REPORTSECTION_H__
#define __REPORTSECTION_H__

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <koproperty/Set.h>
#include <koproperty/Property.h>
#include <krsectiondata.h>
#include <QGraphicsScene>
#include "reportscene.h"

typedef QList<QGraphicsItem*> QGraphicsItemList;

// forward declarations
class QLabel;
class QDomNode;
class QDomDocument;
class QDomElement;

class ReportDesigner;
class ReportSceneView;
class QVBoxLayout;
class ReportResizeBar;
class KoRuler;
class ReportSectionTitle;

//
// Class ReportSection
//
//     This class is the base to all Report Section's visual representation.
// It contains the basic data and interface that all the sections need to work.
//
class ReportSection : public QWidget
{
    Q_OBJECT
public:
    ReportSection(ReportDesigner * rptdes, const char * name = 0);
    virtual ~ReportSection();

    void setTitle(const QString & s);
    void buildXML(QDomDocument & doc, QDomElement & section);
    void initFromXML(QDomNode & section);
    virtual QSize sizeHint() const;

    const QGraphicsItemList items() {
        return m_scene->items();
    };
protected slots:
    void slotResizeBarDragged(int delta);

private slots:
    void slotPageOptionsChanged(KoProperty::Set &);
    void slotSceneClicked();
    void slotPropertyChanged(KoProperty::Set &, KoProperty::Property &);
    void slotSceneLostFocus();

protected:
    ReportSectionTitle * m_title;
    ReportScene * m_scene;
    ReportResizeBar * m_resizeBar;
    ReportSceneView * m_sceneView;
    ReportDesigner* m_reportDesigner;
    KoRuler* m_sectionRuler;

private:
    KRSectionData *m_sectionData;

};

class ReportResizeBar : public QFrame
{
    Q_OBJECT
public:
    ReportResizeBar(QWidget * parent = 0, Qt::WFlags f = 0);

signals:
    void barDragged(int delta);
    void barPress();
    void barRelease();
    
protected:
    void mouseMoveEvent(QMouseEvent * e);
};

class ReportSectionTitle : public QLabel
{
    Q_OBJECT
public:
    ReportSectionTitle(QWidget *parent = 0);
    ~ReportSectionTitle();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent * event);

signals:
    void doubleClicked();
};

#endif

