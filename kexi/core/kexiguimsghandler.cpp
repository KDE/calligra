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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexiguimsghandler.h"

#include "kexi.h"

#include <kexidb/utils.h>

#include <kmessagebox.h>

KexiGUIMessageHandler::KexiGUIMessageHandler(QWidget *parent)
: KexiMessageHandler(parent)
{
}

KexiGUIMessageHandler::~KexiGUIMessageHandler()
{
}

/*virtual*/
void
KexiGUIMessageHandler::showErrorMessage(const QString &msg, KexiDB::Object *obj)
{
	QString _msg = msg;
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

void KexiGUIMessageHandler::showErrorMessage(const QString &msg, const QString &details, KexiDB::Object *obj)
{
	QString _msg = msg;
	if (!obj) {
		showErrorMessage(_msg, details);
		return;
	}
	QString _details;
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
		QString msg = message;
		if (msg.isEmpty()) {
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
		showErrorMessage(message, desc, status->dbObject());
	}
	else {
		showErrorMessage(message);
	}
	status->clearStatus();
}

void
KexiGUIMessageHandler::showMessage(MessageType type,
	const QString &title, const QString &details)
{
	if (!m_enableMessages)
		return;

	//'wait' cursor is a nonsense now
	Kexi::removeWaitCursor();

	QString msg = title;
	if (title.isEmpty())
		msg = i18n("Unknown error");
	msg = "<qt><p>"+msg+"</p>";
	if (!details.isEmpty()) {
		switch (type) {
		case Error:
			KMessageBox::detailedError(m_messageHandlerParentWidget, msg, details);
			break;
		default: //Sorry
			KMessageBox::detailedSorry(m_messageHandlerParentWidget, msg, details);
		}
	}
	else {
		KMessageBox::messageBox(m_messageHandlerParentWidget, type==Error?KMessageBox::Error:KMessageBox::Sorry, msg);
	}
}
