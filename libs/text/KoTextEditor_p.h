/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
 * SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2015 Soma Schliszka <soma.schliszka@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTEDITOR_P_H
#define KOTEXTEDITOR_P_H

#include "KoTextEditor.h"

#include "KoTextDocument.h"
#include "changetracker/KoChangeTracker.h"
#include "styles/KoParagraphStyle.h"
#include "styles/KoStyleManager.h"

#include <KLocalizedString>
#include <kundo2magicstring.h>

#include <QStack>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextTableCell>
#include <QTimer>

class KUndo2Command;

class Q_DECL_HIDDEN KoTextEditor::Private
{
public:
    enum State { NoOp, KeyPress, Delete, Format, Custom };

    explicit Private(KoTextEditor *qq, QTextDocument *document);

    ~Private() = default;

    void documentCommandAdded();
    void updateState(State newState, const KUndo2MagicString &title = KUndo2MagicString());

    void newLine(KUndo2Command *parent);
    void clearCharFormatProperty(int propertyId);

    void emitTextFormatChanged();

    KoTextEditor *q;
    QTextCursor caret;
    QTextDocument *document;
    QStack<KUndo2Command *> commandStack;
    bool addNewCommand;
    bool dummyMacroAdded;
    int customCommandCount;
    KUndo2MagicString commandTitle;

    State editorState;

    bool editProtected;
    bool editProtectionCached;
};

class KoTextVisitor
{
public:
    /// The ObjectVisitingMode enum marks how was the visited object selected.
    enum ObjectVisitingMode {
        Partly, /// The visited object (table, cell, ...) is just @b partly selected. (Eg. just one cell is selected in the visited table)
        Entirely, /// The visited object (table, cell, ...) is @b entirely selected.
    };

    explicit KoTextVisitor(KoTextEditor *editor)
        : m_abortVisiting(false)
        , m_editor(editor)
    {
    }

    virtual ~KoTextVisitor() = default;
    // called whenever a visit was prevented by editprotection
    virtual void nonVisit()
    {
    }

    virtual void visitFragmentSelection(QTextCursor &)
    {
    }

    /**
     * This method allows to perform custom operation when the visitor reaches a QTextTable
     * @param visitedTable pointer to the currently visited table object
     * @param visitingMode flag, marks if the table is just partly visited or entirely
     */
    virtual void visitTable(QTextTable *visitedTable, ObjectVisitingMode visitingMode)
    {
        Q_UNUSED(visitedTable);
        Q_UNUSED(visitingMode);
    }

    /**
     * This method allows to perform custom operation when the visitor reaches a QTextTableCell
     * @param visitedCell pointer to the currently visited cell object
     * @param visitingMode flag, marks if the cell is just partly visited or entirely
     */
    virtual void visitTableCell(QTextTableCell *visitedCell, ObjectVisitingMode visitingMode)
    {
        Q_UNUSED(visitedCell);
        Q_UNUSED(visitingMode);
    }

    // The default implementation calls visitFragmentSelection on each fragment.intersect.selection
    virtual void visitBlock(QTextBlock &block, const QTextCursor &caret)
    {
        for (QTextBlock::iterator it = block.begin(); it != block.end(); ++it) {
            QTextCursor fragmentSelection(caret);
            fragmentSelection.setPosition(qMax(caret.selectionStart(), it.fragment().position()));
            fragmentSelection.setPosition(qMin(caret.selectionEnd(), it.fragment().position() + it.fragment().length()), QTextCursor::KeepAnchor);

            if (fragmentSelection.anchor() >= fragmentSelection.position()) {
                continue;
            }

            visitFragmentSelection(fragmentSelection);
        }
    }

    bool abortVisiting()
    {
        return m_abortVisiting;
    }
    void setAbortVisiting(bool abort)
    {
        m_abortVisiting = abort;
    }
    KoTextEditor *editor() const
    {
        return m_editor;
    }

private:
    bool m_abortVisiting;
    KoTextEditor *m_editor;
};

class BlockFormatVisitor
{
public:
    BlockFormatVisitor() = default;
    virtual ~BlockFormatVisitor() = default;

    virtual void visit(QTextBlock &block) const = 0;

