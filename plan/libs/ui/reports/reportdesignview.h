/*
* KPlato Report Plugin
* Copyright (C) 2007-2009 by Adam Pigg (adam@piggz.co.uk)
* Copyright (C) 2009 by Dag Andersen <danders@get2net.dk>
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
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef PLAN_REPORTDESIGNVIEW_H
#define PLAN_REPORTDESIGNVIEW_H

#include <core/View.h>
#include <QDomDocument>
#include <koproperty/Set.h>
#include <kexireportpart.h>
#include "koreport/wrtembed/reportdesigner.h"
#include "koreportdata.h"

class QScrollArea;
class ReportEntitySelector;
class SourceSelector;

/**
 @author
*/

namespace KPlato
{

class ReportDesignView : public View
{
    Q_OBJECT
public:
    ReportDesignView(QWidget *parent, ReportEntitySelector*, SourceSelector*);

    ~ReportDesignView();
    virtual tristate afterSwitchFrom(::ViewMode mode);
    virtual tristate beforeSwitchTo(::ViewMode mode, bool &dontStore);
private:
    ReportDesigner *_rd;
    KoProperty::Set *_set;
    ReportPart::TempData* tempData() const;
    QScrollArea * scr;

    //Actions
    QAction *editCutAction;
    QAction *editCopyAction;
    QAction *editPasteAction;
    QAction *editDeleteAction;
    QAction *sectionEdit;
    QAction *parameterEdit;
    QAction *itemRaiseAction;
    QAction *itemLowerAction;

    ReportEntitySelector* res;
    SourceSelector *srcsel;
protected:
    virtual KoProperty::Set *propertySet();
    virtual tristate storeData(bool dontAsk = false);
    virtual DB::SchemaData* storeNewData(const DB::SchemaData& sdata, bool &cancel);

private slots:
    void slotDesignerPropertySetChanged();
    void slotLabel(){_rd->slotItem(KRObjectData::EntityLabel);};
    void slotField(){_rd->slotItem(KRObjectData::EntityField);};
    void slotText(){_rd->slotItem(KRObjectData::EntityText);};
    void slotChart(){_rd->slotItem(KRObjectData::EntityChart);};
    void slotBarcode(){_rd->slotItem(KRObjectData::EntityBarcode);};
    void slotImage(){_rd->slotItem(KRObjectData::EntityImage);};
    void slotLine(){_rd->slotItem(KRObjectData::EntityLine);};
    void slotCheck(){_rd->slotItem(KRObjectData::EntityCheck);};

    public slots:
    void slotSetData(KoReportData*);


};

} //namespace KPlato

#endif
