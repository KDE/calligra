/***************************************************************************
 *   Copyright (C) 2006 by Adam Pigg                                       *
 *   adam@piggz.co.uk                                                      *
 *                                                                         *
 *   Based on insert calendar code:                                        *
 *   Copyright (C) 2005 by Raphael Langerhorst                             *
 *   raphael-langerhorst@gmx.at                                            *
 *                                                                         *
 *   Permission is hereby granted, free of charge, to any person obtaining *
 *   a copy of this software and associated documentation files (the       *
 *   "Software"), to deal in the Software without restriction, including   *
 *   without limitation the rights to use, copy, modify, merge, publish,   *
 *   distribute, sublicense, and/or sell copies of the Software, and to    *
 *   permit persons to whom the Software is furnished to do so, subject to *
 *   the following conditions:                                             *
 *                                                                         *
 *   The above copyright notice and this permission notice shall be        *
 *   included in all copies or substantial portions of the Software.       *
 *                                                                         *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       *
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    *
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR     *
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR *
 *   OTHER DEALINGS IN THE SOFTWARE.                                       *
 ***************************************************************************/

#ifndef KSPREAD_KEXIIMPORTDIALOG_H
#define KSPREAD_KEXIIMPORTDIALOG_H

#include "kspread_kexiimportdialogbase.h"
#include <koffice_export.h>

namespace KexiDB
{
class Connection;
class ConnectionData;
class DriverManager;
class Driver;
class TableOrQuerySchema;
}

namespace KSpread
{
class View;
}

class KListViewItem;

class KSpreadKexiImportDialog : public KSpreadKexiImportDialogBase
{
    Q_OBJECT

private:
    
    void populateTables();
    void populateSheets();
    KexiDB::Connection *conn;
    KexiDB::DriverManager *dm;

    KSpread::View *m_kspreadView;
public:

    /**
     * Constructor, initializes functionality.
     * The dates in the dialog initialize to a complete calendar for the
     * current month.
     */
    KSpreadKexiImportDialog(QWidget* parent = 0, const char* name = 0);

    /**
     * Virtual destructor.
     */
    virtual ~KSpreadKexiImportDialog();

    void openDatabase(QString,KexiDB::ConnectionData *cdata);
    KexiDB::Connection *connection()
    {
        return conn;
    };
    
    bool customQuery();
    QString customQueryString();
    int conversion(); //1: kspread 2: text ..... possibly add more option
    QPtrList<QListViewItem> selectedItems();
    
protected slots:

    /**
     * This slot is reimplemented from QDialog and
     * is connected to the insert button.
     * It also emits a insertCalendar signal.
     *
     * The dialog is closed (not deleted) when
     * this slot is called.
     *
     * @see insertCalendar, reject, QDialog::done
     */
    virtual void accept();

    /**
     * This slot is reimplemented from QDialog and
     * is connected to the cancel button.
     *
     * The dialog is closed (not deleted) when
     * this slot is called.
     *
     * @see accept, QDialog::done
     */
    virtual void reject();

signals:

    /**
     * This signal is emitted when the dialog is
     * accepted - that is, when the Insert button
     * is clicked.
     * This signal is meant to tell the
     * insert calendar plugin to actually insert
     * the calendar with the selected dates.
     */
    void insertKexi();
};


#endif
