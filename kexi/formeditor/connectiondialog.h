/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2009 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef FORMCONNECTIONDIALOG_H
#define FORMCONNECTIONDIALOG_H

#include <QLabel>
#include <kdialog.h>

#include <kexi_export.h>

namespace KexiDB
{
class ResultInfo;
class RecordData;
}

class QLabel;
class KexiTableView;
class KexiTableViewData;

namespace KFormDesigner
{

class Form;
class ConnectionBuffer;
class Connection;


/*! This dialog is used to edit the connections of a form. It uses KexiTableView for this. There is also a details widget (icon + text)) that shows correctness
  of current connection.  */
class KFORMEDITOR_EXPORT ConnectionDialog : public KDialog
{
    Q_OBJECT

public:
    ConnectionDialog(Form *form, QWidget *parent);
    ~ConnectionDialog();

    /*! Displays as modal dialog, to edit connections in Form::connectionBuffer(). */
    void exec();

protected:
    /*! Used when connection is ok. Displays a message in details widget and changes icon in 'OK?' column. */
    void setStatusOk(KexiDB::RecordData *record = 0);
    /*! Used when connection is wrong. Displays a message in details widget and changes icon in 'OK?' column. \a msg is
      the message explaining what's wrong. */
    void setStatusError(const QString &msg, KexiDB::RecordData *record = 0);
    //! Inits table data, columns, etc.
    void initTable();
    /*! Updates the widget list (shown in receiver and sender columns). Then fill in the table with the connections in m_buffer. */
    void updateTableData();
    /*! Updates the slot list, according to the receiver name, and only shows those who are compatible with signal args. */
    void updateSlotList(KexiDB::RecordData *record);
    //! Updates the signal list, according to the sender name.
    void updateSignalList(KexiDB::RecordData *record);

protected slots:
    /*! Slot called when the user modifies a cell. Signal and/or slot cells are cleared if necessary (not valid anymore). */
    void slotCellChanged(KexiDB::RecordData *, int, QVariant&, KexiDB::ResultInfo*);
    /*! This function checks if the connection represented by \a record is valid. It checks if all args (sender, receiver, signal and slot)
     are given, and then if signal/slot args are compatible (should be always true, as we don't show non-compatible slots). It calls \ref setStatusOk()
     or \ref setStatusError() following the result of checks. */
    void checkConnection(KexiDB::RecordData *record);

    /*! Hides the dialog and allow the user to create the Connection by drag-and-drop in the Form itself. Currently disabled in the GUI.
     \sa FormManager::startCreatingConnection()  */
    void newItemByDragnDrop();
    /*! Creates a new item. It just moves the cursor to the last empty row. */
    void newItem();
    void removeItem();

    /*! This slot is called when the user ends connection creation (when in drag-and-drop mode). The dialog is restored,
      and the created connection is added to the list. */
    void slotConnectionCreated(KFormDesigner::Form *form, KFormDesigner::Connection &connection);
    /*! This slot is called when the user aborts connection creation (when in drag-and-drop mode). The dialog is restored,
      and an empty connection is created. */
    void slotConnectionAborted(KFormDesigner::Form *form);

    void slotCellSelected(int col, int row);
    void slotRowInserted(KexiDB::RecordData*, bool);

    /*! Slot called when the user presses 'Ok' button. The Form::connectionBuffer() is deleted, created again and filled with Connection.
     If the user presses 'Cancel', nothing happens. */
    virtual void slotOk();

private:
    class Private;

    Private* const d;
};

}

#endif
