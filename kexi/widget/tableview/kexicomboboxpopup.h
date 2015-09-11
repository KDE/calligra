/* This file is part of the KDE project
   Copyright (C) 2004-2014 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXICOMBOBOXPOPUP_H
#define KEXICOMBOBOXPOPUP_H

#include <QFrame>

class KDbField;
class KDbRecordData;
class KDbTableViewColumn;
class KDbTableViewData;
class KexiComboBoxPopupPrivate;
class KexiTableScrollArea;

//! Internal class for displaying popup table view
class KexiComboBoxPopup : public QFrame
{
    Q_OBJECT
public:
//! @todo js: more ctors!
    /*! Constructor for creating a popup using definition from \a column.
     If the column is lookup column, it's definition is used to display
     one or more column within the popup. Otherwise column.field() is used
     to display single-column data. */
    KexiComboBoxPopup(QWidget* parent, KDbTableViewColumn &column);

    /*! Alternative constructor supporting lookup fields and enum hints. */
    KexiComboBoxPopup(QWidget* parent, KDbField &field);

    virtual ~KexiComboBoxPopup();

    KexiTableScrollArea* tableView();

    /*! Sets maximum number of records for this popup. */
    void setMaxRecordCount(int count);

    /*! \return maximum number of records for this popup. */
    int maxRecordCount() const;

    /*! Default maximum number of records for KexiComboBoxPopup objects. */
    static const int defaultMaxRecordCount;

Q_SIGNALS:
    void recordAccepted(KDbRecordData *data, int record);
    void cancelled();
    void hidden();

public Q_SLOTS:
    virtual void resize(int w, int h);
    void updateSize(int minWidth = 0);

protected Q_SLOTS:
    void slotTVItemAccepted(KDbRecordData *data, int record, int column);
    void slotDataReloadRequested();

protected:
    void init();

    virtual bool eventFilter(QObject *o, QEvent *e);

    //! The main function for setting data; data can be set either by passing \a column or \a field.
    //! The second case is used for lookup
    void setData(KDbTableViewColumn *column, KDbField *field);

    //! used by setData()
    void setDataInternal(KDbTableViewData *data, bool owner = true);   //!< helper

    KexiComboBoxPopupPrivate * const d;

    friend class KexiComboBoxTableEdit;
};

#endif

