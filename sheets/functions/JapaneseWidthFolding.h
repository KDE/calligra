// This file is part of the KDE project
// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-only

#pragma once

#include <QString>

// Ported from LibreOffice's i18nutil::widthfolding (MPL-2.0).

namespace Calligra
{
namespace Sheets
{
namespace JapaneseWidthFolding
{

QString toHalfWidthForAsc(const QString &input);
QString toFullWidthForJis(const QString &input);

}
}
}
