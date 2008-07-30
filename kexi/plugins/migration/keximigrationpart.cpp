/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include <kgenericfactory.h>

KexiMigrationPart::KexiMigrationPart(QObject *parent, const QStringList &args)
 : KexiInternalPart(parent, args)
{
}

KexiMigrationPart::~KexiMigrationPart()
{
}

QWidget *KexiMigrationPart::createWidget(const char* /*widgetClass*/, 
 QWidget *parent, const char *objName, QMap<QString,QString>* args )
{
  KexiMigration::ImportWizard *w = new KexiMigration::ImportWizard(parent, args);
  w->setName(objName);
  return w;
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_migration, 
  KGenericFactory<KexiMigrationPart>("kexihandler_migration") )
