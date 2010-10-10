/* This file is part of the KDE project
   Copyright (C) 2006 Jarosław Staniek <staniek@kde.org>

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

#include "kexiactioncategories.h"

#include <kdebug.h>

#include <qmap.h>
#include <kexi_global.h>

namespace Kexi
{

//! @internal
class ActionInternal
{
public:
    ActionInternal(int _categories)
            : categories(_categories)
            , supportedObjectTypes(0)
            , allObjectTypesAreSupported(false) {
    }
    ~ActionInternal() {
        delete supportedObjectTypes;
    }
    int categories;
    QSet<int> *supportedObjectTypes;
    bool allObjectTypesAreSupported;
};

K_GLOBAL_STATIC(ActionCategories, Kexi_actionCategories)

//! @internal
class ActionCategories::Private
{
public:
    Private() {
    }
    ~Private() {
        qDeleteAll(actions);
        actions.clear();
    }

    QMap<QByteArray, ActionInternal*> actions;
};

KEXICORE_EXPORT ActionCategories *actionCategories()
{
    return Kexi_actionCategories;
}

} // Kexi

using namespace Kexi;

//----------------------------------

ActionCategories::ActionCategories()
        : d(new Private())
{
}

ActionCategories::~ActionCategories()
{
    delete d;
}

void ActionCategories::addAction(const char* name, int categories,
                                 KexiPart::ObjectType supportedObjectType1, KexiPart::ObjectType supportedObjectType2,
                                 KexiPart::ObjectType supportedObjectType3, KexiPart::ObjectType supportedObjectType4,
                                 KexiPart::ObjectType supportedObjectType5, KexiPart::ObjectType supportedObjectType6,
                                 KexiPart::ObjectType supportedObjectType7, KexiPart::ObjectType supportedObjectType8)
{
    ActionInternal * a = d->actions.value(name);
    if (a) {
        a->categories |= categories;
    } else {
        a = new ActionInternal(categories);
        d->actions.insert(name, a);
    }
    if (supportedObjectType1 > 0) {
        if (!a->supportedObjectTypes)
            a->supportedObjectTypes = new QSet<int>();
        a->supportedObjectTypes->insert(supportedObjectType1);
        if (supportedObjectType2 > 0) {
            a->supportedObjectTypes->insert(supportedObjectType2);
            if (supportedObjectType3 > 0) {
                a->supportedObjectTypes->insert(supportedObjectType3);
                if (supportedObjectType4 > 0) {
                    a->supportedObjectTypes->insert(supportedObjectType4);
                    if (supportedObjectType5 > 0) {
                        a->supportedObjectTypes->insert(supportedObjectType5);
                        if (supportedObjectType6 > 0) {
                            a->supportedObjectTypes->insert(supportedObjectType6);
                            if (supportedObjectType7 > 0) {
                                a->supportedObjectTypes->insert(supportedObjectType7);
                                if (supportedObjectType8 > 0) {
                                    a->supportedObjectTypes->insert(supportedObjectType8);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void ActionCategories::setAllObjectTypesSupported(const char* name, bool set)
{
    ActionInternal * a = d->actions.value(name);
    if (a)
        a->allObjectTypesAreSupported = set;
    else
        kWarning() << "no such action:" << name;
}

int ActionCategories::actionCategories(const char* name) const
{
    const ActionInternal * a = d->actions.value(name);
    return a ? a->categories : 0;
}

bool ActionCategories::actionSupportsObjectType(const char* name, KexiPart::ObjectType objectType) const
{
    const ActionInternal * a = d->actions.value(name);
    if (a && a->allObjectTypesAreSupported)
        return true;
    return (a && a->supportedObjectTypes) ? a->supportedObjectTypes->contains(objectType) : false;
}
