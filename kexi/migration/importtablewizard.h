/* This file is part of the KDE project
   Copyright (C) 2009 Adam Pigg <adam@piggz.co.uk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef IMPORTTABLEWIZARD_H
#define IMPORTTABLEWIZARD_H

#include <kassistantdialog.h>
#include <kexidb/kexidb_export.h>
#include <QPointer>

class KexiConnSelectorWidget;
class QListWidget;
class QLabel;
class QProgressBar;
class KexiProjectSelectorWidget;
class KexiProjectSet;

namespace KexiDB {
    class Connection;
}

namespace Kexi {
    class ObjectStatus;
}

namespace KexiMigration {

class MigrateManager;
class KexiMigrate;
class AlterSchemaWidget;

class KEXIMIGR_EXPORT ImportTableWizard : public KAssistantDialog {
Q_OBJECT
    public:
        
        ImportTableWizard ( KexiDB::Connection* curDB, QWidget* parent = 0, Qt::WFlags flags = 0 );
        ~ImportTableWizard ( );

        virtual void back();
        virtual void next();
        virtual void accept();
        virtual void reject();

    protected slots:
        void slot_currentPageChanged(KPageWidgetItem*,KPageWidgetItem*);
        
    private:

        KexiDB::Connection* m_currentDatabase;
        KexiConnSelectorWidget *m_srcConnSel;
        KexiMigration::MigrateManager *m_migrateManager;
        //KexiMigration::KexiMigrate *m_migrateDriver;
        QPointer<KexiMigration::KexiMigrate> m_migrateDriver;
        QListWidget *m_tableListWidget;
        AlterSchemaWidget *m_alterSchemaWidget;
        KexiProjectSelectorWidget *m_srcDBName;
        KexiProjectSet* m_prjSet;
        QString m_importTableName;
        
        bool fileBasedSrcSelected() const;
        QString driverNameForSelectedSource();
        KexiMigrate* prepareImport(Kexi::ObjectStatus& result);

        QLabel *m_lblImportingTxt, *m_lblImportingErrTxt, *m_finishLbl;
        QProgressBar *m_progressBar;
        KPushButton* m_importOptionsButton;
        bool m_importComplete;
        bool doImport();
        
        //Page Items
        KPageWidgetItem *m_introPageItem, *m_srcConnPageItem, *m_srcDBPageItem, *m_tablesPageItem, *m_alterTablePageItem, *m_importingPageItem, *m_finishPageItem;
        
        //Page Widgets
        QWidget *m_introPageWidget, *m_srcConnPageWidget, *m_srcDBPageWidget, *m_tablesPageWidget, *m_alterTablePageWidget, *m_importingPageWidget, *m_finishPageWidget;

        //Page Setup
        void setupIntroPage();
        void setupSrcConn();
        void setupSrcDB();
        void setupTableSelectPage();
        void setupAlterTablePage();
        void setupImportingPage();
        void setupFinishPage();
        
        //Page Arrival
        void arriveSrcConnPage();
        void arriveSrcDBPage();
        void arriveTableSelectPage();
        void arriveAlterTablePage();
        void arriveImportingPage();
        void arriveFinishPage();
};
}
#endif // IMPORTTABLEWIZARD_H
