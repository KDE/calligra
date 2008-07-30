/* This file is part of the KDE project
   Copyright (C) 2004-2007 Jaroslaw Staniek <js@iidea.pl>

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

class KexiComboBoxPopupPrivate;
class KexiTableView;
class KexiTableViewData;
class KexiTableViewColumn;
namespace KexiDB {
  class Field;
  class RecordData;
}
class QEvent;

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
    KexiComboBoxPopup(QWidget* parent, KexiTableViewColumn &column);

    /*! Alternative constructor supporting lookup fields and enum hints. */
    KexiComboBoxPopup(QWidget* parent, KexiDB::Field &field);

    virtual ~KexiComboBoxPopup();

    KexiTableView* tableView();

    /*! Sets maximum number of rows for this popup. */
    void setMaxRows(int r);

    /*! \return maximum number of rows for this popup. */
    int maxRows() const;

    /*! Default maximum number of rows for KexiComboBoxPopup objects. */
    static const int defaultMaxRows;

    virtual bool eventFilter( QObject *o, QEvent *e );

  signals:
    void rowAccepted(KexiDB::RecordData *record, int row);
    void cancelled();
    void hidden();

  public slots:
    virtual void resize( int w, int h );
    void updateSize(int minWidth = 0);

  protected slots:
    void slotTVItemAccepted(KexiDB::RecordData *record, int row, int col);
    void slotDataReloadRequested();

  protected:
    void init();
    //! The main function for setting data; data can be set either by passing \a column or \a field.
    //! The second case is used for lookup
    void setData(KexiTableViewColumn *column, KexiDB::Field *field);

    //! used by setData()
    void setDataInternal( KexiTableViewData *data, bool owner = true ); //!< helper

    KexiComboBoxPopupPrivate *d;

    friend class KexiComboBoxTableEdit;
};

#endif

