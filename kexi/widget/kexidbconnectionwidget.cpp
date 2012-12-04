/* This file is part of the KDE project
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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

#include "kexidbconnectionwidget.h"

#include <kexi.h>
#include <kexiguimsghandler.h>
#include <db/connection.h>
#include <db/utils.h>
#include "kexidbdrivercombobox.h"

#include <KoIcon.h>

#include <kdebug.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kpassworddialog.h>
#include <kurlrequester.h>
#include <ktextedit.h>

#include <QLabel>
#include <QCheckBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

//! Templorary hides db list
//! @todo reenable this when implemented
#define NO_LOAD_DB_LIST

// @internal
class KexiDBConnectionWidget::Private
{
public:
    Private()
            : connectionOnly(false) {
    }

    KPushButton *btnSaveChanges, *btnTestConnection;
    bool connectionOnly;
    KexiProjectData data;
    KexiDBDriverComboBox *driversCombo;

};

class KexiDBConnectionDialog::Private
{
public:
    Private() { }

    KexiDBConnectionTabWidget *tabWidget;
};

//---------

KexiDBConnectionWidget::KexiDBConnectionWidget(QWidget* parent)
        : QWidget(parent)
        , d(new Private)
{
    setupUi(this);
    setObjectName("KexiConnectionSelectorWidget");
    iconLabel->setPixmap(DesktopIcon(KEXI_DATABASE_SERVER_ICON_NAME));

    QVBoxLayout *driversComboLyr = new QVBoxLayout(frmEngine);
    d->driversCombo = new KexiDBDriverComboBox(frmEngine, Kexi::driverManager().driversInfo(),
            KexiDBDriverComboBox::ShowServerDrivers);
    driversComboLyr->addWidget(d->driversCombo);
    frmEngine->setFocusProxy(d->driversCombo);
    lblEngine->setBuddy(d->driversCombo);
    QWidget::setTabOrder(lblEngine, d->driversCombo);

#ifdef NO_LOAD_DB_LIST
    btnLoadDBList->hide();
#endif
    btnLoadDBList->setIcon(koIcon("view-refresh"));
    btnLoadDBList->setToolTip(i18n("Load database list from the server"));
    btnLoadDBList->setWhatsThis(
        i18n("Loads database list from the server, so you can select one using the \"Name\" combo box."));

    QHBoxLayout *hbox = new QHBoxLayout(frmBottom);
    hbox->addStretch(2);
    d->btnSaveChanges = new KPushButton(
        KGuiItem(
            i18n("Save Changes"), "document-save",
            i18n("Save all changes made to this connection information"),
            i18n("Save all changes made to this connection information. "
                 "You can later reuse this information.")),
        frmBottom);
    d->btnSaveChanges->setObjectName("savechanges");
    hbox->addWidget(d->btnSaveChanges);
    hbox->addSpacing(KDialog::spacingHint());
    QWidget::setTabOrder(titleEdit, d->btnSaveChanges);
    d->btnSaveChanges->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    d->btnTestConnection = new KPushButton(
// @todo add Test Connection icon
        KGuiItem(i18n("&Test Connection"), QString(),
                 i18n("Test database connection"),
                 i18n("Tests database connection. "
                      "You can ensure that valid connection information is provided.")),
        frmBottom);
    d->btnTestConnection->setObjectName("testConnection");
    hbox->addWidget(d->btnTestConnection);
    setTabOrder(d->btnSaveChanges, d->btnTestConnection);
    d->btnTestConnection->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(locationBGrp, SIGNAL(clicked(int)), this, SLOT(slotLocationBGrpClicked(int)));
    connect(chkPortDefault, SIGNAL(toggled(bool)), this , SLOT(slotCBToggled(bool)));
    connect(btnLoadDBList, SIGNAL(clicked()), this, SIGNAL(loadDBList()));
    connect(d->btnSaveChanges, SIGNAL(clicked()), this, SIGNAL(saveChanges()));
}

KexiDBConnectionWidget::~KexiDBConnectionWidget()
{
    delete d;
}

bool KexiDBConnectionWidget::connectionOnly() const
{
    return d->connectionOnly;
}

void KexiDBConnectionWidget::setDataInternal(const KexiProjectData& data, bool connectionOnly,
        const QString& shortcutFileName)
{
    d->data = data;
    d->connectionOnly = connectionOnly;

    if (d->connectionOnly) {
        nameLabel->hide();
        nameCombo->hide();
        btnLoadDBList->hide();
        dbGroupBox->setTitle(i18n("Database Connection"));
    } else {
        nameLabel->show();
        nameCombo->show();
#ifndef NO_LOAD_DB_LIST
        btnLoadDBList->show();
#endif
        nameCombo->setEditText(d->data.databaseName());
        dbGroupBox->setTitle(i18n("Database"));
    }
//! @todo what if there's no such driver name?
    d->driversCombo->setDriverName(d->data.connectionData()->driverName);
    hostEdit->setText(d->data.connectionData()->hostName);
    locationBGrp->setButton(d->data.connectionData()->hostName.isEmpty() ? 0 : 1);
    slotLocationBGrpClicked(locationBGrp->selectedId());
    if (d->data.connectionData()->port != 0) {
        chkPortDefault->setChecked(false);
        customPortEdit->setValue(d->data.connectionData()->port);
    } else {
        chkPortDefault->setChecked(true);
        /* @todo default port # instead of 0 */
        customPortEdit->setValue(0);
    }
    userEdit->setText(d->data.connectionData()->userName);
    passwordEdit->setText(d->data.connectionData()->password);
    if (d->connectionOnly)
        titleEdit->setText(d->data.connectionData()->caption);
    else
        titleEdit->setText(d->data.caption());

    if (shortcutFileName.isEmpty()) {
        d->btnSaveChanges->hide();
    } else {
        if (!QFileInfo(shortcutFileName).isWritable()) {
            d->btnSaveChanges->setEnabled(false);
        }
    }
    chkSavePassword->setChecked(d->data.connectionData()->savePassword);
    adjustSize();
}

