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

#include <db/msghandler.h>

class KexiContextMessageWidget;

class KexiAssistantMessageHandler : public KexiDB::MessageHandler
{
public:
    KexiAssistantMessageHandler();

    ~KexiAssistantMessageHandler();

protected:
    //! Implementation for KexiDB::MessageHandler.
    virtual void showErrorMessageInternal(const QString &msg,
                                          const QString &details = QString());

    //! Implementation for KexiDB::MessageHandler.
    virtual void showErrorMessageInternal(KexiDB::Object *obj, const QString& msg = QString());

    virtual QWidget* calloutWidget() const = 0;

    KexiContextMessageWidget* messageWidget();

private:
    class Private;
    Private * const d;
};

#endif // KEXIASSISTANTMESSAGEHANDLER_H
