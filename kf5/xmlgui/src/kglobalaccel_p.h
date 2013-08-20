/* This file is part of the KDE libraries
    Copyright (C) 2001,2002 Ellis Whitehead <ellis@kde.org>
    Copyright (C) 2006 Hamish Rodda <rodda@kde.org>
    Copyright (C) 2007 Andreas Hartmetz <ahartmetz@gmail.com>

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
    Boston, MA 02110-1301, USA.
*/

#ifndef KGLOBALACCEL_P_H
#define KGLOBALACCEL_P_H

#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QKeySequence>
#include <QList>
#include <QWidget>

#include "kglobalaccel.h"
#include "kglobalaccel_interface.h"
#include "kglobalaccel_component_interface.h"

enum SetShortcutFlag
    {
    SetPresent =2,
    NoAutoloading = 4,
    IsDefault = 8
    };


class KGlobalAccelPrivate
{
public:
    enum ShortcutType {
      /// The shortcut will immediately become active but may be reset to "default".
      ActiveShortcut = 0x1,
      /// The shortcut is a default shortcut - it becomes active when somebody decides to
      /// reset shortcuts to default.
      DefaultShortcut = 0x2
    };

    Q_DECLARE_FLAGS(ShortcutTypes, ShortcutType)
    enum Removal {
        SetInactive = 0,    ///< Forget the action in this class and mark it as not present in the KDED module
        UnRegister          ///< Remove any trace of the action in this class and in the KDED module
    };
    KGlobalAccelPrivate(KGlobalAccel*);

    ///Propagate any shortcut changes to the KDED module that does the bookkeeping
    ///and the key grabbing.
    void updateGlobalShortcut(QAction *action, ShortcutTypes actionFlags, KGlobalAccel::GlobalShortcutLoading globalFlags);

    ///Register the action in this class and in the KDED module
    bool doRegister(QAction *action);   //"register" is a C keyword :p
    ///cf. the RemoveAction enum
    void remove (QAction *action, Removal r);

    //"private" helpers
    QString componentUniqueForAction(const QAction *action);
    QString componentFriendlyForAction(const QAction *action);
    QStringList makeActionId(const QAction *action);
    QList<int> intListFromShortcut(const QList<QKeySequence> &cut);
    QList<QKeySequence> shortcutFromIntList(const QList<int> &list);

    //private slot implementations
    void _k_invokeAction(const QString &, const QString &,qlonglong);
    void _k_shortcutGotChanged(const QStringList&, const QList<int>&);
    void _k_serviceOwnerChanged(const QString& name, const QString& oldOwner, const QString& newOwner);
    void reRegisterAll();

    //for all actions with (isEnabled() && globalShortcutAllowed())
    QMultiHash<QString, QAction *> nameToAction;
    QSet<QAction *> actions;
    QWidget actionsWidget; // Used to know when an action is deleted (via event filter)

    bool enabled;

    org::kde::KGlobalAccel iface;

    //! Get the component @p componentUnique. If @p remember is true the instance is cached and we
    //! subscribe to signals about changes to the component.
    org::kde::kglobalaccel::Component *getComponent(const QString &componentUnique, bool remember);

    //! Our owner
    KGlobalAccel *q;

    //! The components the application is using
    QHash<QString, org::kde::kglobalaccel::Component *> components;
    QMap<const QAction*, QList<QKeySequence> > actionDefaultShortcuts;
    QMap<const QAction*, QList<QKeySequence> > actionShortcuts;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KGlobalAccelPrivate::ShortcutTypes)

#endif
