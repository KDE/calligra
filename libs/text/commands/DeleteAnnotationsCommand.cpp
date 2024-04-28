/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "DeleteAnnotationsCommand.h"

#include "KoAnnotation.h"
#include "KoTextDocument.h"
#include "KoTextRangeManager.h"

#include "TextDebug.h"

DeleteAnnotationsCommand::DeleteAnnotationsCommand(const QList<KoAnnotation *> &annotations, QTextDocument *document, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_annotations(annotations)
    , m_document(document)
    , m_deleteAnnotations(false)
{
}

DeleteAnnotationsCommand::~DeleteAnnotationsCommand()
{
    if (m_deleteAnnotations) {
        qDeleteAll(m_annotations);
    }
}

void DeleteAnnotationsCommand::redo()
{
    KUndo2Command::redo();
    m_deleteAnnotations = true;
    KoTextRangeManager *rangeManager = KoTextDocument(m_document).textRangeManager();
    if (rangeManager) {
        foreach (KoAnnotation *annotation, m_annotations) {
            rangeManager->remove(annotation);
        }
    }
}

void DeleteAnnotationsCommand::undo()
{
    KUndo2Command::undo();
    KoTextRangeManager *rangeManager = KoTextDocument(m_document).textRangeManager();
    if (rangeManager) {
        foreach (KoAnnotation *annotation, m_annotations) {
            rangeManager->insert(annotation);
            // it's a textrange so we need to ask for a layout so we know where it is
            m_document->markContentsDirty(annotation->rangeStart(), 0);
        }
    }

    m_deleteAnnotations = false;
}
