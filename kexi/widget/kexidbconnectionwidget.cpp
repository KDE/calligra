/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#include "kexidbconnectionwidget.h"
#include "kexidbconnectionwidgetdetailsbase.h"

#include <kexi.h>
#include "kexidbdrivercombobox.h"

#include <kdebug.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kpassdlg.h>
#include <kurlrequester.h>
#include <ktextedit.h>

#include <qlabel.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qwidgetstack.h>
#include <qlayout.h>
#include <qvbox.h>

class KexiDBConnectionWidget::Private
{
	public:
		KexiDBConnectionWidget::Private()
		{
		}

		KPushButton *btnSaveChanges;
};

//---------

KexiDBConnectionWidget::KexiDBConnectionWidget( QWidget* parent,  const char* name )
 : KexiDBConnectionWidgetBase( parent, name )
 , d(new Private())
{
	iconLabel->setPixmap(DesktopIcon("socket"));

	QVBoxLayout *driversComboLyr = new QVBoxLayout(frmEngine);
	driversCombo = new KexiDBDriverComboBox(Kexi::driverManager().driversInfo(), false,
		frmEngine, "drivers combo");
	lblEngine->setBuddy( driversCombo );
	lblEngine->setFocusProxy( driversCombo );
	driversComboLyr->addWidget( driversCombo );

	QHBoxLayout *hbox = new QHBoxLayout(frmBottom);
	hbox->addStretch(2);
	d->btnSaveChanges = new KPushButton(KGuiItem(i18n("Save changes"), "filesave", 
		i18n("Save all changes made to this connection")), frmBottom, "savechanges");
	hbox->addWidget( d->btnSaveChanges );
	d->btnSaveChanges->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

	connect(locationBGrp, SIGNAL(clicked(int)), this, SLOT(slotLocationBGrpClicked(int)));
	connect( chkPortDefault, SIGNAL(toggled(bool)), this , SLOT(slotCBToggled(bool)) );
	connect( d->btnSaveChanges, SIGNAL(clicked()), this, SIGNAL(saveChanges()) );
}

KexiDBConnectionWidget::~KexiDBConnectionWidget()
{
	delete d;
}

void KexiDBConnectionWidget::setData(const KexiProjectData& data, const QString& shortcutFileName)
{
	m_data = data;
	nameEdit->setText(m_data.databaseName());
	hostEdit->setText(m_data.connectionData()->hostName);
	locationBGrp->setButton( m_data.connectionData()->hostName.isEmpty() ? 0 : 1 );
	slotLocationBGrpClicked( locationBGrp->selectedId() );
	if (m_data.connectionData()->port!=0) {
		chkPortDefault->setChecked(false);
		customPortEdit->setValue(m_data.connectionData()->port);
	}
	else {
		chkPortDefault->setChecked(true);
/* @todo default port # instead of 0 */
		customPortEdit->setValue(0);
	}
	userEdit->setText(m_data.connectionData()->userName);
	passwordEdit->setText(m_data.connectionData()->password);
	titleEdit->setText(m_data.caption());

	if (shortcutFileName.isEmpty()) {
		d->btnSaveChanges->hide();
		chkSavePassword->hide();
	}
	else {
		chkSavePassword->setChecked(!m_data.connectionData()->password.isEmpty());
		if (!QFileInfo(shortcutFileName).isWritable()) {
			d->btnSaveChanges->setEnabled(false);
		}
	}
}

KPushButton* KexiDBConnectionWidget::saveChangesButton() const
{
	return d->btnSaveChanges;
}

KexiProjectData KexiDBConnectionWidget::data()
{
	return m_data;
}

void KexiDBConnectionWidget::slotLocationBGrpClicked(int id)
{
	hostLbl->setEnabled(id==1);
	hostEdit->setEnabled(id==1);
}

void KexiDBConnectionWidget::slotCBToggled(bool on)
{
	if (sender()==chkPortDefault) {
		customPortEdit->setEnabled(!on);
	}
//	else if (sender()==chkSocketDefault) {
//		customSocketEdit->setEnabled(!on);
//	}
}

//-----------

