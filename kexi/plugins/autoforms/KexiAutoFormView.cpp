/*
    Kexi Auto Form Plugin
    Copyright (C) 2011  Adam Pigg <adam@piggz.co.uk>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "KexiAutoFormView.h"
#include <QScrollArea>
#include <QLayout>
#include <KexiWindow.h>
#include "widgets/AutoForm.h"

#include <db/cursor.h>
#include <KexiMainWindowIface.h>

#ifndef KEXI_MOBILE
#include <widget/utils/kexirecordnavigator.h>
#endif

KexiAutoFormView::KexiAutoFormView(QWidget* parent): KexiView(parent), m_autoForm(0), m_pageSelector(0)
{
    kDebug();
    setObjectName("KexiAutoForm_DataView");
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setBackgroundRole(QPalette::Dark);
    m_scrollArea->viewport()->setAutoFillBackground(true);
    layout()->addWidget(m_scrollArea);
    
    #ifndef KEXI_MOBILE
    m_pageSelector = new KexiRecordNavigator(this, 0);
    layout()->addWidget(dynamic_cast<QWidget*>(m_pageSelector));
    m_pageSelector->setRecordCount(0);
    m_pageSelector->setInsertingButtonVisible(true);
    m_pageSelector->setLabelText(i18n("Record"));
    m_pageSelector->setRecordHandler(this);
    #endif
    
}

KexiAutoFormView::~KexiAutoFormView()
{

}

void KexiAutoFormView::resizeEvent(QResizeEvent* event)
{
    //Handle screen rotation
    QWidget::resizeEvent(event);
}

tristate KexiAutoFormView::afterSwitchFrom(Kexi::ViewMode mode)
{
    kDebug();
    kDebug() << tempData()->name;
    
    QDomElement e = tempData()->autoformDefinition;
    
    //if (tempData()->schemaChangedInPreviousView) {
        if (m_autoForm) {
            m_scrollArea->takeWidget();
            delete m_autoForm;
        }
        m_autoForm = new AutoForm(this, m_pageSelector);
        
        KexiDB::Connection *conn = KexiMainWindowIface::global()->project()->dbConnection();
        KexiDB::Cursor *cursor = conn->executeQuery(*(conn->tableSchema("actor")));
        
        if (cursor) {
            kDebug() << "Opened Cursor";
            KexiTableViewData *data = new KexiTableViewData(cursor);
            data->preloadAllRows();
            m_autoForm->setData(data);
            
        }
        m_scrollArea->setWidget(m_autoForm);
    //}
    return true;
}

void KexiAutoFormView::addNewRecordRequested()
{

}

void KexiAutoFormView::moveToFirstRecordRequested()
{
    m_autoForm->selectFirstRow();
}

void KexiAutoFormView::moveToNextRecordRequested()
{
    m_autoForm->selectNextRow();
}

void KexiAutoFormView::moveToPreviousRecordRequested()
{
    m_autoForm->selectPrevRow();
}

void KexiAutoFormView::moveToLastRecordRequested()
{
    m_autoForm->selectLastRow();
}

void KexiAutoFormView::moveToRecordRequested(uint r)
{

}

int KexiAutoFormView::currentRecord() const
{
    return m_autoForm->currentRow() + 1;
}

int KexiAutoFormView::recordCount() const
{
    return m_autoForm->rows();
}

tristate KexiAutoFormView::beforeSwitchTo(Kexi::ViewMode mode, bool &dontStore)
{
    Q_UNUSED(mode);
    Q_UNUSED(dontStore);
    
    return true;
}

KexiAutoFormPart::TempData* KexiAutoFormView::tempData() const
{
    return static_cast<KexiAutoFormPart::TempData*>(window()->data());
}
