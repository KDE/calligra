/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "ChangeVectorDataCommand.h"

#include <KLocalizedString>
#include <KoImageData.h>
#include <math.h>

#include "VectorShape.h"

ChangeVectorDataCommand::ChangeVectorDataCommand(VectorShape *shape,
                                                 const QByteArray &newImageData,
                                                 VectorShape::VectorType newVectorType,
                                                 KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_shape(shape)
{
    Q_ASSERT(shape);
    m_oldImageData = m_shape->compressedContents();
    m_oldVectorType = m_shape->vectorType();
    m_newImageData = newImageData;
    m_newVectorType = newVectorType;
    setText(kundo2_i18n("Change Vector Data"));
}

ChangeVectorDataCommand::~ChangeVectorDataCommand() = default;

void ChangeVectorDataCommand::redo()
{
    m_shape->update();
    m_shape->setCompressedContents(m_newImageData, m_newVectorType);
    m_shape->update();
}

void ChangeVectorDataCommand::undo()
{
    m_shape->update();
    m_shape->setCompressedContents(m_oldImageData, m_oldVectorType);
    m_shape->update();
}
