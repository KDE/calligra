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

#ifndef _KARBONBOOLEANCOMMAND_H_
#define _KARBONBOOLEANCOMMAND_H_

#include <QtGui/QUndoCommand>

class KoShapeControllerBase;
class KoPathShape;
class QPainterPath;

class KarbonBooleanCommand : public QUndoCommand
{
public:
    enum BooleanOperation
    {
        Intersection,
        Subtraction,
        Union
    };

    /**
     * Command for doing a boolean operation on two given path shapes.
     * @param controller the controller to used for removing/inserting.
     * @param parent the parent command used for macro commands
     */
    KarbonBooleanCommand( KoShapeControllerBase *controller, QUndoCommand *parent = 0 );
    virtual ~KarbonBooleanCommand();
    /// Sets the first operand of the boolean operation
    void setFirstOperand( KoPathShape* pathA );
    /// Sets the second operand of the boolean operation
    void setSecondOperand( KoPathShape * pathB );
    /// Sets the type of the booelan operation to execute
    void setOperation( BooleanOperation operation );
    /// redo the command
    void redo();
    /// revert the actions done in redo
    void undo();
private:
    KoPathShape * shapeFromPath( const QPainterPath &path );
    class Private;
    Private * const d;
};

#endif // _KARBONBOOLEANCOMMAND_H_
