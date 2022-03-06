/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_APPLYFILTER_COMMAND
#define CALLIGRA_SHEETS_APPLYFILTER_COMMAND

#include "AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{

class Database;
class DataFilter;

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

    void setDatabase(const Database& database);
    void setOldFilter(const DataFilter& filter);

private:
    Database m_database;
    DataFilter m_oldFilter;
    QHash<int, bool> m_undoData;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_APPLYFILTER_COMMAND
