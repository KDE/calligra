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

#ifndef KEXIGUIMSGHANDLER_H
#define KEXIGUIMSGHANDLER_H

#include <core/kexi.h>

#include <KDbMessageHandler>

class KEXICORE_EXPORT KexiGUIMessageHandler : public KDbMessageHandler
{
public:
    explicit KexiGUIMessageHandler(QWidget *parent = 0);
    virtual ~KexiGUIMessageHandler();

    using KDbMessageHandler::showErrorMessage;

    void showErrorMessage(const QString &message, const QString &details, KDbResultable *resultable);
    void showErrorMessage(const QString &message, KDbResultable *resultable);
    void showErrorMessage(Kexi::ObjectStatus *status);
    void showErrorMessage(const QString &message, Kexi::ObjectStatus *status);
    void showErrorMessage(const QString &title, const QString &details);

    /*! Displays a "Sorry" message with \a title text and optional \a details. */
    void showSorryMessage(const QString &title, const QString &details = QString());

    /*! Displays a message of a type \a type, with \a title text and optional \a details.
     \a dontShowAgainName can be specified to add "Do not show again" option if \a type is Warning. */
    virtual void showMessage(MessageType type, const QString &title, const QString &details,
                             const QString& dontShowAgainName = QString());

    /*! Displays a Warning message with \a title text and optional \a details
     with "Continue" button instead "OK".
     \a dontShowAgainName can be specified to add "Do not show again" option. */
    virtual void showWarningContinueMessage(const QString &title, const QString &details = QString(),
                                            const QString& dontShowAgainName = QString());

    /*! Shows error message with @a title (it is not caption) and details. */
    virtual void showErrorMessage(
        KDbMessageHandler::MessageType messageType,
        const QString &msg,
        const QString &details = QString(),
        const QString &caption = QString()
    );

    /*! Shows error message with @a msg text. Existing error message from @a obj object
     is also copied, if present. */
    virtual void showErrorMessage(
        const KDbResult& result,
        KDbMessageHandler::MessageType messageType = Error,
        const QString& msg = QString(),
        const QString& caption = QString()
    );

    /*! Interactively asks a question. For GUI version, message boxes are used.
     @a defaultResult is returned in case when no message handler is installed.
     @a message should contain translated string.
     Value of ButtonCode is returned.
     Reimplement this. This implementation does nothing, just returns @a defaultResult. */
    virtual KDbMessageHandler::ButtonCode askQuestion(
            KDbMessageHandler::QuestionType messageType,
            const QString &message,
            const QString &caption = QString(),
            KDbMessageHandler::ButtonCode defaultResult = KDbMessageHandler::Yes,
            const KDbGuiItem &buttonYes = KDbGuiItem(),
            const KDbGuiItem &buttonNo = KDbGuiItem(),
            const QString &dontShowAskAgainName = QString(),
            KDbMessageHandler::Options options = 0,
            KDbMessageHandler* msgHandler = 0);

protected:
    //using KDbMessageHandler::showErrorMessage;
};

#endif