void KexiDBConnectionWidget::setData(const KexiProjectData& data, const QString& shortcutFileName)
{
    setDataInternal(data, false /*!connectionOnly*/, shortcutFileName);
}

void KexiDBConnectionWidget::setData(const KexiDB::ConnectionData& data, const QString& shortcutFileName)
{
    KexiProjectData pdata(data);
    setDataInternal(pdata, true /*connectionOnly*/, shortcutFileName);
}

KPushButton* KexiDBConnectionWidget::saveChangesButton() const
{
    return d->btnSaveChanges;
}

KPushButton* KexiDBConnectionWidget::testConnectionButton() const
{
    return d->btnTestConnection;
}

KexiDBDriverComboBox* KexiDBConnectionWidget::driversCombo() const
{
    return d->driversCombo;
}


KexiProjectData KexiDBConnectionWidget::data()
{
    return d->data;
}

void KexiDBConnectionWidget::slotLocationBGrpClicked(int id)
{
    if (id != 0 && id != 1) //only support local/remove radio buttons
        return;
    hostLbl->setEnabled(id == 1);
    hostEdit->setEnabled(id == 1);
}

void KexiDBConnectionWidget::slotCBToggled(bool on)
{
    if (sender() == chkPortDefault) {
        customPortEdit->setEnabled(!on);
    }
// else if (sender()==chkSocketDefault) {
//  customSocketEdit->setEnabled(!on);
// }
}

//-----------

KexiDBConnectionWidgetDetails::KexiDBConnectionWidgetDetails(QWidget* parent)
        : QWidget(parent)
{
    setupUi(this);
    customSocketEdit->setMode(KFile::File | KFile::ExistingOnly | KFile::LocalOnly);
}

KexiDBConnectionWidgetDetails::~KexiDBConnectionWidgetDetails()
{
}

//-----------

KexiDBConnectionTabWidget::KexiDBConnectionTabWidget(QWidget* parent)
        : KTabWidget(parent)
{
    mainWidget = new KexiDBConnectionWidget(this);
    mainWidget->setObjectName("mainWidget");
    mainWidget->layout()->setMargin(KDialog::marginHint());
    addTab(mainWidget, i18n("Parameters"));

    detailsWidget = new KexiDBConnectionWidgetDetails(this);
    detailsWidget->setObjectName("detailsWidget");
    addTab(detailsWidget, i18n("Details"));
    connect(detailsWidget->chkSocketDefault, SIGNAL(toggled(bool)),
            this, SLOT(slotSocketComboboxToggled(bool)));
    connect(detailsWidget->chkUseSocket, SIGNAL(toggled(bool)),
            this, SLOT(slotSocketComboboxToggled(bool)));

    connect(mainWidget->testConnectionButton(), SIGNAL(clicked()),
            this, SLOT(slotTestConnection()));
}

