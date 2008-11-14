/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2008 Jarosław Staniek <staniek@kde.org>

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

#include "kexipartinfo_p.h"

#include <kexidb/global.h>
//Added by qt3to4:
#include <Q3CString>

using namespace KexiPart;

Info::Private::Private(const KService::Ptr& aPtr)
        : ptr(aPtr)
        , groupName(aPtr->name())
//        , mimeType(aPtr->property("X-Kexi-TypeMime").toString())
        , itemIcon(aPtr->property("X-Kexi-ItemIcon", QVariant::String).toString())
        , objectName(aPtr->property("X-Kexi-TypeName", QVariant::String).toString())
//        , projectPartID( aPtr->property("X-Kexi-TypeId").toInt() )
        , partClass( aPtr->property("X-Kexi-Class", QVariant::String).toString() )
        , broken(false)
        , idStoredInPartDatabase(false)
{
    QVariant val = ptr->property("X-Kexi-NoObject", QVariant::Bool);
    isVisibleInNavigator = val.isValid() ? !val.toBool() : true;

    val = aPtr->property("X-Kexi-PropertyEditorAlwaysVisibleInDesignMode", QVariant::Bool);
    isPropertyEditorAlwaysVisibleInDesignMode = val.isValid() ? val.toBool() : false;
#if 0
    if (projectPartID == 0) {
        if (isVisibleInNavigator) {
            kWarning() << "Could not found project part ID! (name: '" << objectName 
                << "'). Possible problem with installation of the .desktop files for Kexi plugins";
            isVisibleInNavigator = false;
        }
        projectPartID = -1;
    }
#endif
}

Info::Private::Private()
#if 0 //moved as internal to KexiProject
        : projectPartID(-1) //OK?
#endif
        : broken(false)
        , isVisibleInNavigator(false)
        , idStoredInPartDatabase(false)
        , isPropertyEditorAlwaysVisibleInDesignMode(false)
{
}

//------------------------------

Info::Info(KService::Ptr ptr)
        : d(new Private(ptr))
{
}

Info::Info()
        : d(new Private())
{
}

Info::~Info()
{
    delete d;
}

QString Info::groupName() const
{
    return d->groupName;
}

QString Info::partClass() const
{
    return d->partClass;
}

QString Info::itemIcon() const
{
    return d->itemIcon;
}

QString Info::createItemIcon() const
{
    return d->itemIcon + "_newobj";
}

QString Info::objectName() const
{
    return d->objectName;
}

KService::Ptr Info::ptr() const
{
    return d->ptr;
}

bool Info::isBroken() const
{
    return d->broken;
}

bool Info::isVisibleInNavigator() const
{
    return d->isVisibleInNavigator;
}

#if 0 //moved as internal to KexiProject
int Info::projectPartID() const
{
    return d->projectPartID;
}

void Info::setProjectPartID(int id)
{
    d->projectPartID = id;
}
#endif

void Info::setBroken(bool broken, const QString& errorMessage)
{
    d->broken = broken; d->errorMessage = errorMessage;
}

QString Info::errorMessage() const
{
    return d->errorMessage;
}

void Info::setIdStoredInPartDatabase(bool set)
{
    d->idStoredInPartDatabase = set;
}

bool Info::isIdStoredInPartDatabase() const
{
    return d->idStoredInPartDatabase;
}

bool Info::isDataExportSupported() const
{
    QVariant val = d->ptr ? d->ptr->property("X-Kexi-SupportsDataExport") : QVariant();
    return val.isValid() ? val.toBool() : false;
}

bool Info::isPrintingSupported() const
{
    QVariant val = d->ptr ? d->ptr->property("X-Kexi-SupportsPrinting") : QVariant();
    return val.isValid() ? val.toBool() : false;
}

bool Info::isExecuteSupported() const
{
    QVariant val = d->ptr ? d->ptr->property("X-Kexi-SupportsExecution") : QVariant();
    return val.isValid() ? val.toBool() : false;
}

bool Info::isPropertyEditorAlwaysVisibleInDesignMode() const
{
    return d->isPropertyEditorAlwaysVisibleInDesignMode;
}

//--------------

QString KexiPart::nameForCreateAction(const Info& info)
{
    return info.objectName() + "part_create";
}
