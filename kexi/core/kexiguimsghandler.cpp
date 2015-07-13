/* This file is part of the KDE project
   Copyright (C) 2004-2013 Jarosław Staniek <staniek@kde.org>

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
#include <kexiutils/utils.h>

#include <KDbUtils>

#include <KMessageBox>
#include <KLocalizedString>

KexiGUIMessageHandler::KexiGUIMessageHandler(QWidget *parent)
        : KDbMessageHandler(parent)
{
}

KexiGUIMessageHandler::~KexiGUIMessageHandler()
{
}

KexiGUIMessageHandler* KexiGUIMessageHandler::guiRedirection()
{
    return dynamic_cast<KexiGUIMessageHandler*>(redirection());
}

void
KexiGUIMessageHandler::showErrorMessage(const QString& message, KDbResultable* resultable)
{
    if (!messagesEnabled()) {
        return;
    }
    if (guiRedirection()) {
        guiRedirection()->showErrorMessage(message, resultable);
        return;
    }
    QString msg(message);
    if (!resultable) {
        showErrorMessage(msg, QString());
        return;
    }
    QString details;
    KDb::getHTMLErrorMesage(*resultable, &msg, &details);
    showErrorMessage(msg, details);
}

void
KexiGUIMessageHandler::showErrorMessage(const QString &title, const QString &details)
{
    if (!messagesEnabled()) {
        return;
    }
    if (guiRedirection()) {
        guiRedirection()->showErrorMessage(title, details);
        return;
    }
    showMessage(Error, title, details);
}

void
KexiGUIMessageHandler::showSorryMessage(const QString &title, const QString &details)
{
    if (!messagesEnabled()) {
        return;
    }
    if (guiRedirection()) {
        guiRedirection()->showSorryMessage(title, details);
        return;
    }
    showMessage(Sorry, title, details);
}

void KexiGUIMessageHandler::showErrorMessage(const QString &message, const QString &details,
                                             KDbResultable *resultable)
{
    if (!messagesEnabled()) {
        return;
    }
    if (guiRedirection()) {
        guiRedirection()->showErrorMessage(message, details, resultable);
        return;
    }
    QString msg(message);
    if (!resultable) {
        showErrorMessage(msg, details);
        return;
    }
    QString _details(details);
    KDb::getHTMLErrorMesage(*resultable, &msg, &_details);
    showErrorMessage(msg, _details);
}

void
KexiGUIMessageHandler::showErrorMessage(Kexi::ObjectStatus *status)
{
    if (!messagesEnabled()) {
        return;
    }
    if (guiRedirection()) {
        guiRedirection()->showErrorMessage(status);
        return;
    }
    showErrorMessage("", status);
}

void
KexiGUIMessageHandler::showErrorMessage(const QString &message, Kexi::ObjectStatus *status)
{
    if (!messagesEnabled()) {
        return;
    }
    if (guiRedirection()) {
        guiRedirection()->showErrorMessage(message, status);
        return;
    }
    if (status && status->error()) {
        QString msg(message);
        if (msg.isEmpty() || msg == status->message) {
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
        showErrorMessage(msg, desc, status->resultable());
    } else {
        showErrorMessage(message, QString());
    }
    status->clearStatus();
}

void
KexiGUIMessageHandler::showMessage(MessageType type,
                                   const QString &title, const QString &details,
                                   const QString& dontShowAgainName)
{
    if (!messagesEnabled()) {
        return;
    }
    if (guiRedirection()) {
        guiRedirection()->showMessage(type, title, details, dontShowAgainName);
        return;
    }
    //'wait' cursor is a nonsense now
    KexiUtils::removeWaitCursor();

    QString msg(title);
    if (title.isEmpty())
        msg = xi18n("Unknown error");
    msg = "<qt><p>" + msg + "</p>";
    if (!details.isEmpty()) {
        switch (type) {
        case Information:
            KMessageBox::information(parentWidget(), title, dontShowAgainName);
            break;
        case Error:
            KMessageBox::detailedError(parentWidget(), msg, details);
            break;
        case Warning:
            showWarningContinueMessage(title, details, dontShowAgainName);
            break;
        default: //Sorry
            KMessageBox::detailedSorry(parentWidget(), msg, details);
        }
    } else {
        KMessageBox::DialogType msgType;
        switch (type) {
        case Information: msgType = KMessageBox::Information;
            break;
        case Error: msgType = KMessageBox::Error;
            break;
        default:
            msgType = KMessageBox::Sorry;
        }
        KMessageBox::messageBox(parentWidget(), msgType, msg);
    }
}

void KexiGUIMessageHandler::showWarningContinueMessage(const QString &title,
                                                       const QString &details,
                                                       const QString& dontShowAgainName)
{
    if (!messagesEnabled()) {
        return;
    }
    if (guiRedirection()) {
        guiRedirection()->showWarningContinueMessage(title, details, dontShowAgainName);
        return;
    }
    if (!KMessageBox::shouldBeShownContinue(dontShowAgainName))
        return;
    KMessageBox::warningContinueCancel(parentWidget(),
                                       title + (details.isEmpty() ? QString() : (QString("\n") + details)),
                                       QString(),
                                       KStandardGuiItem::cont(),
                                       KStandardGuiItem::cancel(),
                                       dontShowAgainName,
                                       KMessageBox::Notify | KMessageBox::AllowLink);
}

static KGuiItem toGuiItem(const KDbGuiItem &item)
{
    KGuiItem result;
    if (item.hasProperty("text")) {
        result.setText(item.property("text").toString());
    }
    if (item.hasProperty("icon")) {
        result.setIcon(item.property("icon").value<QIcon>());
    }
    if (item.hasProperty("iconName")) {
        result.setIconName(item.property("iconName").toString());
    }
    if (item.hasProperty("toolTip")) {
        result.setToolTip(item.property("toolTip").toString());
    }
    if (item.hasProperty("whatsThis")) {
        result.setWhatsThis(item.property("whatsThis").toString());
    }
    return result;
}

KDbMessageHandler::ButtonCode KexiGUIMessageHandler::askQuestion(
                                       KDbMessageHandler::QuestionType messageType,
                                       const QString &message,
                                       const QString &caption,
                                       KDbMessageHandler::ButtonCode defaultResult,
                                       const KDbGuiItem &buttonYes,
                                       const KDbGuiItem &buttonNo,
                                       const QString &dontShowAskAgainName,
                                       KDbMessageHandler::Options options,
                                       KDbMessageHandler* msgHandler)

//                                               KMessageBox::DialogType dlgType, KMessageBox::ButtonCode defaultResult,
//                                               const KGuiItem &buttonYes,
//                                               const KGuiItem &buttonNo,
//                                               const QString &dontShowAskAgainName,
//                                               KMessageBox::Options options)
{
    if (!messagesEnabled()) {
        return defaultResult;
    }
    if (redirection()) {
        return redirection()->askQuestion(messageType, message, caption, defaultResult,
                                          buttonYes, buttonNo, dontShowAskAgainName,
                                          options, msgHandler);
    }
    KMessageBox::Options kmsgboxOptions = 0;
    if (options <= (KDbMessageHandler::Notify|KDbMessageHandler::AllowLink|KDbMessageHandler::Dangerous)) {
        kmsgboxOptions = static_cast<KMessageBox::Options>(int(options));
    }

    if (KDbMessageHandler::WarningContinueCancel == messageType) {
        return static_cast<KDbMessageHandler::ButtonCode>(
            KMessageBox::warningContinueCancel(parentWidget(),
                message, caption, toGuiItem(buttonYes), KStandardGuiItem::cancel(),
                dontShowAskAgainName, kmsgboxOptions));
    }
    else {
        return static_cast<KDbMessageHandler::ButtonCode>(
                    KMessageBox::messageBox(parentWidget(),
                                       static_cast<KMessageBox::DialogType>(messageType),
                                       message, caption, toGuiItem(buttonYes),
                                       toGuiItem(buttonNo),
                                       KStandardGuiItem::cancel(),
                                       dontShowAskAgainName, kmsgboxOptions));
    }
}

void KexiGUIMessageHandler::showErrorMessage(KDbMessageHandler::MessageType messageType,
                                             const QString &message, const QString &details,
                                             const QString &caption)
{
    if (!messagesEnabled()) {
        return;
    }
    if (redirection()) {
        redirection()->showErrorMessage(messageType, message, details, caption);
        return;
    }
    showMessage(messageType, message, details);
}

void KexiGUIMessageHandler::showErrorMessage(const KDbResult& result,
                                             KDbMessageHandler::MessageType messageType,
                                             const QString& message,
                                             const QString& caption)
{
    if (!messagesEnabled()) {
        return;
    }
    if (redirection()) {
        redirection()->showErrorMessage(result, messageType, message, caption);
        return;
    }
    showMessage(messageType, result.message() + '\n' + message, QString());
}
