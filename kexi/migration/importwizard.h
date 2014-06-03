/* This file is part of the KDE project
   Copyright (C) 2004 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2004-2006 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2005 Martin Ellis <martin.ellis@kdemail.net>

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

#ifndef KEXIMIGRATIONIMPORTWIZARD_H
#define KEXIMIGRATIONIMPORTWIZARD_H

#include <kassistantdialog.h>
#include <kapplication.h>

#include <db/tristate.h>
#include "migratemanager.h"

#include <QProgressBar>
#include <QLabel>

class QLabel;
class QCheckBox;
class QRadioButton;
class QHBoxLayout;
class QVBoxLayout;
class QGroupBox;
class KComboBox;
class KLineEdit;
class KPushButton;
class KexiConnectionSelectorWidget;
class KexiProjectSelectorWidget;
class KexiProjectSet;
class KexiDBTitlePage;
class KexiDBDriverComboBox;
class KexiPrjTypeSelector;
class KPageWidgetItem;

namespace Kexi
{
class ObjectStatus;
}

namespace KexiDB
{
class ConnectionData;
}

namespace KexiMigration
{

class KexiMigrate;

//! GUI for importing external databases (file-based and server-based)
class KEXIMIGR_EXPORT ImportWizard : public KAssistantDialog
{
    Q_OBJECT
public:
    /*! Creates wizard's instance.
     \a args contains arguments that can be parsed by parseArguments().
     \a *arg will be also set to imported project's filename on success
     and to null value on failure or cancellation. */
    ImportWizard(QWidget *parent = 0, QMap<QString, QString>* args = 0);
    virtual ~ImportWizard();

public slots:
    void progressUpdated(int percent);

protected slots:
    virtual void next();
    virtual void back();
    void slot_currentPageChanged(KPageWidgetItem*,KPageWidgetItem*);
    virtual void accept();
    virtual void reject();
    void helpClicked();
    void slotOptionsButtonClicked();

private:
    void parseArguments();
    
    void setupIntro();
    void setupSrcConn();
    void setupSrcDB();
    void setupDstType();
    void setupDstTitle();
    void setupDst();
    void setupFinish();
    void setupImportType();
    void setupImporting();
    bool checkUserInput();

    KexiMigrate* prepareImport(Kexi::ObjectStatus& result);

    /*! Performs import. \return true/false on success/faulure
     or cancelled when user cancelled importing (mainly
     because didn't allow overwriting an existing database by a new one). */
    tristate import();

    bool fileBasedSrcSelected() const;
    bool fileBasedDstSelected() const;
    QString driverNameForSelectedSource();

    void arriveSrcConnPage();
    void arriveSrcDBPage();
    void arriveDstTitlePage();
    void arriveDstPage();
    void arriveFinishPage();
    void arriveImportingPage();

    //! @return source filename selected by user or preselected one (if present)
    QString selectedSourceFileName() const;

    class Private;
    Private * const d;
};

}

#endif
