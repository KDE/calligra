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


#ifndef AUTOFORM_H
#define AUTOFORM_H

#include <QWidget>
#include <tableview/kexidataawareobjectiface.h>
#include <kexidataprovider.h>
#include "AutoWidget.h"

class QGridLayout;

class AutoForm : public QWidget, 
                 public KexiDataAwareObjectInterface,
                 public KexiFormDataProvider
{
    Q_OBJECT
    KEXI_DATAAWAREOBJECTINTERFACE
    
public:
    AutoForm(QWidget* parent);
    virtual ~AutoForm();
    
protected:
    virtual void resizeEvent(QResizeEvent* );
    
//Reimplement functions from KexiDataAwareObjectInterface
    virtual void addHeaderColumn(const QString& caption, const QString& description, const QIcon& icon, int size);
    virtual void cellSelected(int col, int row);
    virtual void clearColumnsInternal(bool repaint);
    virtual void copySelection();
    virtual void createEditor(int row, int col, const QString& addText = QString(), bool removeOld = false);
    virtual void currentItemDeleteRequest();
    virtual int currentLocalSortColumn() const;
    virtual int currentLocalSortingOrder() const;
    virtual void cutSelection();
    virtual void dataRefreshed();
    virtual void dataSet(KexiTableViewData* data);
    virtual KexiDataItemInterface* editor(int col, bool ignoreMissingEditor = false);
    virtual void editorShowFocus(int row, int col);
    virtual void ensureCellVisible(int row, int col);
    virtual void itemChanged(KexiDB::RecordData* , int row, int col);
    virtual void itemChanged(KexiDB::RecordData* , int row, int col, QVariant oldValue);
    virtual void itemDeleteRequest(KexiDB::RecordData* , int row, int col);
    virtual void itemSelected(KexiDB::RecordData* );
    virtual void initDataContents();
    virtual int lastVisibleRow() const;
    virtual void newItemAppendedForAfterDeletingInSpreadSheetMode();
    virtual void paste();
    virtual void reloadActions();
    virtual void rowEditTerminated(int row);
    virtual int rowsPerPage() const;
    virtual void setLocalSortingOrder(int col, int order);
    virtual void sortedColumnChanged(int col);
    virtual void updateCell(int row, int col);
    virtual void updateCurrentCell();
    virtual void updateGUIAfterSorting();
    virtual void updateRow(int row);
    virtual void updateWidgetContents();
    virtual void updateWidgetContentsSize();
    virtual void updateWidgetScrollBars();
    virtual QScrollBar* verticalScrollBar() const;
    
private:
    QLabel *m_title;
    QGridLayout *m_layout;
    QList<AutoWidget*> m_widgets;
    
    void buildForm();
    void layoutForm();
};

#endif // AUTOFORM_H
