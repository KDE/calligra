/*
 *  SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef CHANGEANCHORPROPERTIESCOMMAND_H
#define CHANGEANCHORPROPERTIESCOMMAND_H

#include "KoShapeAnchor.h"
#include "kotext_export.h"
#include <kundo2command.h>

#include <QPointF>

class KoShapeContainer;

class KOTEXT_EXPORT ChangeAnchorPropertiesCommand : public KUndo2Command
{
public:
    ChangeAnchorPropertiesCommand(KoShapeAnchor *anchor, const KoShapeAnchor &newAnchorData, KoShapeContainer *newParent, KUndo2Command *parent);
    ~ChangeAnchorPropertiesCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    void copyLayoutProperties(const KoShapeAnchor *from, KoShapeAnchor *to);

    KoShapeAnchor *m_anchor;
    KoShapeAnchor m_oldAnchor;
    KoShapeAnchor m_newAnchor;
    KoShapeContainer *m_oldParent;
    KoShapeContainer *m_newParent;
    QPointF m_oldAbsPos;
    QPointF m_newAbsPos;
    KoShapeAnchor::TextLocation *m_oldLocation;
    KoShapeAnchor::TextLocation *m_newLocation;
    bool m_first;
    bool m_undone;
};

#endif // CHANGEANCHORPROPERTIESCOMMAND_H
