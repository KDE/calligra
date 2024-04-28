/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_APPLYFILTER_COMMAND
#define CALLIGRA_SHEETS_APPLYFILTER_COMMAND

#include "AbstractRegionCommand.h"
#include "core/DataFilter.h"
#include "core/Database.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Commands
 * \brief Filtering a cell range.
 */
class ApplyFilterCommand : public AbstractRegionCommand
{
public:
    /**
     * Constructor.
     */
    ApplyFilterCommand();

    /**
     * Destructor.
     */
    ~ApplyFilterCommand() override;

    void redo() override;
    void undo() override;

    void setDatabase(const Database &database);
    void setOldFilter(const Filter &filter);

private:
    Database m_database;
    Filter m_oldFilter;
    QHash<int, bool> m_undoData;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_APPLYFILTER_COMMAND
