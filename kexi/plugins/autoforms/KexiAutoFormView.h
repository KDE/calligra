/*
 *  Kexi Auto Form Plugin
 *  Copyright (C) 2011  Adam Pigg <adam@piggz.co.uk>
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 * 
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef KEXIAUTOFORMVIEW_H
#define KEXIAUTOFORMVIEW_H

#include <core/KexiView.h>
#include <core/KexiRecordNavigatorHandler.h>
#include <widget/dataviewcommon/kexidataawareobjectiface.h>
#include <widget/dataviewcommon/kexidataprovider.h>
#include "KexiAutoFormPart.h"

class AutoForm;
class QScrollArea;

class KexiAutoFormView : public KexiView, public KexiRecordNavigatorHandler
{
Q_OBJECT

public:
    KexiAutoFormView(QWidget* parent);
    virtual ~KexiAutoFormView();

    virtual tristate afterSwitchFrom(Kexi::ViewMode mode);
    virtual tristate beforeSwitchTo(Kexi::ViewMode mode, bool& dontStore);
    
    virtual void resizeEvent(QResizeEvent* );
    
    virtual void addNewRecordRequested();
    virtual void moveToFirstRecordRequested();
    virtual void moveToLastRecordRequested();
    virtual void moveToNextRecordRequested();
    virtual void moveToPreviousRecordRequested();
    virtual void moveToRecordRequested(uint r);
    virtual long int currentRecord();
    virtual long int recordCount();
    
private:
    QScrollArea *m_scrollArea;
    KexiRecordNavigatorIface *m_pageSelector;
    AutoForm *m_autoForm;
    
    KexiAutoFormPart::TempData* tempData() const;
};

#endif // KEXIAUTOFORMVIEW_H
