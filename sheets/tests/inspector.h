// This file is part of the KDE project
// SPDX-FileCopyrightText: 2005 Ariya Hidayat <ariya@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_INSPECTOR
#define CALLIGRA_SHEETS_INSPECTOR

#include <kpagedialog.h>

namespace Calligra
{
namespace Sheets
{
class Cell;

class Inspector : public KPageDialog
{
    Q_OBJECT
public:
    Inspector(const Cell& cell);
    ~Inspector() override;

private:
    Q_DISABLE_COPY(Inspector)

    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_INSPECTOR
