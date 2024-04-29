/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef CHANGEVECTORDATACOMMAND_H
#define CHANGEVECTORDATACOMMAND_H

#include <QByteArray>
#include <kundo2command.h>

#include "VectorShape.h"

class ChangeVectorDataCommand : public KUndo2Command
{
public:
    ChangeVectorDataCommand(VectorShape *shape, const QByteArray &newImageData, VectorShape::VectorType newVectorType, KUndo2Command *parent = nullptr);
    ~ChangeVectorDataCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    VectorShape *m_shape;
    QByteArray m_oldImageData;
    VectorShape::VectorType m_oldVectorType;
    QByteArray m_newImageData;
    VectorShape::VectorType m_newVectorType;
};

#endif /* CHANGEVECTORDATACOMMAND_H */
