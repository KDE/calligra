// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Ganesh Paramasivam <ganesh@crystalfab.com>
// SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "ShowChangesCommand.h"
#include <iostream>

#include <KoCanvasBase.h>
#include <KoChangeTracker.h>
#include <KoChangeTrackerElement.h>
#include <KoCharacterStyle.h>
#include <KoInlineTextObjectManager.h>
#include <KoShapeAnchor.h>
#include <KoShapeContainer.h>
#include <KoShapeController.h>
#include <KoTextDocument.h>
#include <KoTextDocumentLayout.h>
#include <KoTextEditor.h>

#include <KLocalizedString>

#include <QList>
#include <QTextDocument>
#include <QtAlgorithms>

#include <algorithm>

ShowChangesCommand::ShowChangesCommand(bool showChanges, QTextDocument *document, KoCanvasBase *canvas, KUndo2Command *parent)
    : KoTextCommandBase(parent)
    , m_document(document)
    , m_first(true)
    , m_showChanges(showChanges)
    , m_canvas(canvas)
{
    Q_ASSERT(document);
    m_changeTracker = KoTextDocument(m_document).changeTracker();
    m_textEditor = KoTextDocument(m_document).textEditor();
    if (showChanges)
        setText(kundo2_i18n("Show Changes"));
    else
        setText(kundo2_i18n("Hide Changes"));
}

void ShowChangesCommand::undo()
{
    KoTextCommandBase::undo();
    UndoRedoFinalizer finalizer(this);
    foreach (KUndo2Command *shapeCommand, m_shapeCommands)
        shapeCommand->undo();
    Q_EMIT toggledShowChange(!m_showChanges);
    enableDisableStates(!m_showChanges);
}

void ShowChangesCommand::redo()
{
    if (!m_first) {
        KoTextCommandBase::redo();
        UndoRedoFinalizer finalizer(this);
        foreach (KUndo2Command *shapeCommand, m_shapeCommands)
            shapeCommand->redo();
        Q_EMIT toggledShowChange(m_showChanges);
        enableDisableStates(m_showChanges);
    } else {
        m_first = false;
        enableDisableChanges();
    }
}

void ShowChangesCommand::enableDisableChanges()
{
    if (m_changeTracker) {
        enableDisableStates(m_showChanges);

        if (m_showChanges)
            insertDeletedChanges();
        else
            removeDeletedChanges();
#if 0
TODO
        KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout*>(m_document->documentLayout());
        if (lay)
          lay->scheduleLayout();
#endif
    }
}

void ShowChangesCommand::enableDisableStates(bool showChanges)
{
    m_changeTracker->setDisplayChanges(showChanges);

    QTextCharFormat format = m_textEditor->charFormat();
    format.clearProperty(KoCharacterStyle::ChangeTrackerId);
    m_textEditor->setCharFormat(format);
}

void ShowChangesCommand::insertDeletedChanges()
{
    QVector<KoChangeTrackerElement *> elementVector;
    KoTextDocument(m_textEditor->document()).changeTracker()->getDeletedChanges(elementVector);
    std::sort(elementVector.begin(), elementVector.end());
}

void ShowChangesCommand::checkAndAddAnchoredShapes(int position, int length)
{
    KoInlineTextObjectManager *inlineObjectManager = KoTextDocument(m_document).inlineTextObjectManager();
    Q_ASSERT(inlineObjectManager);

    QTextCursor cursor = m_textEditor->document()->find(QString(QChar::ObjectReplacementCharacter), position);
    while (!cursor.isNull() && cursor.position() < position + length) {
        QTextCharFormat fmt = cursor.charFormat();
        KoInlineObject *object = inlineObjectManager->inlineTextObject(fmt);
        Q_ASSERT(object);
        Q_UNUSED(object);
/* FIXME
        KoTextAnchor *anchor = dynamic_cast<KoTextAnchor *>(object);
        if (!anchor) {
            continue;
        }
        */
#if 0
        // TODO -- since March 2010...
        KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout*>(m_document->documentLayout());

        KoShapeContainer *container = dynamic_cast<KoShapeContainer *>(lay->shapeForPosition(i));

        // a very ugly hack. Since this class is going away soon, it should be okay
        if (!container)
            container = dynamic_cast<KoShapeContainer *>((lay->shapes()).at(0));

        if (container) {
            container->addShape(anchor->shape());
            KUndo2Command *shapeCommand = m_canvas->shapeController()->addShapeDirect(anchor->shape());
            shapeCommand->redo();
            m_shapeCommands.push_front(shapeCommand);
        }
#endif
        cursor = m_textEditor->document()->find(QString(QChar::ObjectReplacementCharacter), position);
    }
}

void ShowChangesCommand::removeDeletedChanges()
{
    QVector<KoChangeTrackerElement *> elementVector;
    m_changeTracker->getDeletedChanges(elementVector);
    std::sort(elementVector.begin(), elementVector.end());
}

void ShowChangesCommand::checkAndRemoveAnchoredShapes(int position, int length)
{
    KoInlineTextObjectManager *inlineObjectManager = KoTextDocument(m_document).inlineTextObjectManager();
    Q_ASSERT(inlineObjectManager);

    QTextCursor cursor = m_textEditor->document()->find(QString(QChar::ObjectReplacementCharacter), position);
    while (!cursor.isNull() && cursor.position() < position + length) {
        QTextCharFormat fmt = cursor.charFormat();
        KoInlineObject *object = inlineObjectManager->inlineTextObject(fmt);
        Q_ASSERT(object);
        Q_UNUSED(object);
        /* FIXME
        KoTextAnchor *anchor = dynamic_cast<KoTextAnchor *>(object);
        if (!anchor)
            continue;

        KUndo2Command *shapeCommand = m_canvas->shapeController()->removeShape(anchor->shape());
        shapeCommand->redo();
        m_shapeCommands.push_front(shapeCommand);
        */
    }
}

ShowChangesCommand::~ShowChangesCommand() = default;
