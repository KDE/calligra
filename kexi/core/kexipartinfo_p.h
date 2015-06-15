/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIPROJECTPARTITEM_P_H
#define KEXIPROJECTPARTITEM_P_H

#include "kexipartinfo.h"

#include <QAction>

namespace KexiPart
{
//! @internal
class Info::Private
{
public:
    Private(Info *info, const QJsonObject &rootObject);

    //! used in StaticItem class
    Private();

    void getBooleanProperty(const KService::Ptr& aPtr, const char* name, bool* target)
    {
        QVariant val = aPtr->property(name, QVariant::Bool);
        if (val.isValid())
            *target = val.toBool();
    }

    QString errorMessage;
    QString groupName;
    QString untranslatedGroupName;
    QString typeName;

    /*! Supported modes for dialogs created by this part.
    @see KexiPart::Info::supportedViewModes() */
    Kexi::ViewModes supportedViewModes;

    /*! Supported modes for dialogs created by this part in user mode.
    @see KexiPart::Info::supportedUserViewModes() */
    Kexi::ViewModes supportedUserViewModes;

    int majorVersion;
    int minorVersion;

    bool isVisibleInNavigator;
    bool isDataExportSupported;
    bool isPrintingSupported;
    bool isExecuteSupported;
    bool isPropertyEditorAlwaysVisibleInDesignMode;
};
}

//! Helper for creating new objects.
//! On triggering, the request is passed to part manager
//! @internal
class KexiNewObjectAction : public QAction
{
    Q_OBJECT
public:
    KexiNewObjectAction(KexiPart::Info* info, QObject *parent);

Q_SIGNALS:
    void newObjectRequested(KexiPart::Info* info);

private Q_SLOTS:
    void slotTriggered();

private:
    KexiPart::Info* m_info;
};

#endif