KexiDBConnectionTabWidget::KexiDBConnectionTabWidget( QWidget* parent, const char* name )
 : KTabWidget( parent, name )
{
	mainWidget = new KexiDBConnectionWidget( this, "mainWidget" );
	mainWidget->layout()->setMargin(KDialog::marginHint());
	addTab( mainWidget, i18n("&Information") );

//	QVBox *page2 = new QVBox(this);
//	page2->setMargin(KDialog::marginHint());
//	page2->setSpacing(KDialog::spacingHint());
//	QLabel *lbl = new QLabel(i18n("&Description:"), page2);
//	m_descriptionEdit = new KTextEdit(page2);
//	lbl->setBuddy(m_descriptionEdit);
	detailsWidget = new KexiDBConnectionWidgetDetailsBase(this, "detailsWidget");
	addTab( detailsWidget, i18n("&Details") );
}

KexiDBConnectionTabWidget::~KexiDBConnectionTabWidget()
{
}

void KexiDBConnectionTabWidget::setData(const KexiProjectData& data, const QString& shortcutFileName)
{
	mainWidget->setData( data, shortcutFileName );
	detailsWidget->chkUseSocket->setChecked( !data.constConnectionData()->localSocketFileName.isEmpty() );
	detailsWidget->customSocketEdit->setURL(data.constConnectionData()->localSocketFileName);
	detailsWidget->chkSocketDefault->setChecked(data.constConnectionData()->localSocketFileName.isEmpty());
	detailsWidget->descriptionEdit->setText( data.description() );
}

KexiProjectData KexiDBConnectionTabWidget::currentData()
{
	KexiProjectData data;

//! @todo check if that's database of connection shortcut. Now we're assuming db shortcut only!

	// collect data from the form's fields
//	if (d->isDatabaseShortcut) {
		data.setCaption( mainWidget->titleEdit->text() );
		data.setDescription( detailsWidget->descriptionEdit->text() );
		data.connectionData()->description = QString::null;
		data.connectionData()->connName = QString::null; /* connection name is not specified... */
		data.setDatabaseName( mainWidget->nameEdit->text() );
//	}
/*	else {
		data.setCaption( QString::null );
		data.connectionData()->connName = config.readEntry("caption");
		data.setDescription( QString::null );
		data.connectionData()->description = config.readEntry("comment");
		data.setDatabaseName( QString::null );
	}*/
	data.connectionData()->driverName = mainWidget->driversCombo->selectedDriverName();

/*	if (data.connectionData()->driverName.isEmpty()) {
		//ERR: "No valid "engine" field specified for %1 section" group
		return false;
	}*/
	data.connectionData()->hostName = 
		(mainWidget->locationBGrp->selectedId()==1/*remote*/) ? mainWidget->hostEdit->text()
		: QString::null;
	data.connectionData()->port = mainWidget->chkPortDefault->isChecked() 
		? 0 : mainWidget->customPortEdit->value();
	data.connectionData()->localSocketFileName = detailsWidget->chkSocketDefault->isChecked() 
		? QString::null : detailsWidget->customSocketEdit->url();
//UNSAFE!!!!
	data.connectionData()->userName = mainWidget->userEdit->text();
	data.connectionData()->password = mainWidget->passwordEdit->text();
/* @todo add "options=", eg. as string list? */
	return data;
}

bool KexiDBConnectionTabWidget::savePasswordSelected() const
{
	return mainWidget->chkSavePassword->isChecked();
}

//--------

//! @todo set proper help ctxt ID

KexiDBConnectionDialog::KexiDBConnectionDialog(const KexiProjectData& data, const QString& shortcutFileName)
 : KDialogBase(0, "dlg", true, i18n("Open database"), 
	KDialogBase::User1|KDialogBase::Cancel|KDialogBase::Help,
	KDialogBase::User1, false, KGuiItem(i18n("&Open"), "fileopen", i18n("Open database connection")))
{
	connect( this, SIGNAL(user1Clicked()), this, SLOT(accept()));

	tabWidget = new KexiDBConnectionTabWidget(this, "tabWidget");
	tabWidget->setData(data, shortcutFileName);
	setMainWidget(tabWidget);

	adjustSize();
	resize(width(), tabWidget->height());
	if (tabWidget->mainWidget->nameEdit->text().isEmpty())
		tabWidget->mainWidget->nameEdit->setFocus();
	else if (tabWidget->mainWidget->userEdit->text().isEmpty())
		tabWidget->mainWidget->userEdit->setFocus();
	else if (tabWidget->mainWidget->passwordEdit->text().isEmpty())
		tabWidget->mainWidget->passwordEdit->setFocus();
	else //back
		tabWidget->mainWidget->nameEdit->setFocus();
}

KexiDBConnectionDialog::~KexiDBConnectionDialog()
{
}

#include "kexidbconnectionwidget.moc"

