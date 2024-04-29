/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _KARBONBOOLEANCOMMAND_H_
#define _KARBONBOOLEANCOMMAND_H_

#include <karboncommon_export.h>
#include <kundo2command.h>

class KoShapeBasedDocumentBase;
class KoPathShape;

class KARBONCOMMON_EXPORT KarbonBooleanCommand : public KUndo2Command
{
public:
    enum BooleanOperation {
        Intersection, ///< the intersection of A and B
        Subtraction, ///< the subtraction A - B
        Union, ///< the union A + B
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
    explicit KarbonBooleanCommand(KoShapeBasedDocumentBase *controller,
                                  KoPathShape *pathA,
                                  KoPathShape *pathB,
                                  BooleanOperation operation,
                                  KUndo2Command *parent = nullptr);
    ~KarbonBooleanCommand() override;
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    class Private;
    Private *const d;
};

#endif // _KARBONBOOLEANCOMMAND_H_
