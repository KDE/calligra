/*
* Kexi Report Plugin
* Copyright (C) 2007-2009 by Adam Pigg <adam@piggz.co.uk>
* Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
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

#ifndef KEXIREPORTDESIGNVIEW_H
#define KEXIREPORTDESIGNVIEW_H

#include <core/KexiView.h>
#include <kexireportpart.h>

#include <KPropertySet>

#include <KoReportDesigner>
#include <KoReportData>

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
    virtual tristate beforeSwitchTo(Kexi::ViewMode mode, bool *dontStore);

    void triggerAction(const QString &);

Q_SIGNALS:
    void itemInserted(const QString& entity);

private:
    KoReportDesigner *m_reportDesigner;
    KPropertySet *m_propertySet;
    KexiReportPart::TempData* tempData() const;
    QScrollArea * m_scrollArea;

    //Actions
    QAction *m_editCutAction;
    QAction *m_editCopyAction;
    QAction *m_editPasteAction;
    QAction *m_editDeleteAction;
    QAction *m_editSectionAction;
    QAction *m_parameterEdit;
    QAction *m_itemRaiseAction;
    QAction *m_itemLowerAction;

    KexiSourceSelector *m_sourceSelector;

protected:
    virtual KPropertySet *propertySet();
    virtual tristate storeData(bool dontAsk = false);
    virtual KDbObject* storeNewData(const KDbObject& object,
                                             KexiView::StoreNewDataOptions options,
                                             bool *cancel);

private Q_SLOTS:
    void slotDesignerPropertySetChanged();

public Q_SLOTS:
    void slotSetData(KoReportData*);
};

#endif
