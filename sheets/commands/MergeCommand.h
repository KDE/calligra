/* This file is part of the KDE project
   Copyright 2005,2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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
    explicit MergeCommand(KUndo2Command *parent = 0);
    ~MergeCommand() override;

    bool preProcessing() override;

    void setReverse(bool reverse) override {
        m_merge = !reverse;
    }
    void setHorizontalMerge(bool state) {
        m_mergeHorizontal = state;
    }
    void setVerticalMerge(bool state) {
        m_mergeVertical = state;
    }

    void setSelection(Selection *selection) {
        m_selection = selection;
    }
protected:
    bool process(Element*) override;

    bool postProcessing() override;

    KUndo2MagicString name() const;

    bool m_merge;
private:
    bool m_mergeHorizontal : 1;
    bool m_mergeVertical   : 1;
    AbstractRegionCommand* m_unmerger; // to restore old merging
    Selection *m_selection;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_MERGE_COMMAND
