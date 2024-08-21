/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef FILTERREGIONCHANGECOMMAND_H
#define FILTERREGIONCHANGECOMMAND_H

#include <QRectF>
#include <kundo2command.h>

class KoShape;
class KoFilterEffect;

/// A command to change the region of a filter effect
class FilterRegionChangeCommand : public KUndo2Command
{
public:
    /**
     * Creates new command to change filter region of a filter effect
     * @param effect the effect to change the filter region of
     * @param filterRegion the new filter region to set
     * @param shape the shape the filter effect is applied to
     * @param parent the parent undo command
     */
    explicit FilterRegionChangeCommand(KoFilterEffect *effect, const QRectF &filterRegion, KoShape *shape = nullptr, KUndo2Command *parent = nullptr);

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoFilterEffect *m_effect; ///< the filter effect we are working on
    QRectF m_oldRegion; ///< the old filter region
    QRectF m_newRegion; ///< the new filter region
    KoShape *m_shape; ///< the shape the effect is applied to, might be zero
};

#endif // FILTERREGIONCHANGECOMMAND_H
