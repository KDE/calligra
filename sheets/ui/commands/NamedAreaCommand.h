/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_NAMED_AREA_COMMAND
#define CALLIGRA_SHEETS_NAMED_AREA_COMMAND

#include "AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Commands
 * \brief Attaches a name to a cell range.
 */
class NamedAreaCommand : public AbstractRegionCommand
{
public:
    /**
     * Constructor.
     */
    explicit NamedAreaCommand(KUndo2Command *parent = nullptr);

    /**
     * Destructor.
     */
    ~NamedAreaCommand() override;

    void setAreaName(const QString &name);
    void setNewAreaName(const QString &name);
    void setRemove(bool remove);

    bool preProcess() override;

protected:
    bool performNonCommandActions() override;
    bool undoNonCommandActions() override;

private:
    QString m_areaName;
    QString m_newAreaName;
    Region m_oldArea;
    bool m_remove : 1;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_NAMED_AREA_COMMAND
