/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2011 Jarosław Staniek <staniek@kde.org>

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
#include "KexiMainWindowIface.h"

#include <db/global.h>
#include <KActionCollection>

using namespace KexiPart;

Info::Private::Private(const KService::Ptr& aPtr)
        : ptr(aPtr)
        , instanceCaption(aPtr->name())
        , groupName(aPtr->genericName())
//        , mimeType(aPtr->property("X-Kexi-TypeMime").toString())
        , itemIconName(aPtr->property("X-Kexi-ItemIcon", QVariant::String).toString())
        , objectName(aPtr->property("X-Kexi-TypeName", QVariant::String).toString())
//        , projectPartID( aPtr->property("X-Kexi-TypeId").toInt() )
        , partClass(aPtr->property("X-Kexi-Class", QVariant::String).toString())
        , broken(false)
        , idStoredInPartDatabase(false)
{
    bool dataView = true;
    getBooleanProperty(aPtr, "X-Kexi-SupportsDataView", &dataView);
    if (dataView) {
        supportedViewModes |= Kexi::DataViewMode;
    }
    bool designView = true;
    getBooleanProperty(aPtr, "X-Kexi-SupportsDesignView", &designView);
    if (designView) {
        supportedViewModes |= Kexi::DesignViewMode;
    }
    bool textView = false;
    getBooleanProperty(aPtr, "X-Kexi-SupportsTextView", &textView);
    if (textView) {
        supportedViewModes |= Kexi::TextViewMode;
    }
    dataView = true;
    getBooleanProperty(aPtr, "X-Kexi-SupportsDataViewInUserMode", &dataView);
    if (dataView) {
        supportedUserViewModes |= Kexi::DataViewMode;
    }
    designView = false;
    getBooleanProperty(aPtr, "X-Kexi-SupportsDesignViewInUserMode", &designView);
    if (designView) {
        supportedUserViewModes |= Kexi::DesignViewMode;
    }
    textView = false;
    getBooleanProperty(aPtr, "X-Kexi-SupportsTextViewInUserMode", &textView);
    if (textView) {
        supportedUserViewModes |= Kexi::TextViewMode;
    }
    
    isVisibleInNavigator = true;
    getBooleanProperty(aPtr, "X-Kexi-NoObject", &isVisibleInNavigator);

    isPropertyEditorAlwaysVisibleInDesignMode = true;
    getBooleanProperty(aPtr, "X-Kexi-PropertyEditorAlwaysVisibleInDesignMode",
                       &isPropertyEditorAlwaysVisibleInDesignMode);

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

KexiNewObjectAction::KexiNewObjectAction(Info* info, QObject *parent)
    : KAction(KIcon(info->createItemIconName()), info->instanceCaption() + "...", parent)
    , m_info(info)
{
    setObjectName(KexiPart::nameForCreateAction(*m_info));
    // default tooltip and what's this
    setToolTip(i18n("Create new object of type \"%1\"",
                m_info->instanceCaption().toLower()));
    setWhatsThis(i18n("Creates new object of type \"%1\"",
                    m_info->instanceCaption().toLower()));
    connect(this, SIGNAL(triggered()), this, SLOT(slotTriggered()));
    connect(this, SIGNAL(newObjectRequested(KexiPart::Info*)),
            &Kexi::partManager(), SIGNAL(newObjectRequested(KexiPart::Info*)));
}

void KexiNewObjectAction::slotTriggered()
{
    emit newObjectRequested(m_info);
}

//------------------------------

Info::Info(KService::Ptr ptr)
        : d(new Private(ptr))
{
    if (KexiMainWindowIface::global()) {
        KexiNewObjectAction *act = new KexiNewObjectAction(
            this,
            KexiMainWindowIface::global()->actionCollection());
        
        if (KexiMainWindowIface::global()->actionCollection()) {
            KexiMainWindowIface::global()->actionCollection()->addAction(act->objectName(), act);
        }
    }
}

Info::Info(const QString &partClass, const QString &itemIconName,
           const QString &objectName)
        : d(new Private)
{                       
    d->partClass = partClass;
    d->itemIconName = itemIconName;
    d->objectName = objectName;
}                       
                       
Info::~Info()
{
    delete d;
}

QString Info::groupName() const
{
    return d->groupName;
}

QString Info::instanceCaption() const
{
    return d->instanceCaption;
}

QString Info::partClass() const
{
    return d->partClass;
}

QString Info::itemIconName() const
{
    return d->itemIconName;
}

QString Info::createItemIconName() const
{
    return d->itemIconName + QLatin1String("_newobj");
}

QString Info::objectName() const
{
    return d->objectName;
}

Kexi::ViewModes Info::supportedViewModes() const
{
    return d->supportedViewModes;
}

Kexi::ViewModes Info::supportedUserViewModes() const
{
    return d->supportedUserViewModes;
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

#include "kexipartinfo_p.moc"
