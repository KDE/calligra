/* This file is part of the KDE project
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>

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

#include "keximigrationpart.h"

#include <migration/importwizard.h>
#include <migration/importtablewizard.h>
#include <core/KexiMainWindowIface.h>
#include <core/kexiproject.h>
#include <core/kexipart.h>

#include <KPluginFactory>

KexiMigrationPart::KexiMigrationPart(QObject *parent, const QVariantList &args)
        : KexiInternalPart(parent, args)
{
}

KexiMigrationPart::~KexiMigrationPart()
{
}

QWidget *KexiMigrationPart::createWidget(const char* widgetClass,
        QWidget *parent, const char *objName, QMap<QString, QString>* args)
{
    QWidget *w;

    if (QString(widgetClass) == "migration") {
        w = new KexiMigration::ImportWizard(parent, args);
    }
    else if (QString(widgetClass) == "importtable") {
        w = new KexiMigration::ImportTableWizard(KexiMainWindowIface::global()->project()->dbConnection(), parent);
    }
    w->setObjectName(objName);
    return w;
}

K_EXPORT_KEXI_PLUGIN( KexiMigrationPart, migration )
