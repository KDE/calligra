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

#ifndef KARBONWHIRLPINCHCOMMAND_H
#define KARBONWHIRLPINCHCOMMAND_H

#include <QtGui/QUndoCommand>

class KoPathShape;

/// The undo / redo command for the whirl pinch effect on a given path
class KarbonWhirlPinchCommand : public QUndoCommand
{
public:
    /**
    * Command for whirl pinch effect on a path.
    *
    * @param path the path to flatten
    * @param angle the whirl angle
    * @param pinch the pinch amount between -1 and 1
    * @param radius the effect radius
    * @param parent the parent command used for macro commands
     */
    KarbonWhirlPinchCommand(KoPathShape * path, qreal angle, qreal pinch, qreal radius, QUndoCommand *parent = 0);
    virtual ~KarbonWhirlPinchCommand();

    /// redo the command
    void redo();
    /// revert the actions done in redo
    void undo();

private:
    class Private;
    Private * const d;
};

#endif // KARBONWHIRLPINCHCOMMAND_H
