/*
 * Kexi Report Plugin
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
#ifndef KEXIREPORTDESIGNVIEW_H
#define KEXIREPORTDESIGNVIEW_H

#include <core/KexiView.h>
#include <QDomDocument>
#include <koproperty/set.h>
#include <kexireportpart.h>
#include "backend/wrtembed/reportdesigner.h"

class QScrollArea;
class ReportEntitySelector;

/**
 @author
*/
class KexiReportDesignView : public KexiView
{
    Q_OBJECT
public:
    KexiReportDesignView(QWidget *parent, ReportEntitySelector*);

    ~KexiReportDesignView();
    virtual tristate afterSwitchFrom(Kexi::ViewMode mode);
    virtual tristate beforeSwitchTo(Kexi::ViewMode mode, bool &dontStore);
private:
    ReportDesigner *_rd;
    KoProperty::Set *_set;
    KexiReportPart::TempData* tempData() const;
    QScrollArea * scr;

    //Actions
    KAction *editCutAction;
    KAction *editCopyAction;
    KAction *editPasteAction;
    KAction *editDeleteAction;
    KAction *sectionEdit;
    KAction *parameterEdit;
    KAction *itemRaiseAction;
    KAction *itemLowerAction;

    ReportEntitySelector* res;
protected:
    virtual KoProperty::Set *propertySet();
    virtual tristate storeData(bool dontAsk = false);
    virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);

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



};

#endif
