/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_MERGE_COMMAND
#define CALLIGRA_SHEETS_MERGE_COMMAND

#include "AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{

class Selection;

/**
 * \class MergeCommand
 * \ingroup Commands
 * \brief Merges and splits the cells of a cell region.
 */
class MergeCommand : public AbstractRegionCommand
{
public:
    explicit MergeCommand(KUndo2Command *parent = nullptr);
    ~MergeCommand() override;

    void setMerge(bool merge)
    {
        m_merge = merge;
    }
    void setHorizontalMerge(bool state)
    {
        m_mergeHorizontal = state;
    }
    void setVerticalMerge(bool state)
    {
        m_mergeVertical = state;
    }

    void setSelection(Selection *selection)
    {
        m_selection = selection;
    }

protected:
    bool preProcess() override;
    bool process(Element *) override;

    KUndo2MagicString name() const;

    bool m_merge;

private:
    bool m_mergeHorizontal : 1;
    bool m_mergeVertical : 1;
    Selection *m_selection;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_MERGE_COMMAND
