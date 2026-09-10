/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QObject>
#include <QVariantList>

class Plugin final : public QObject
{
    Q_OBJECT

public:
    Plugin(QObject *parent, const QVariantList &args);
};
