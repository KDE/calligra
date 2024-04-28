/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWShapeCreateCommand.h"

#include <KWDocument.h>
#include <KoShape.h>
#include <KoShapeAnchor.h>
#include <KoShapeContainer.h>

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

KWShapeCreateCommand::KWShapeCreateCommand(KWDocument *doc, KoShape *shape, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_document(doc)
    , m_shape(shape)
    , m_deleteShape(true)
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