KexiDBConnectionTabWidget::~KexiDBConnectionTabWidget()
{
}

void KexiDBConnectionTabWidget::setData(const KexiProjectData& data, const QString& shortcutFileName)
{
    mainWidget->setData(data, shortcutFileName);
    detailsWidget->chkUseSocket->setChecked(data.constConnectionData()->useLocalSocketFile);
    detailsWidget->customSocketEdit->setUrl(data.constConnectionData()->localSocketFileName);
    detailsWidget->customSocketEdit->setEnabled(detailsWidget->chkUseSocket->isChecked());
    detailsWidget->chkSocketDefault->setChecked(data.constConnectionData()->localSocketFileName.isEmpty());
    detailsWidget->chkSocketDefault->setEnabled(detailsWidget->chkUseSocket->isChecked());
    detailsWidget->descriptionEdit->setText(data.description());
}

void KexiDBConnectionTabWidget::setData(const KexiDB::ConnectionData& data,
                                        const QString& shortcutFileName)
{
    mainWidget->setData(data, shortcutFileName);
    detailsWidget->chkUseSocket->setChecked(data.useLocalSocketFile);
    detailsWidget->customSocketEdit->setUrl(data.localSocketFileName);
    detailsWidget->customSocketEdit->setEnabled(detailsWidget->chkUseSocket->isChecked());
    detailsWidget->chkSocketDefault->setChecked(data.localSocketFileName.isEmpty());
    detailsWidget->chkSocketDefault->setEnabled(detailsWidget->chkUseSocket->isChecked());
    detailsWidget->descriptionEdit->setText(data.description);
}

KexiProjectData KexiDBConnectionTabWidget::currentProjectData()
{
    KexiProjectData data;

//! @todo check if that's database of connection shortcut. Now we're assuming db shortcut only!

    // collect data from the form's fields
    if (mainWidget->connectionOnly()) {
        data.connectionData()->caption = mainWidget->titleEdit->text();
        data.setCaption(QString());
        data.connectionData()->description = detailsWidget->descriptionEdit->toPlainText();
        data.setDatabaseName(QString());
    } else {
        data.connectionData()->caption.clear(); /* connection name is not specified... */
        data.setCaption(mainWidget->titleEdit->text());
        data.setDescription(detailsWidget->descriptionEdit->toPlainText());
        data.setDatabaseName(mainWidget->nameCombo->currentText());
    }
// }
    /* else {
        data.setCaption( QString() );
        data.connectionData()->connName = config.readEntry("caption");
        data.setDescription( QString() );
        data.connectionData()->description = config.readEntry("comment");
        data.setDatabaseName( QString() );
      }*/
    data.connectionData()->driverName = mainWidget->driversCombo()->selectedDriverName();

    /* if (data.connectionData()->driverName.isEmpty()) {
        //ERR: "No valid "engine" field specified for %1 section" group
        return false;
      }*/
    data.connectionData()->hostName =
        (mainWidget->remotehostRBtn->isChecked()/*remote*/)
        ? mainWidget->hostEdit->text() : QString();
    data.connectionData()->port = mainWidget->chkPortDefault->isChecked()
                                  ? 0 : mainWidget->customPortEdit->value();
    data.connectionData()->localSocketFileName = detailsWidget->chkSocketDefault->isChecked()
            ? QString() : detailsWidget->customSocketEdit->url().toLocalFile();
    data.connectionData()->useLocalSocketFile = detailsWidget->chkUseSocket->isChecked();
//UNSAFE!!!!
    data.connectionData()->userName = mainWidget->userEdit->text();
    data.connectionData()->password = mainWidget->passwordEdit->text();
    data.connectionData()->savePassword = mainWidget->chkSavePassword->isChecked();
    /*! @todo add "options=", eg. as string list? */
    return data;
}

