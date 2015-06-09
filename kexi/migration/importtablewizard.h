/* This file is part of the KDE project
   Copyright (C) 2009 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2014 Jarosław Staniek <staniek@kde.org>

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

#include <QPointer>

class QListWidget;
class QLabel;
class QProgressBar;
class QCheckBox;
class KDbConnection;
class KexiConnectionSelectorWidget;
class KexiProjectSelectorWidget;
class KexiProjectSet;

namespace Kexi {
    class ObjectStatus;
}

namespace KexiMigration {

class MigrateManager;
class KexiMigrate;
class AlterSchemaWidget;

class KEXIMIGR_EXPORT ImportTableWizard : public KAssistantDialog
{
Q_OBJECT
    public:
        explicit ImportTableWizard(KDbConnection* curDB, QWidget* parent = 0, QMap<QString, QString>* args = 0, Qt::WFlags flags = 0);
        virtual ~ImportTableWizard( );

        virtual void back();
        virtual void next();
        virtual void accept();
        virtual void reject();

    protected Q_SLOTS:
        void slot_currentPageChanged(KPageWidgetItem*,KPageWidgetItem*);

    private:

        KDbConnection* m_connection;
        KexiConnectionSelectorWidget *m_srcConnSel;
        KexiMigration::MigrateManager *m_migrateManager;
        QPointer<KexiMigration::KexiMigrate> m_migrateDriver;
        QListWidget *m_tableListWidget;
        AlterSchemaWidget *m_alterSchemaWidget;
        KexiProjectSelectorWidget *m_srcDBName;
        KexiProjectSet* m_prjSet;
        QString m_importTableName;
        QMap<QString, QString>* m_args;

        bool fileBasedSrcSelected() const;
        QString driverNameForSelectedSource();
        KexiMigrate* prepareImport(Kexi::ObjectStatus& result);

        QLabel *m_lblImportingTxt, *m_lblImportingErrTxt, *m_progressLbl, *m_rowsImportedLbl, *m_finishLbl;
        QProgressBar *m_importingProgressBar;
        QCheckBox *m_finishCheckBox;
        QPushButton* m_importOptionsButton;
        bool m_importComplete;
        bool m_importWasCanceled;
        unsigned m_rowCount;
        bool doImport();

        //Page Items
        KPageWidgetItem *m_introPageItem, *m_srcConnPageItem, *m_srcDBPageItem, *m_tablesPageItem,
                        *m_alterTablePageItem, *m_importingPageItem, *m_progressPageItem, *m_finishPageItem;

        //Page Widgets
        QWidget *m_introPageWidget, *m_srcConnPageWidget, *m_srcDBPageWidget, *m_tablesPageWidget,
                *m_alterTablePageWidget, *m_importingPageWidget, *m_progressPageWidget, *m_finishPageWidget;

        //Page Setup
        void setupIntroPage();
        void setupSrcConn();
        void setupSrcDB();
        void setupTableSelectPage();
        void setupAlterTablePage();
        void setupImportingPage();
        void setupProgressPage();
        void setupFinishPage();

        //Page Arrival
        void arriveSrcConnPage();
        void arriveSrcDBPage();
        void arriveTableSelectPage(KPageWidgetItem *prevPage);
        void arriveAlterTablePage();
        void arriveImportingPage();
        void arriveProgressPage();
        void arriveFinishPage();
    private Q_SLOTS:
        void slotConnPageItemSelected(bool isSelected);
        void slotTableListWidgetSelectionChanged();
        void slotNameChanged();
        void slotCancelClicked();
};
}
#endif // IMPORTTABLEWIZARD_H
