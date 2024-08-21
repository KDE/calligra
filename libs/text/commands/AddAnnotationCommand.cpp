// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Ganesh Paramasivam <ganesh@crystalfab.com>
// SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
// SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
// SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "AddAnnotationCommand.h"

#include <KoAnnotation.h>
#include <KoShapeBasedDocumentBase.h>
#include <KoShapeController.h>
#include <KoTextDocument.h>

AddAnnotationCommand::AddAnnotationCommand(KoAnnotation *annotation, KUndo2Command *parent)
    : AddTextRangeCommand(annotation, parent)
    , m_annotation(annotation)
    , m_shape(nullptr)
{
    setText(kundo2_noi18n("internal step"));
}

void AddAnnotationCommand::undo()
{
    AddTextRangeCommand::undo();
    KoShapeController *shapeController = KoTextDocument(m_annotation->document()).shapeController();
    m_shape = m_annotation->annotationShape();
    shapeController->documentBase()->removeShape(m_shape);
}

void AddAnnotationCommand::redo()
{
    AddTextRangeCommand::redo();

    KoShapeController *shapeController = KoTextDocument(m_annotation->document()).shapeController();
    shapeController->documentBase()->addShape(m_annotation->annotationShape());

    m_shape = nullptr;

    // it's a textrange so we need to ask for a layout so we know where it is
    m_annotation->document()->markContentsDirty(m_annotation->rangeStart(), 0);
}

AddAnnotationCommand::~AddAnnotationCommand()
{
    // We delete shape at KoShapeDeleteCommand.
    // delete m_annotation->annotationShape();
}