bool KexiDBConnectionTabWidget::savePasswordOptionSelected() const
{
    return mainWidget->chkSavePassword->isChecked();
}

void KexiDBConnectionTabWidget::slotTestConnection()
{
    KexiGUIMessageHandler msgHandler;
    KexiDB::connectionTestDialog(this, *currentProjectData().connectionData(),
                                 msgHandler);
}

void KexiDBConnectionTabWidget::slotSocketComboboxToggled(bool on)
{
    if (sender() == detailsWidget->chkSocketDefault) {
        detailsWidget->customSocketEdit->setEnabled(!on);
    } else if (sender() == detailsWidget->chkUseSocket) {
        detailsWidget->customSocketEdit->setEnabled(
            on && !detailsWidget->chkSocketDefault->isChecked());
        detailsWidget->chkSocketDefault->setEnabled(on);
    }
}

//--------

//! @todo set proper help ctxt ID

KexiDBConnectionDialog::KexiDBConnectionDialog(QWidget* parent, const KexiProjectData& data,
        const QString& shortcutFileName, const KGuiItem& acceptButtonGuiItem)
        : KDialog(parent)
	, d(new Private)
{
    setWindowTitle(i18n("Open Database"));
    d->tabWidget = new KexiDBConnectionTabWidget(this);
    d->tabWidget->setData(data, shortcutFileName);
    init(acceptButtonGuiItem);
}

KexiDBConnectionDialog::KexiDBConnectionDialog(QWidget* parent,
        const KexiDB::ConnectionData& data,
        const QString& shortcutFileName, const KGuiItem& acceptButtonGuiItem)
        : KDialog(parent)
        , d(new Private)
{
    setWindowTitle(i18n("Connect to a Database Server"));
    d->tabWidget = new KexiDBConnectionTabWidget(this);
    d->tabWidget->setData(data, shortcutFileName);
    init(acceptButtonGuiItem);
}

KexiDBConnectionDialog::~KexiDBConnectionDialog()
{
    delete d;
}

void KexiDBConnectionDialog::init(const KGuiItem& acceptButtonGuiItem)
{
    setObjectName("KexiDBConnectionDialog");
    setButtons(KDialog::User1 | KDialog::Cancel | KDialog::Help);
    setButtonGuiItem(KDialog::User1,
                     acceptButtonGuiItem.text().isEmpty()
                     ? KGuiItem(i18n("&Open"), koIconName("document-open"), i18n("Open Database Connection"))
                     : acceptButtonGuiItem
                    );
    setModal(true);

    setMainWidget(d->tabWidget);
    connect(this, SIGNAL(user1Clicked()), this, SLOT(accept()));
    connect(d->tabWidget->mainWidget, SIGNAL(saveChanges()), this, SIGNAL(saveChanges()));
    connect(d->tabWidget, SIGNAL(testConnection()), this, SIGNAL(testConnection()));

    adjustSize();
    resize(width(), d->tabWidget->height());
    if (d->tabWidget->mainWidget->connectionOnly())
        d->tabWidget->mainWidget->driversCombo()->setFocus();
    else if (d->tabWidget->mainWidget->nameCombo->currentText().isEmpty())
        d->tabWidget->mainWidget->nameCombo->setFocus();
    else if (d->tabWidget->mainWidget->userEdit->text().isEmpty())
        d->tabWidget->mainWidget->userEdit->setFocus();
    else if (d->tabWidget->mainWidget->passwordEdit->text().isEmpty())
        d->tabWidget->mainWidget->passwordEdit->setFocus();
    else //back
        d->tabWidget->mainWidget->nameCombo->setFocus();
}

KexiProjectData KexiDBConnectionDialog::currentProjectData()
{
    return d->tabWidget->currentProjectData();
}

bool KexiDBConnectionDialog::savePasswordOptionSelected() const
{
    return d->tabWidget->savePasswordOptionSelected();
}

KexiDBConnectionWidget* KexiDBConnectionDialog::mainWidget() const
{
    return d->tabWidget->mainWidget;
}

KexiDBConnectionWidgetDetails* KexiDBConnectionDialog::detailsWidget() const
{
    return d->tabWidget->detailsWidget;
}

#include "kexidbconnectionwidget.moc"
