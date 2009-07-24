/*
* Kexi Report Plugin
* Copyright (C) 2007-2009 by Adam Pigg (adam@piggz.co.uk)
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
*/

#ifndef KEXIREPORTDESIGNVIEW_H
#define KEXIREPORTDESIGNVIEW_H

#include <core/KexiView.h>
#include <QDomDocument>
#include <koproperty/Set.h>
#include <kexireportpart.h>
#include "koreport/wrtembed/reportdesigner.h"
#include "koreportdata.h"

class QScrollArea;
class KexiSourceSelector;

/**
 @author
*/
class KexiReportDesignView : public KexiView
{
    Q_OBJECT
public:
    KexiReportDesignView(QWidget *parent, KexiSourceSelector*);

    ~KexiReportDesignView();
    virtual tristate afterSwitchFrom(Kexi::ViewMode mode);
    virtual tristate beforeSwitchTo(Kexi::ViewMode mode, bool &dontStore);

    void triggerAction(const QString &);
    
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

    KexiSourceSelector *srcsel;
protected:
    virtual KoProperty::Set *propertySet();
    virtual tristate storeData(bool dontAsk = false);
    virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);

private slots:
    void slotDesignerPropertySetChanged();

    public slots:
    void slotSetData(KoReportData*);


};

#endif
