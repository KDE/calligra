/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2008 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextPage.h"

#include <QString>

KoTextPage::KoTextPage() = default;

KoTextPage::~KoTextPage() = default;

QString KoTextPage::masterPageName() const
{
    return QString();
}
