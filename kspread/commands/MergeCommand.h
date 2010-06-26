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

#ifndef KSPREAD_MERGE_COMMAND
#define KSPREAD_MERGE_COMMAND

#include "AbstractRegionCommand.h"

namespace KSpread
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
    MergeCommand(QUndoCommand* parent = 0);
    virtual ~MergeCommand();

    virtual bool preProcessing();

    virtual void setReverse(bool reverse) {
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
    virtual bool process(Element*);

    virtual bool postProcessing();

    QString name() const;

    bool m_merge;
private:
    bool m_mergeHorizontal : 1;
    bool m_mergeVertical   : 1;
    AbstractRegionCommand* m_unmerger; // to restore old merging
    Selection *m_selection;
};

} // namespace KSpread

#endif // KSPREAD_MERGE_COMMAND
