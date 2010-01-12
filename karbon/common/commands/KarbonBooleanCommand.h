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

#include <karboncommon_export.h>
#include <QtGui/QUndoCommand>

class KoShapeControllerBase;
class KoPathShape;
class QPainterPath;

class KARBONCOMMON_EXPORT KarbonBooleanCommand : public QUndoCommand
{
public:
    enum BooleanOperation {
        Intersection, ///< the intersection of A and B
        Subtraction,  ///< the subtraction A - B
        Union,        ///< the union A + B
        Exclusion
    };

    /**
     * Command for doing a boolean operation on two given path shapes.
     * @param controller the controller to used for removing/inserting.
     * @param pathA the first operand
     * @param pathB the second operand
     * @param operation the booelan operation to execute
     * @param parent the parent command used for macro commands
     */
    explicit KarbonBooleanCommand(KoShapeControllerBase *controller, KoPathShape* pathA, KoPathShape * pathB,
                                  BooleanOperation operation, QUndoCommand *parent = 0);
    virtual ~KarbonBooleanCommand();
    /// redo the command
    void redo();
    /// revert the actions done in redo
    void undo();
private:
    class Private;
    Private * const d;
};

#endif // _KARBONBOOLEANCOMMAND_H_
