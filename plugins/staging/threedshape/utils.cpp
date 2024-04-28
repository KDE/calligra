/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "utils.h"

// Qt
#include <QString>
#include <QStringList>

QVector3D odfToVector3D(QString &string)
{
    // The string comes into this function in the form "(0 3.5 0.3)".
    QStringList elements = string.mid(1, string.size() - 2).split(' ', Qt::SkipEmptyParts);
    if (elements.size() != 3)
        return QVector3D(0, 0, 1);
    else
        return QVector3D(elements[0].toDouble(), elements[1].toDouble(), elements[2].toDouble());
}
