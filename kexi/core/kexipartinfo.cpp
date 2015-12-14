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

#include "kexipartinfo_p.h"
#include "kexipartmanager.h"
#include "KexiMainWindowIface.h"

#include <KDbGlobal>

#include <KActionCollection>

#include <QStringList>
#include <QDebug>
#include <QJsonArray>

using namespace KexiPart;

static bool isTrue(KPluginMetaData *metaData, const char* fieldName, bool defaultValue = false)
{
    QString s = metaData->value(QLatin1String(fieldName));
    if (s.isEmpty()) {
        return defaultValue;
    }
    return 0 == s.compare(QLatin1String("true"), Qt::CaseInsensitive);
}

Info::Private::Private(Info *info, const QJsonObject &rootObject)
    : untranslatedGroupName(info->value("X-Kexi-GroupName"))
    , typeName(info->value("X-Kexi-TypeName"))
    , supportedViewModes(0)
    , supportedUserViewModes(0)
    , isVisibleInNavigator(isTrue(info, "X-Kexi-VisibleInProjectNavigator"))
    , isDataExportSupported(isTrue(info, "X-Kexi-SupportsDataExport"))
    , isPrintingSupported(isTrue(info, "X-Kexi-SupportsPrinting"))
    , isExecuteSupported(isTrue(info, "X-Kexi-SupportsExecution"))
    , isPropertyEditorAlwaysVisibleInDesignMode(
          isTrue(info, "X-Kexi-PropertyEditorAlwaysVisibleInDesignMode", true))
{
    groupName = info->readTranslatedString(rootObject, "X-Kexi-GroupName", untranslatedGroupName);
    const QStringList serviceTypes = info->serviceTypes();
    if (serviceTypes.contains("Kexi/Viewer")) {
        supportedViewModes |= Kexi::DataViewMode;
    }
    if (serviceTypes.contains("Kexi/Designer")) {
        supportedViewModes |= Kexi::DesignViewMode;
    }
    if (serviceTypes.contains("Kexi/Editor")) {
        supportedViewModes |= Kexi::TextViewMode;
    }

    const QJsonArray userServiceTypes = rootObject.value("X-Kexi-ServiceTypesInUserMode").toArray();
    if (userServiceTypes.contains(QJsonValue("Kexi/Viewer"))) {
        supportedUserViewModes |= Kexi::DataViewMode;
    }
    if (userServiceTypes.contains(QJsonValue("Kexi/Designer"))) {
        supportedUserViewModes |= Kexi::DesignViewMode;
    }
    if (userServiceTypes.contains(QJsonValue("Kexi/Editor"))) {
        supportedUserViewModes |= Kexi::TextViewMode;
    }
}

Info::Private::Private()
    : supportedViewModes(0)
    , supportedUserViewModes(0)
    , isVisibleInNavigator(false)
    , isDataExportSupported(false)
    , isPrintingSupported(false)
    , isExecuteSupported(false)
    , isPropertyEditorAlwaysVisibleInDesignMode(true)
{
}

//------------------------------

/*! \return "create" QAction's name for part defined by \a info.
 The result is like "tablepart_create". */
static QString nameForCreateAction(const Info& info)
{
    return info.id() + ".create";
}

//------------------------------

KexiNewObjectAction::KexiNewObjectAction(Info* info, QObject *parent)
    : QAction(QIcon::fromTheme(KexiIconName(info->iconName())), info->name() + "...", parent)
    , m_info(info)
{
    setObjectName(nameForCreateAction(*m_info));
    // default tooltip and what's this
    setToolTip(xi18n("Create new object of type \"%1\"",
                m_info->name().toLower()));
    setWhatsThis(xi18n("Creates new object of type \"%1\"",
                    m_info->name().toLower()));
    connect(this, SIGNAL(triggered()), this, SLOT(slotTriggered()));
    connect(this, SIGNAL(newObjectRequested(KexiPart::Info*)),
            &Kexi::partManager(), SIGNAL(newObjectRequested(KexiPart::Info*)));
}

void KexiNewObjectAction::slotTriggered()
{
    emit newObjectRequested(m_info);
}

//------------------------------

//Info::Info(const QString &id, const QString &iconName,
//           const QString &objectName)
//        : KPluginMetaData(), d(new Private)
//{
//    d->iconName = iconName;
//    d->objectName = objectName;
//}

Info::Info(const QPluginLoader &loader)
    : KexiPluginMetaData(loader), d(new Private(this, rootObject()))
{
}

Info::~Info()
{
    delete d;
}

QString Info::typeName() const
{
    return d->typeName;
}

QString Info::groupName() const
{
    return d->groupName;
}

QString Info::untranslatedGroupName() const
{
    return d->untranslatedGroupName;
}

Kexi::ViewModes Info::supportedViewModes() const
{
    return d->supportedViewModes;
}

Kexi::ViewModes Info::supportedUserViewModes() const
{
    return d->supportedUserViewModes;
}

bool Info::isVisibleInNavigator() const
{
    return d->isVisibleInNavigator;
}

bool Info::isDataExportSupported() const
{
    return d->isDataExportSupported;
}

bool Info::isPrintingSupported() const
{
    return d->isPrintingSupported;
}

bool Info::isExecuteSupported() const
{
    return d->isExecuteSupported;
}

bool Info::isPropertyEditorAlwaysVisibleInDesignMode() const
{
    return d->isPropertyEditorAlwaysVisibleInDesignMode;
}

QAction* Info::newObjectAction()
{
    if (!KexiMainWindowIface::global() || !KexiMainWindowIface::global()->actionCollection()
        || !isVisibleInNavigator())
    {
        qWarning();
        return 0;
    }
    QAction *act = KexiMainWindowIface::global()->actionCollection()->action(nameForCreateAction(*this));
    if (!act) {
        act = new KexiNewObjectAction(this, KexiMainWindowIface::global()->actionCollection());
        KexiMainWindowIface::global()->actionCollection()->addAction(act->objectName(), act);
    }
    return act;
}
