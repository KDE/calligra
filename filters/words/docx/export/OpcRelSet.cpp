/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "OpcRelSet.h"

// Qt
#include <QByteArray>
#include <QList>
#include <QString>

// This filter
#include "DocxExportDebug.h"

// ================================================================
//                         class OpcRelSet

OpcRelSet::OpcRelSet() = default;

OpcRelSet::~OpcRelSet() = default;

QString OpcRelSet::addRelationship(const QString &type, const QString &target)
{
    Q_UNUSED(type);
    Q_UNUSED(target);
    // FIXME: NYI
    return QString();
}
