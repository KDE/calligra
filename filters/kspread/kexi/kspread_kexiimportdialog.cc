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

#include "kspread_kexiimportdialog.h"

//KDE Includes
#include <kdebug.h>
#include <kglobal.h>
#include <kpushbutton.h>
#include <klistview.h>
#include <kcombobox.h>
#include <qradiobutton.h>
#include <qtextedit.h>
#include <qcheckbox.h>
#include <klocale.h>

//Kexi Includes
#include <kexidb/global.h>
#include <kexidb/kexidb_export.h>
#include <kexidb/connectiondata.h>
#include <kexidb/connection.h>
#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>

//KSpread includes
#include <kspread_view.h>
#include <kspread_doc.h>
#include <kspread_map.h>

/**
 * Constructor
 * @param parent Pointer to kspread view
 * @param name Name of the dialog
 * @return None
 */
    KSpreadKexiImportDialog::KSpreadKexiImportDialog(QWidget* parent, const char* name)
    : KSpreadKexiImportDialogBase(parent,name)
{
    connect(this->m_insertButton,SIGNAL(clicked()),this,SLOT(accept()));
    connect(this->m_cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
}

/**
 * Destructor
 * @return None
 */
KSpreadKexiImportDialog::~KSpreadKexiImportDialog()
{}

/**
 * Accepts the dialog and start the import process
 */
void KSpreadKexiImportDialog::accept()
{
    kdDebug() << "insert kexi data dialog accepted (insert button clicked)" << endl;
    done(QDialog::Accepted);
    emit insertKexi();
}

/**
 * Cancels the dialog
 */
void KSpreadKexiImportDialog::reject()
{
    kdDebug() << "insert kexi data dialog rejected (cancel button clicked)" << endl;
    done(QDialog::Rejected);
}

/**
 * Opens a database given either a filename or connection data
 * @param fileName name of file to open if cdata is null
 * @param cdata connection data to server based database...not implemented though
 */
void KSpreadKexiImportDialog::openDatabase( QString fileName , KexiDB::ConnectionData *cdata)
{
    kdDebug() << "openDatabase" << endl;
    KexiDB::Driver *dr;
    KexiDB::DriverManager *dm;
    KexiDB::ConnectionData cd;
    
    //Open file db
    dm = new KexiDB::DriverManager();
    dr = dm->driver("sqlite3");
    if (!dr)
    {
        kdDebug() << "Unable to create driver" << endl;
        return;
    }
    
    if (cdata)
    {
        //Open server db
        cd = *cdata;
    }
    else
    {
        if (!fileName.isEmpty())
        {
            cd.setFileName(fileName); 
        }
        else
        {
            kdDebug() << "No file name" << endl;
            KMessageBox::error(NULL, i18n("No file specified"), i18n("Error"));
            return;
        }
    }

    conn = dr->createConnection(cd);

    if (!conn)
    {
        KMessageBox::error(NULL, i18n("Error creating connection"), i18n("Error"));
        return;
    }

    if(!conn->connect())
    {
        KMessageBox::error(NULL, i18n("Error connecting to database"), i18n("Error"));
        conn->debugError();
        return;
    }

    if (!conn->useDatabase( fileName ))
    {
        KMessageBox::error(NULL, i18n("Error using database"), i18n("Error"));
        conn->debugError();
        return;
    }

    populateTables();
    
}

/**
 * Gets all the tables and queries from the database
 * and adds them to m_sourceList
 */
void KSpreadKexiImportDialog::populateTables()
{
    QValueList<int> tids;
    QValueList<int> qids;

    kdDebug() << "Getting Tables and Queries" << endl;
    tids = conn->objectIds(KexiDB::TableObjectType);
    qids = conn->objectIds(KexiDB::QueryObjectType);

    kdDebug() << qids.count() << " queries " << tids.count() << " tables" << endl;

    QValueList<int>::iterator it;

    for ( it = tids.begin(); it != tids.end(); ++it )
    {
        (void) new KListViewItem(m_sourceList,"Table", conn->tableSchema(*it)->name());
        kdDebug() << "Table ID:" << (*it) << " " << conn->tableSchema(*it)->name()<< endl;
    }

    for ( it = qids.begin(); it != qids.end(); ++it )
    {
        (void) new KListViewItem(m_sourceList,"Query", conn->querySchema(*it)->name());
        kdDebug() << "Query ID:" << (*it) << " " << conn->querySchema(*it)->name() << endl;
    }

    //Select the first item
    if(m_sourceList->firstChild())
    {
        m_sourceList->setSelected(m_sourceList->firstChild(), true);
    }

}

/**
 * 
 * @return the custom query that has been entered
 */
QString KSpreadKexiImportDialog::customQueryString()
{
    return m_CustomQueryText->text();
}

QPtrList<QListViewItem> KSpreadKexiImportDialog::selectedItems()
{
    QPtrList<QListViewItem> lst;
    QListViewItemIterator it( m_sourceList );
    while ( it.current() ) 
    {
        if ( it.current()->isSelected() )
            lst.append( it.current() );
        ++it;
    }
    return lst;
}

bool KSpreadKexiImportDialog::customQuery()
{
    return m_customQueryCheck->isChecked();
}
/**
 * Returns the user specified option for data conversion
 * @return integer stating the type of cenverion to make
 */
int KSpreadKexiImportDialog::conversion()
{
    if ( m_convKSRadio->isChecked())
        return 1;
    else if ( m_convTextRadio->isChecked())
        return 2;
    else
        return -1;
}