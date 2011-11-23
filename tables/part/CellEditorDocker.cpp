/* This file is part of the KDE project
   Copyright 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>

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

#include "CellEditorDocker.h"

#include <KLocale>

using namespace Calligra::Tables;

CellEditorDocker::CellEditorDocker()
{
    setWindowTitle(i18n("Cell Editor"));
}

CellEditorDockerFactory::CellEditorDockerFactory()
{
}

QString CellEditorDockerFactory::id() const
{
    return QString::fromLatin1("CalligraTablesCellEditor");
}

QDockWidget* CellEditorDockerFactory::createDockWidget()
{
    CellEditorDocker* widget = new CellEditorDocker();
    widget->setObjectName(id());

    return widget;
}
