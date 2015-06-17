/* This file is part of the KDE project
   Copyright (C) 2012-2013 Jarosław Staniek <staniek@kde.org>

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

#include "KexiAssistantMessageHandler.h"
#include <core/kexitextmsghandler.h>
#include <kexiutils/KexiContextMessage.h>
#include <kexiutils/KexiAssistantPage.h>
#include <kexiutils/KexiAssistantWidget.h>

#include <KexiIcon.h>

#include <KLocalizedString>

#include <QAction>

class KexiAssistantMessageHandler::Private
{
public:
    Private()
        : messageWidgetActionNo(0)
        , messageWidgetActionTryAgain(0)
    {
    }

    QAction* messageWidgetActionNo;
    QAction* messageWidgetActionTryAgain;
    QPointer<KexiContextMessageWidget> messageWidget;
};

KexiAssistantMessageHandler::KexiAssistantMessageHandler()
    : KDbMessageHandler(), d(new Private)
{
}

KexiAssistantMessageHandler::~KexiAssistantMessageHandler()
{
    delete d;
}

void KexiAssistantMessageHandler::showErrorMessage(KDbMessageHandler::MessageType messageType,
                                                   const QString &message,
                                                   const QString &details,
                                                   const QString &caption)
{
    Q_UNUSED(caption);
    Q_UNUSED(messageType);
    if (!messagesEnabled()) {
        return;
    }
    QString msg(message);
    msg += details;
    KexiContextMessage contextMessage(msg);
    //! @todo hide details by default
    if (!d->messageWidgetActionTryAgain) {
        d->messageWidgetActionTryAgain = new QAction(
            koIcon("view-refresh"), xi18n("Try Again"), dynamic_cast<QWidget*>(this));
        QObject::connect(d->messageWidgetActionTryAgain, SIGNAL(triggered()),
                         dynamic_cast<QWidget*>(this), SLOT(tryAgainActionTriggered()));
    }
    if (!d->messageWidgetActionNo) {
        d->messageWidgetActionNo = new QAction(KStandardGuiItem::no().text(), dynamic_cast<QWidget*>(this));
        QObject::connect(d->messageWidgetActionNo, SIGNAL(triggered()),
                         dynamic_cast<QWidget*>(this), SLOT(cancelActionTriggered()));
    }
    d->messageWidgetActionNo->setText(KStandardGuiItem::cancel().text());
    contextMessage.addAction(d->messageWidgetActionTryAgain);
    contextMessage.setDefaultAction(d->messageWidgetActionNo);
    contextMessage.addAction(d->messageWidgetActionNo);
    // (delete not needed here because KexiContextMessageWidget deletes itself)
    d->messageWidget = new KexiContextMessageWidget(
        dynamic_cast<QWidget*>(this), 0 /*contents->formLayout*/,
        0/*contents->le_dbname*/, contextMessage);
    KexiAssistantWidget *assistant = dynamic_cast<KexiAssistantWidget*>(this);
    if (assistant && assistant->currentPage()) {
        if (assistant->currentPage()->focusWidget()) {
            d->messageWidget->setNextFocusWidget(assistant->currentPage()->focusWidget());
        }
        else {
            d->messageWidget->setNextFocusWidget(assistant->currentPage());
        }
    }
    d->messageWidget->setCalloutPointerDirection(KMessageWidget::Right);
    QWidget *b = calloutWidget();
    d->messageWidget->setCalloutPointerPosition(
        b->mapToGlobal(QPoint(0, b->height() / 2)));
}

void KexiAssistantMessageHandler::showErrorMessage(const KDbResult& result,
                                                   KDbMessageHandler::MessageType messageType,
                                                   const QString& message,
                                                   const QString& caption)
{
    if (!messagesEnabled()) {
        return;
    }
    if (!result.isError()) {
        showErrorMessage(messageType, message, caption);
        return;
    }
    QString msg, details;
    KexiTextMessageHandler textHandler(&msg, &details);
    textHandler.showErrorMessage(result, messageType, message, caption);
    showErrorMessage(messageType, msg, details);
}

KexiContextMessageWidget* KexiAssistantMessageHandler::messageWidget()
{
    return d->messageWidget;
}
