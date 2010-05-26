/*
* This file is part of the KDE project
*
* Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
*
* Contact: Amit Aggarwal <amitcs06@gmail.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
* 02110-1301 USA
*
*/
#include "Plugin.h"
#include <kgenericfactory.h>
#include <KoInlineObjectRegistry.h>
#include "PresentationVariableFactory.h"


K_EXPORT_COMPONENT_FACTORY(kprvariables, KGenericFactory<Plugin>("kprvariables"))

Plugin::Plugin(QObject *parent, const QStringList &)
    : QObject(parent)
{
    KoInlineObjectRegistry::instance()->add(new PresentationVariableFactory());
}

#include "Plugin.moc"

