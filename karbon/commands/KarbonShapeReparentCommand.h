/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KARBONSHAPEREPARENTCOMMAND_H
#define KARBONSHAPEREPARENTCOMMAND_H

#include <flake_export.h>

#include <QList>
#include <QUndoCommand>

class KoShape;
class KoShapeContainer;

/// The undo / redo command for changing parent of shapes
class FLAKE_EXPORT KarbonShapeReparentCommand : public QUndoCommand {
public:
    /**
     * Command to change parents on a set of shapes.
     * @param shapes a list of all the shapes that should be reparented
     * @param oldParents a list of the old shape parents
     * @param newParents a list of the new shape parents
     * @param parent the parent command used for macro commands
     */
    KarbonShapeReparentCommand( QList<KoShape*> shapes, QList<KoShapeContainer*> oldParents,
                            QList<KoShapeContainer*> newParents, QUndoCommand *parent = 0);

    virtual ~KarbonShapeReparentCommand();
    /// redo the command
    virtual void redo();
    /// revert the actions done in redo
    virtual void undo();

protected:
    QList<KoShape*> m_shapes; ///< list of shapes to be reparented
    QList<KoShapeContainer*> m_oldParents; ///< the old parents of the shapes
    QList<KoShapeContainer*> m_newParents; ///< the new parents of the shapes
};

#endif // KARBONSHAPEREPARENTCOMMAND_H
