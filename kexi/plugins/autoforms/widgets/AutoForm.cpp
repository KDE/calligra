/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Adam Pigg <piggz1@gmail.com>

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


#include "AutoForm.h"
#include <QGridLayout>
#include "AutoLineEdit.h"
#include <kexidb/cursor.h>
#include <kexidb/RecordData.h>

AutoForm::AutoForm(QWidget* parent, KexiRecordNavigator *nav): QWidget(parent), m_previousRecord(0)
{
    setBackgroundRole(QPalette::Window);
    m_layout = new QGridLayout(this);
    setLayout(m_layout);
    m_title = new QLabel("Title", this);
    m_layout->addWidget(m_title, 0, 0, 1, 1);
    
    m_navPanel = nav;
}

AutoForm::~AutoForm()
{

}

void AutoForm::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}

void AutoForm::addHeaderColumn(const QString& caption, const QString& description, const QIcon& icon, int size)
{

}

void AutoForm::cellSelected(int col, int row)
{

}

void AutoForm::clearColumnsInternal(bool repaint)
{

}

void AutoForm::copySelection()
{

}

void AutoForm::createEditor(int row, int col, const QString& addText, bool removeOld)
{

}

void AutoForm::currentItemDeleteRequest()
{

}

int AutoForm::currentLocalSortColumn() const
{
return 0;
}

int AutoForm::currentLocalSortingOrder() const
{
return 0;
}

void AutoForm::cutSelection()
{

}

void AutoForm::dataRefreshed()
{

}

void AutoForm::dataSet(KexiTableViewData* data)
{
}

KexiDataItemInterface* AutoForm::editor(int col, bool ignoreMissingEditor)
{
return 0;
}

void AutoForm::editorShowFocus(int row, int col)
{

}

void AutoForm::ensureCellVisible(int row, int col)
{

}

void AutoForm::itemChanged(KexiDB::RecordData* , int row, int col)
{

}

void AutoForm::itemChanged(KexiDB::RecordData* , int row, int col, QVariant oldValue)
{

}

void AutoForm::itemDeleteRequest(KexiDB::RecordData* , int row, int col)
{

}

void AutoForm::itemSelected(KexiDB::RecordData* )
{

}

void AutoForm::initDataContents()
{
    kDebug();

    m_title->setText(KexiDataAwareObjectInterface::data()->dbTableName());
    buildForm();
    layoutForm();
    
    KexiDataAwareObjectInterface::initDataContents();
    
    fillDataItems(*m_currentItem, cursorAtNewRow());
    
}

int AutoForm::lastVisibleRow() const
{
    return 0;
}

void AutoForm::newItemAppendedForAfterDeletingInSpreadSheetMode()
{

}

void AutoForm::paste()
{

}

void AutoForm::reloadActions()
{

}

void AutoForm::rowEditTerminated(int row)
{

}

int AutoForm::rowsPerPage() const
{
    return 0;
}

void AutoForm::setLocalSortingOrder(int col, int order)
{

}

void AutoForm::selectCellInternal()
{
    //m_currentItem is already set by KexiDataAwareObjectInterface::setCursorPosition()
    if (m_currentItem) {
        if (m_currentItem != m_previousRecord) {
            fillDataItems(*m_currentItem, cursorAtNewRow());
            m_previousRecord = m_currentItem;

//!@todo needed?            
#if 0
            QWidget *w = 0;
            if (m_curCol >= 0 && m_curCol < dbFormWidget()->orderedDataAwareWidgets()->count()) {
                w = dbFormWidget()->orderedDataAwareWidgets()->at(m_curCol);
            }
            if (w) {
                w->setFocus(); // re-focus, as we could have lost focus, e.g. when navigator button was clicked
                // select all
                KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(w);
                //! @todo add option for not selecting the field
                if (iface) {
                    iface->selectAllOnFocusIfNeeded();
                }
            }
#endif
        }
    } else {
        m_previousRecord = 0;
    }
}

void AutoForm::sortedColumnChanged(int col)
{

}

void AutoForm::updateCell(int row, int col)
{

}

void AutoForm::updateCurrentCell()
{

}

void AutoForm::updateGUIAfterSorting()
{

}

void AutoForm::updateRow(int row)
{

}

void AutoForm::updateWidgetContents()
{

}

void AutoForm::updateWidgetContentsSize()
{

}

void AutoForm::updateWidgetScrollBars()
{

}

QScrollBar* AutoForm::verticalScrollBar() const
{
    return 0;
}

void AutoForm::buildForm()
{
    KexiTableViewColumn::List col_list = KexiDataAwareObjectInterface::data()->columns();

    foreach(KexiTableViewColumn *col, col_list) {
        kDebug() << col->captionAliasOrName();
        AutoWidget* widget = new AutoLineEdit(this);
        widget->setDataSource(col->field()->name());
        widget->setColumnInfo(col->columnInfo());
        m_widgets << widget;
    }
    setMainDataSourceWidget(this);
    
    QSet<QString> invalidSources;
    invalidateDataSources(invalidSources, KexiDataAwareObjectInterface::data()->cursor()->query());
}

void AutoForm::layoutForm()
{
    int row = 1;
    foreach(AutoWidget *widget, m_widgets) {
        m_layout->addWidget(widget, row, 0, 1, 1);
        ++row;
    }
    resize(sizeHint());
}
