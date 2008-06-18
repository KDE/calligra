/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexiguimsghandler.h"

#include "kexi.h"
#include <kexidb/utils.h>
#include <kexiutils/utils.h>

#include <kmessagebox.h>
#include <kdialog.h>

KexiGUIMessageHandler::KexiGUIMessageHandler(QWidget *parent)
: KexiDB::MessageHandler(parent)
{
}

KexiGUIMessageHandler::~KexiGUIMessageHandler()
{
}

/*virtual*/
void
KexiGUIMessageHandler::showErrorMessage(KexiDB::Object *obj, 
	const QString& msg)
{
	QString _msg(msg);
	if (!obj) {
		showErrorMessage(_msg);
		return;
	}
	QString details;
	KexiDB::getHTMLErrorMesage(obj, _msg, details);
	showErrorMessage(_msg, details);
}

/*virtual*/
void
KexiGUIMessageHandler::showErrorMessage(const QString &title, const QString &details)
{
	showMessage(Error, title, details);
}

void
KexiGUIMessageHandler::showSorryMessage(const QString &title, const QString &details)
{
	showMessage(Sorry, title, details);
}

void KexiGUIMessageHandler::showErrorMessage(const QString &msg, const QString &details, 
	KexiDB::Object *obj)
{
	QString _msg(msg);
	if (!obj) {
		showErrorMessage(_msg, details);
		return;
	}
	QString _details(details);
	KexiDB::getHTMLErrorMesage(obj, _msg, _details);
	showErrorMessage(_msg, _details);
}

void
KexiGUIMessageHandler::showErrorMessage(Kexi::ObjectStatus *status)
{
	showErrorMessage("", status);
}

void
KexiGUIMessageHandler::showErrorMessage(const QString &message, Kexi::ObjectStatus *status)
{
	if (status && status->error()) {
		QString msg(message);
		if (msg.isEmpty() || msg==status->message) {
			msg = status->message;
			status->message = status->description;
			status->description = "";
		}
		QString desc;
		if (!status->message.isEmpty()) {
			if (status->description.isEmpty()) {
				desc = status->message;
			} else {
				msg += (QString("<br><br>") + status->message);
				desc = status->description;
			}
		}
		showErrorMessage(msg, desc, status->dbObject());
	}
	else {
		showErrorMessage(message);
	}
	status->clearStatus();
}

void
KexiGUIMessageHandler::showMessage(MessageType type,
	const QString &title, const QString &details, const QString& dontShowAgainName)
{
	if (!m_enableMessages)
		return;

	//'wait' cursor is a nonsense now
	KexiUtils::removeWaitCursor();

	QString msg(title);
	if (title.isEmpty())
		msg = i18n("Unknown error");
	msg = "<qt><p>"+msg+"</p>";
	if (!details.isEmpty()) {
		switch (type) {
		case Error:
			KMessageBox::detailedError(m_messageHandlerParentWidget, msg, details);
			break;
		case Warning:
			showWarningContinueMessage(title, details, dontShowAgainName);
			break;
		default: //Sorry
			KMessageBox::detailedSorry(m_messageHandlerParentWidget, msg, details);
		}
	}
	else {
		KMessageBox::messageBox(m_messageHandlerParentWidget, 
			type==Error ? KMessageBox::Error : KMessageBox::Sorry, msg);
	}
}

void KexiGUIMessageHandler::showWarningContinueMessage(const QString &title, const QString &details,
	const QString& dontShowAgainName)
{
	if (!KMessageBox::shouldBeShownContinue(dontShowAgainName))
		return;
#if 0 //sebsauer 20061123
	KDialogBase *dialog = new KDialogBase(
		i18n("Warning"), KDialogBase::Yes, KDialogBase::Yes, KDialogBase::No,
		m_messageHandlerParentWidget, "warningContinue", true, true, KStandardGuiItem::cont() );
	bool checkboxResult = false;
	KMessageBox::createKMessageBox(dialog, QMessageBox::Warning, 
		title + (details.isEmpty() ? QString() : (QString("\n")+details)), QStringList(),
		dontShowAgainName.isEmpty() ? QString() : i18n("Do not show this message again"),
		&checkboxResult, 0);
	if (checkboxResult)
		KMessageBox::saveDontShowAgainContinue(dontShowAgainName);
#else
	KMessageBox::warningContinueCancel(m_messageHandlerParentWidget, 
    title + (details.isEmpty() ? QString() : (QString("\n")+details)));
#endif
}

int KexiGUIMessageHandler::askQuestion( const QString& message, 
	KMessageBox::DialogType dlgType, KMessageBox::ButtonCode defaultResult,
	const KGuiItem &buttonYes, 
	const KGuiItem &buttonNo,
	const QString &dontShowAskAgainName,
	KMessageBox::Options options )
{
	Q_UNUSED(defaultResult);
	if (KMessageBox::WarningContinueCancel == dlgType)
		return KMessageBox::warningContinueCancel(m_messageHandlerParentWidget,
			message, QString(), buttonYes, KStandardGuiItem::cancel(),
			dontShowAskAgainName, options);
	else
		return KMessageBox::messageBox(m_messageHandlerParentWidget,
			dlgType, message, QString(), buttonYes, buttonNo, KStandardGuiItem::cancel(),
			dontShowAskAgainName, options);
}

