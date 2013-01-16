/* This file is part of the KDE project
    Copyright (C) 2011  Adam Pigg <piggz1@gmail.com>
    Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

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
#include <dataviewcommon/kexidataawareobjectiface.h>
#include <dataviewcommon/kexidataprovider.h>
#include "AutoWidget.h"

class QGridLayout;

namespace KexiDB {
        class RecordDate;
}

class AutoForm : public QWidget, 
                 public KexiDataAwareObjectInterface,
                 public KexiFormDataProvider
{
    Q_OBJECT
    KEXI_DATAAWAREOBJECTINTERFACE
    
public:
    AutoForm(QWidget* parent, KexiRecordNavigatorIface *nav);
    virtual ~AutoForm();

    int itemIndex(AutoWidget*);
    
protected:
    virtual void resizeEvent(QResizeEvent* );
    
//Reimplement functions from KexiDataAwareObjectInterface
    virtual void addHeaderColumn(const QString& caption, const QString& description, const QIcon& icon, int size);
    virtual void clearColumnsInternal(bool repaint);
    virtual void copySelection();
    virtual void createEditor(int row, int col, const QString& addText = QString(), bool removeOld = false);
    virtual int currentLocalSortColumn() const;
    virtual int currentLocalSortingOrder() const;
    virtual void cutSelection();
    
    virtual KexiDataItemInterface* editor(int col, bool ignoreMissingEditor = false);
    virtual void editorShowFocus(int row, int col);
    virtual void ensureCellVisible(int row, int col);
    virtual void initDataContents();
    virtual int lastVisibleRow() const;
    virtual void paste();
    virtual int rowsPerPage() const;
    virtual void setLocalSortingOrder(int col, int order);
    virtual void selectCellInternal();
    virtual void updateCell(int row, int col);
    virtual void updateCurrentCell();
    virtual void updateGUIAfterSorting();
    virtual void updateRow(int row);
    virtual void updateWidgetContents();
    virtual void updateWidgetContentsSize();
    virtual void updateWidgetScrollBars();
    virtual QScrollBar* verticalScrollBar() const;
    
    //Reimplement from KexiFormDataProvider
    virtual void valueChanged(KexiDataItemInterface* item);

    
signals:
    void dataSet(KexiTableViewData *data);

    void itemSelected(KexiDB::RecordData *);
    void cellSelected(int col, int row);

    void itemReturnPressed(KexiDB::RecordData *, int row, int col);
    void itemDblClicked(KexiDB::RecordData *, int row, int col);
    void itemMouseReleased(KexiDB::RecordData *, int row, int col);

    void dragOverRow(KexiDB::RecordData *record, int row, QDragMoveEvent* e);
    void droppedAtRow(KexiDB::RecordData *record, int row, QDropEvent *e, KexiDB::RecordData*& newRecord);

    /*! Data has been refreshed on-screen - emitted from initDataContents(). */
    void dataRefreshed();

    void itemChanged(KexiDB::RecordData *, int row, int col);
    void itemChanged(KexiDB::RecordData *, int row, int col, QVariant oldValue);
    void itemDeleteRequest(KexiDB::RecordData *, int row, int col);
    void currentItemDeleteRequest();
    //! Emitted for spreadsheet mode when an item was deleted and a new item has been appended
    void newItemAppendedForAfterDeletingInSpreadSheetMode();
// void addRecordRequest();
// void contextMenuRequested(KexiDB::RecordData *,  int row, int col, const QPoint &);
    void sortedColumnChanged(int col);

    //! emitted when row editing is started (for updating or inserting)
    void rowEditStarted(int row);

    //! emitted when row editing is terminated (for updating or inserting)
    //! no matter if accepted or not
    void rowEditTerminated(int row);

    //! emitted when state of 'save/cancel record changes' actions should be updated.
    void updateSaveCancelActions();
    
    //! Emitted in initActions() to force reload actions
    //! You should remove existing actions and add them again.
    void reloadActions();

private:
    QLabel *m_title;
    QGridLayout *m_layout;
    QHash<int, AutoWidget*> m_widgets;
    
    KexiDB::RecordData *m_previousRecord;
        
    void buildForm();
    void layoutForm();
};

#endif // AUTOFORM_H
