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

#ifndef KEXIASSISTANTMESSAGEHANDLER_H
#define KEXIASSISTANTMESSAGEHANDLER_H

#include <KDbMessageHandler>

class KDbObject;
class KexiContextMessageWidget;

class KexiAssistantMessageHandler : public KDbMessageHandler
{
public:
    KexiAssistantMessageHandler();

    ~KexiAssistantMessageHandler();

protected:
    //! Shows error message with @a title (it is not caption) and details.
    //! Implementation for KDbMessageHandler.
    virtual void showErrorMessage(
        KDbMessageHandler::MessageType messageType,
        const QString &message,
        const QString &details = QString(),
        const QString &caption = QString()
    );

    /*! Shows error message with @a msg text. Existing error message from @a obj object
     is also copied, if present. */
    //! Implementation for KDbMessageHandler.
    virtual void showErrorMessage(
        const KDbResult& result,
        KDbMessageHandler::MessageType messageType = Error,
        const QString& message = QString(),
        const QString& caption = QString()
    );

    virtual QWidget* calloutWidget() const = 0;

    KexiContextMessageWidget* messageWidget();

private:
    class Private;
    Private * const d;
};

#endif // KEXIASSISTANTMESSAGEHANDLER_H
