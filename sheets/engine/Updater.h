// This file is part of the KDE project
// SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_UPDATER_H
#define CALLIGRA_SHEETS_UPDATER_H

#include "sheets_engine_export.h"

namespace Calligra
{
namespace Sheets
{

/**
 * A callback for task progress updates. Sheets has a child class that couples this with KoUpdater, the purpose of this class is so we can use this
 * functionality without pulling in kolibs/UI dependencies.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT Updater
{
public:
    Updater() = default;
    virtual ~Updater() = default;

    virtual void setProgress(int percent) = 0;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_UPDATER_H
