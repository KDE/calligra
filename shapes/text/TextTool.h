/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
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

#ifndef KOTEXTTOOL_H
#define KOTEXTTOOL_H

#include "TextShape.h"

#include <KoTextSelectionHandler.h>
#include <KoTool.h>

#include <QTextCursor>
#include <QAction>
#include <QHash>

class KoStyleManager;
class KoTextEditingPlugin;
class UndoTextCommand;
class ChangeTracker;

/**
 * This is the tool for the text-shape (which is a flake-based plugin).
 */
class TextTool : public KoTool {
    Q_OBJECT
public:
    explicit TextTool(KoCanvasBase *canvas);
    ~TextTool();

    /// reimplemented from superclass
    virtual void paint( QPainter &painter, KoViewConverter &converter );

    /// reimplemented from superclass
    virtual void mousePressEvent( KoPointerEvent *event ) ;
    /// reimplemented from superclass
    virtual void mouseDoubleClickEvent( KoPointerEvent *event );
    /// reimplemented from superclass
    virtual void mouseMoveEvent( KoPointerEvent *event );
    /// reimplemented from superclass
    virtual void mouseReleaseEvent( KoPointerEvent *event );
    /// reimplemented from superclass
    virtual void keyPressEvent(QKeyEvent *event);
    /// reimplemented from superclass
    virtual void keyReleaseEvent(QKeyEvent *event);

    /// reimplemented from superclass
    virtual void activate (bool temporary=false);
    /// reimplemented from superclass
    virtual void deactivate();
    /// reimplemented from superclass
    virtual void copy();

    /// reimplemented from superclass
    virtual KoToolSelection* selection();
    /// reimplemented from superclass
    virtual QWidget *createOptionWidget();

signals:
    /// emitted every time a different styleManager is set.
    void styleManagerChanged(KoStyleManager *manager);
    /// emitted every time a caret move leads to a different character format being under the caret
    void charFormatChanged(const QTextCharFormat &format);
    /// emitted every time a caret move leads to a different paragraph format being under the caret
    void blockFormatChanged(const QTextBlockFormat &format);

private slots:
    /// insert a non breaking space at the caret position
    void nonbreakingSpace();
    /// insert a non breaking hyphen at the caret position
    void nonbreakingHyphen();
    /// insert a soft hyphen at the caret position
    void softHyphen();
    /// insert a linebreak at the caret position
    void lineBreak();
    /// align all of the selected text left
    void alignLeft();
    /// align all of the selected text right
    void alignRight();
    /// align all of the selected text centered
    void alignCenter();
    /// align all of the selected text block-justified
    void alignBlock();
    /// make the selected text switch to be super-script
    void superScript(bool);
    /// make the selected text switch to be sub-script
    void subScript(bool);
    /// move the paragraph indent of the selected text to be less (left on LtR text)
    void decreaseIndent();
    /// move the paragraph indent of the selected text to be more (right on LtR text)
    void increaseIndent();
    /// Default Format
    void textDefaultFormat();
    /// see KoTextSelectionHandler::insertIndexMarker
    void insertIndexMarker();
    /// shows a dialog to alter the paragraph properties
    void formatParagraph();
    /// When enabled, make the change tracker record changes made while typing
    void toggleTrackChanges(bool);

    /// add a KoDocument wide undo command which will call undo on the qtextdocument.
    void addUndoCommand();

private:
    void repaintCaret();
    void repaintSelection(int from, int to);
    void ensureCursorVisible();
    QRectF textRect(int startPosition, int endPosition);
    int pointToPosition(const QPointF & point) const;
    void updateSelectionHandler();
    void updateActions();
    void updateStyleManager();
    void setShapeData(KoTextShapeData *data);

    void editingPluginEvents();
    void finishedWord();
    void finishedParagraph();

private:
    friend class UndoTextCommand;
    friend class ChangeTracker;
    TextShape *m_textShape;
    KoTextShapeData *m_textShapeData;
    QTextCursor m_caret;
    ChangeTracker *m_changeTracker;
    KoTextSelectionHandler m_selectionHandler;
    bool m_allowActions;
    bool m_allowAddUndoCommand;
    bool m_trackChanges;
    int m_prevCursorPosition; /// used by editingPluginEvents

    QAction *m_actionFormatBold;
    QAction *m_actionFormatItalic;
    QAction *m_actionFormatUnderline;
    QAction *m_actionFormatStrikeOut;
    QAction *m_actionAlignLeft;
    QAction *m_actionAlignRight;
    QAction *m_actionAlignCenter;
    QAction *m_actionAlignBlock;
    QAction *m_actionFormatSuper;
    QAction *m_actionFormatSub;
    QAction *m_actionFormatIncreaseIndent;
    QAction *m_actionFormatDecreaseIndent;

    QHash<QString, KoTextEditingPlugin*> m_textEditingPlugins;
};

#endif
