/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Victor Lafon <metabolic.ewilan@hotmail.fr>
 * SPDX-FileCopyrightText: 2013 Sascha Suelzer <s.suelzer@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoResourceModelBase.h"

KoResourceModelBase::KoResourceModelBase(QObject *parent)
    : QAbstractTableModel(parent)
{
}

KoResourceModelBase::~KoResourceModelBase() = default;