    static void visitSelection(KoTextEditor *editor,
                               const BlockFormatVisitor &visitor,
                               const KUndo2MagicString &title = kundo2_i18n("Format"),
                               bool resetProperties = false,
                               bool registerChange = true)
    {
        int start = qMin(editor->position(), editor->anchor());
        int end = qMax(editor->position(), editor->anchor());

        QTextBlock block = editor->block();
        if (block.position() > start)
            block = block.document()->findBlock(start);

        // now loop over all blocks that the selection contains and alter the text fragments where applicable.
        while (block.isValid() && block.position() <= end) {
            QTextBlockFormat prevFormat = block.blockFormat();
            if (resetProperties) {
                if (KoTextDocument(editor->document()).styleManager()) {
                    KoParagraphStyle *old =
                        KoTextDocument(editor->document()).styleManager()->paragraphStyle(block.blockFormat().intProperty(KoParagraphStyle::StyleId));
                    if (old)
                        old->unapplyStyle(block);
                }
            }
            visitor.visit(block);
            QTextCursor cursor(block);
            QTextBlockFormat format = cursor.blockFormat();
            if (registerChange)
                editor->registerTrackedChange(cursor, KoGenChange::FormatChange, title, format, prevFormat, true);
            block = block.next();
        }
    }
};

class CharFormatVisitor
{
public:
    CharFormatVisitor() = default;
    virtual ~CharFormatVisitor() = default;

    virtual void visit(QTextCharFormat &format) const = 0;

    static void
    visitSelection(KoTextEditor *editor, const CharFormatVisitor &visitor, const KUndo2MagicString &title = kundo2_i18n("Format"), bool registerChange = true)
    {
        int start = qMin(editor->position(), editor->anchor());
        int end = qMax(editor->position(), editor->anchor());
        if (start == end) { // just set a new one.
            QTextCharFormat format = editor->charFormat();
            visitor.visit(format);

            if (registerChange && KoTextDocument(editor->document()).changeTracker() && KoTextDocument(editor->document()).changeTracker()->recordChanges()) {
                QTextCharFormat prevFormat(editor->charFormat());

                int changeId = KoTextDocument(editor->document())
                                   .changeTracker()
                                   ->getFormatChangeId(title, format, prevFormat, editor->charFormat().property(KoCharacterStyle::ChangeTrackerId).toInt());
                format.setProperty(KoCharacterStyle::ChangeTrackerId, changeId);
            }

            editor->cursor()->setCharFormat(format);
            return;
        }

        QTextBlock block = editor->block();
        if (block.position() > start)
            block = block.document()->findBlock(start);

        QList<QTextCursor> cursors;
        QVector<QTextCharFormat> formats;
        // now loop over all blocks that the selection contains and alter the text fragments where applicable.
        while (block.isValid() && block.position() < end) {
            QTextBlock::iterator iter = block.begin();
            while (!iter.atEnd()) {
                QTextFragment fragment = iter.fragment();
                if (fragment.position() > end)
                    break;
                if (fragment.position() + fragment.length() <= start) {
                    ++iter;
                    continue;
                }

                QTextCursor cursor(block);
                cursor.setPosition(fragment.position() + 1);
                QTextCharFormat format = cursor.charFormat(); // this gets the format one char after the position.
                visitor.visit(format);

                if (registerChange && KoTextDocument(editor->document()).changeTracker()
                    && KoTextDocument(editor->document()).changeTracker()->recordChanges()) {
                    QTextCharFormat prevFormat(cursor.charFormat());

                    int changeId = KoTextDocument(editor->document())
                                       .changeTracker()
                                       ->getFormatChangeId(title, format, prevFormat, cursor.charFormat().property(KoCharacterStyle::ChangeTrackerId).toInt());
                    format.setProperty(KoCharacterStyle::ChangeTrackerId, changeId);
                }

                cursor.setPosition(qMax(start, fragment.position()));
                int to = qMin(end, fragment.position() + fragment.length());
                cursor.setPosition(to, QTextCursor::KeepAnchor);
                cursors.append(cursor);
                formats.append(format);

                QTextCharFormat prevFormat(cursor.charFormat());
                if (registerChange)
                    editor->registerTrackedChange(cursor,
                                                  KoGenChange::FormatChange,
                                                  title,
                                                  format,
                                                  prevFormat,
                                                  false); // this will lead to every fragment having a different change until the change merging in
                                                          // registerTrackedChange checks also for formatChange or not?

                ++iter;
            }
            block = block.next();
        }
        QVector<QTextCharFormat>::Iterator iter = formats.begin();
        foreach (QTextCursor cursor, cursors) {
            cursor.setCharFormat(*iter);
            ++iter;
        }
    }
};

#endif // KOTEXTEDITOR_P_H
