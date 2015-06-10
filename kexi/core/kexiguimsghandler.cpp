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

void
KexiGUIMessageHandler::showErrorMessage(const QString& msg, KDbResultable* resultable)
{
    QString _msg(msg);
    if (!resultable) {
        showErrorMessage(_msg, QString());
        return;
    }
    QString details;
    KDb::getHTMLErrorMesage(*resultable, &_msg, &details);
    showErrorMessage(_msg, details);
}

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
                                             KDbResultable *resultable)
{
    QString _msg(msg);
    if (!resultable) {
        showErrorMessage(_msg, details);
        return;
    }
    QString _details(details);
    KDb::getHTMLErrorMesage(*resultable, &_msg, &_details);
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
                                   const QString &title, const QString &details, const QString& dontShowAgainName)
{
    if (!m_enableMessages)
        return;

    //'wait' cursor is a nonsense now
    KexiUtils::removeWaitCursor();

    QString msg(title);
    if (title.isEmpty())
        msg = xi18n("Unknown error");
    msg = "<qt><p>" + msg + "</p>";
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
    } else {
        KMessageBox::messageBox(m_messageHandlerParentWidget,
                                type == Error ? KMessageBox::Error : KMessageBox::Sorry, msg);
    }
}

void KexiGUIMessageHandler::showWarningContinueMessage(const QString &title, const QString &details,
        const QString& dontShowAgainName)
{
    if (!KMessageBox::shouldBeShownContinue(dontShowAgainName))
        return;
    KMessageBox::warningContinueCancel(m_messageHandlerParentWidget,
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
    Q_UNUSED(defaultResult);
    Q_UNUSED(msgHandler);
    KMessageBox::Options kmsgboxOptions = 0;
    if (options <= (KDbMessageHandler::Notify|KDbMessageHandler::AllowLink|KDbMessageHandler::Dangerous)) {
        kmsgboxOptions = static_cast<KMessageBox::Options>(int(options));
    }

    if (KDbMessageHandler::WarningContinueCancel == messageType) {
        return static_cast<KDbMessageHandler::ButtonCode>(
            KMessageBox::warningContinueCancel(m_messageHandlerParentWidget,
                message, caption, toGuiItem(buttonYes), KStandardGuiItem::cancel(),
                dontShowAskAgainName, kmsgboxOptions));
    }
    else {
        return static_cast<KDbMessageHandler::ButtonCode>(
                    KMessageBox::messageBox(m_messageHandlerParentWidget,
                                       static_cast<KMessageBox::DialogType>(messageType),
                                       message, caption, toGuiItem(buttonYes),
                                       toGuiItem(buttonNo),
                                       KStandardGuiItem::cancel(),
                                       dontShowAskAgainName, kmsgboxOptions));
    }
}

void KexiGUIMessageHandler::showErrorMessage(KDbMessageHandler::MessageType messageType,
                                             const QString &msg, const QString &details,
                                             const QString &caption)
{
    Q_UNUSED(caption);
    showMessage(messageType, msg, details);
}

void KexiGUIMessageHandler::showErrorMessage(const KDbResult& result,
                                             KDbMessageHandler::MessageType messageType,
                                             const QString& msg,
                                             const QString& caption)
{
    Q_UNUSED(result);
    Q_UNUSED(caption);
    showMessage(messageType, result.message() + '\n' + msg, QString());
}
