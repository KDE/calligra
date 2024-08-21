/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "AbstractStylesModel.h"

AbstractStylesModel::AbstractStylesModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_styleThumbnailer(nullptr)
{
}
