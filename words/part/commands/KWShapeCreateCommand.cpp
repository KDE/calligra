/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
 * Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>
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

#include "KWShapeCreateCommand.h"

#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoShapeAnchor.h>
#include <KWDocument.h>

// class KoShapeCreateCommand::Private
// {
// public:
//     Private(KWDocument *c, KoShape *s):
//         document(c),
//         shape(s),
//         shapeParent(shape->parent()),
//         deleteShape(true) {
//     }
//     
//     ~Private() {
//         if (deleteShape)
//             delete shape;
//     }
// 
//     KWDocument *document;
//     KoShape *shape;
//     KoShapeContainer *shapeParent;
//     bool deleteShape;
// };

KWShapeCreateCommand::KWShapeCreateCommand(KWDocument *doc, KoShape *shape, KUndo2Command *parent):
    KUndo2Command(parent),
    m_document(doc),
    m_shape(shape),
    m_deleteShape(true)
{
    setText(kundo2_i18n("Create shape"));
}

KWShapeCreateCommand::~KWShapeCreateCommand()
{
    if (m_deleteShape) {
        delete m_shape;
    }
}

void KWShapeCreateCommand::redo()
{
    KUndo2Command::redo();
    
    if (m_shape->parent())
        m_shape->parent()->addShape(m_shape);
    // the parent has to be there when it is added to the KoShapeBasedDocumentBase
    m_document->addShape(m_shape);
    m_shape->setParent(m_shape->parent()); // update parent if the 'addShape' changed it
    m_deleteShape = false;
}

void KWShapeCreateCommand::undo()
{
    KUndo2Command::undo();
    
    // the parent has to be there when it is removed from the KoShapeBasedDocumentBase
    m_document->removeShape(m_shape);
    
    if (m_shape->parent())
        m_shape->parent()->removeShape(m_shape);
    
    m_deleteShape = true;
}
