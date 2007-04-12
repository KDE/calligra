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

#include "TextTool.h"
#include "ChangeTracker.h"
#include "dialogs/SimpleStyleWidget.h"
#include "dialogs/StylesWidget.h"
#include "dialogs/ParagraphSettingsDialog.h"

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>

#include <KoTextDocumentLayout.h>
#include <KoTextEditingPlugin.h>
#include <KoTextEditingRegistry.h>
#include <KoTextEditingFactory.h>
#include <KoXmlWriter.h>

#include <kdebug.h>
#include <KStandardShortcut>
#include <KAction>
#include <KStandardAction>
#include <QAbstractTextDocumentLayout>
#include <QAction>
#include <QBuffer>
#include <QTextBlock>
#include <QTabWidget>
#include <QTextLayout>
#include <QKeyEvent>
#include <QUndoCommand>
#include <QPointer>
#include <QMenu>
#include <QClipboard>

static bool hit(const QKeySequence &input, KStandardShortcut::StandardShortcut shortcut) {
    foreach(QKeySequence ks, KStandardShortcut::shortcut(shortcut)) {
        if(input == ks)
            return true;
    }
    return false;
}

TextTool::TextTool(KoCanvasBase *canvas)
: KoTool(canvas),
    m_textShape(0),
    m_textShapeData(0),
    m_changeTracker(0),
    m_allowActions(true),
    m_allowAddUndoCommand(true),
    m_trackChanges(false),
    m_prevCursorPosition(-1)
{
    m_actionFormatBold  = new QAction(KIcon("format-text-bold"), i18n("Bold"), this);
    addAction("format_bold", m_actionFormatBold );
    m_actionFormatBold->setShortcut(Qt::CTRL + Qt::Key_B);
    m_actionFormatBold->setCheckable(true);
    connect( m_actionFormatBold, SIGNAL(toggled(bool)), &m_selectionHandler, SLOT(bold(bool)) );

    m_actionFormatItalic  = new QAction(KIcon("format-text-italic"), i18n("Italic"), this);
    addAction("format_italic", m_actionFormatItalic );
    m_actionFormatItalic->setShortcut( Qt::CTRL + Qt::Key_I);
    m_actionFormatItalic->setCheckable(true);
    connect( m_actionFormatItalic, SIGNAL(toggled(bool)), &m_selectionHandler, SLOT(italic(bool)) );

    m_actionFormatUnderline  = new QAction(KIcon("format-text-underline"), i18n("Underline"), this);
    addAction("format_underline", m_actionFormatUnderline );
    m_actionFormatUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    m_actionFormatUnderline->setCheckable(true);
    connect( m_actionFormatUnderline, SIGNAL(toggled(bool)), &m_selectionHandler, SLOT(underline(bool)) );

    m_actionFormatStrikeOut  = new QAction(KIcon("format-text-strikethrough"), i18n("Strike Out"), this);
    addAction("format_strike", m_actionFormatStrikeOut );
    m_actionFormatStrikeOut->setCheckable(true);
    connect( m_actionFormatStrikeOut, SIGNAL(toggled(bool)), &m_selectionHandler, SLOT(strikeOut(bool)) );

    QActionGroup *alignmentGroup = new QActionGroup(this);
    m_actionAlignLeft  = new QAction(KIcon("text-left"), i18n("Align Left"), this);
    addAction("format_alignleft", m_actionAlignLeft );
    m_actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    m_actionAlignLeft->setCheckable(true);
    alignmentGroup->addAction(m_actionAlignLeft);
    connect(m_actionAlignLeft, SIGNAL(toggled(bool)), this, SLOT(alignLeft()));

    m_actionAlignRight  = new QAction(KIcon("text-right"), i18n("Align Right"), this);
    addAction("format_alignright", m_actionAlignRight );
    m_actionAlignRight->setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_R);
    m_actionAlignRight->setCheckable(true);
    alignmentGroup->addAction(m_actionAlignRight);
    connect(m_actionAlignRight, SIGNAL(toggled(bool)), this, SLOT(alignRight()));

    m_actionAlignCenter  = new QAction(KIcon("text-center"), i18n("Align Center"), this);
    addAction("format_aligncenter", m_actionAlignCenter );
    m_actionAlignCenter->setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_C);
    m_actionAlignCenter->setCheckable(true);
    alignmentGroup->addAction(m_actionAlignCenter);
    connect(m_actionAlignCenter, SIGNAL(toggled(bool)), this, SLOT(alignCenter()));

    m_actionAlignBlock  = new QAction(KIcon("format-justify-fill"), i18n("Align Block"), this);
    addAction("format_alignblock", m_actionAlignBlock );
    m_actionAlignBlock->setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_R);
    m_actionAlignBlock->setCheckable(true);
    alignmentGroup->addAction(m_actionAlignBlock);
    connect(m_actionAlignBlock, SIGNAL(toggled(bool)), this, SLOT(alignBlock()));

    m_actionFormatSuper = new QAction(KIcon("text-super"), i18n("Superscript"), this);
    addAction("format_super", m_actionFormatSuper );
    m_actionFormatSuper->setCheckable(true);
    connect(m_actionFormatSuper, SIGNAL(toggled(bool)), this, SLOT(superScript(bool)));

    m_actionFormatSub = new QAction(KIcon("text-sub"), i18n("Subscript"), this);
    addAction("format_sub", m_actionFormatSub );
    m_actionFormatSub->setCheckable(true);
    connect(m_actionFormatSub, SIGNAL(toggled(bool)), this, SLOT(subScript(bool)));

    m_actionFormatIncreaseIndent = new QAction(
            KIcon(QApplication::isRightToLeft() ? "format-indent-less" : "format-indent-more"),
            i18n("Increase Indent"), this);
    addAction("format_increaseindent", m_actionFormatIncreaseIndent );
    connect(m_actionFormatIncreaseIndent, SIGNAL(triggered()), this, SLOT(increaseIndent()));

    m_actionFormatDecreaseIndent = new QAction(
            KIcon(QApplication::isRightToLeft() ? "format-indent-more" :"format-indent-less"),
            i18n("Decrease Indent"), this);
    addAction("format_decreaseindent", m_actionFormatDecreaseIndent );
    connect(m_actionFormatDecreaseIndent, SIGNAL(triggered()), this, SLOT(decreaseIndent()));

    // ------------------- Actions with a key binding and no GUI item
    QAction *action  = new QAction(i18n("Insert Non-Breaking Space"), this);
    addAction("nonbreaking_space", action );
    action->setShortcut( Qt::CTRL+Qt::Key_Space);
    connect(action, SIGNAL(triggered()), this, SLOT( nonbreakingSpace() ));

    action  = new QAction(i18n("Insert Non-Breaking Hyphen"), this);
    addAction("nonbreaking_hyphen", action );
    action->setShortcut( Qt::CTRL+Qt::SHIFT+Qt::Key_Minus);
    connect(action, SIGNAL(triggered()), this, SLOT( nonbreakingHyphen() ));

    action  = new QAction(i18n("Insert Index"), this);
action->setShortcut( Qt::CTRL+ Qt::Key_T);
    addAction("insert_index", action );
    connect(action, SIGNAL(triggered()), this, SLOT( insertIndexMarker() ));

    action  = new QAction(i18n("Insert Soft Hyphen"), this);
    addAction("soft_hyphen", action );
    //action->setShortcut( Qt::CTRL+Qt::Key_Minus); // TODO this one is also used for the kde-global zoom-out :(
    connect(action, SIGNAL(triggered()), this, SLOT( softHyphen() ));

    action  = new QAction(i18n("Line Break"), this);
    addAction("line_break", action );
    action->setShortcut( Qt::SHIFT+Qt::Key_Return);
    connect(action, SIGNAL(triggered()), this, SLOT( lineBreak() ));

    action  = new QAction(i18n("Font..."), this);
    addAction("format_font", action );
    action->setShortcut( Qt::ALT + Qt::CTRL + Qt::Key_F);
    action->setToolTip( i18n( "Change character size, font, boldface, italics etc." ) );
    action->setWhatsThis( i18n( "Change the attributes of the currently selected characters." ) );
    connect(action, SIGNAL(triggered()), &m_selectionHandler, SLOT( selectFont() ));

    action = new QAction(i18n("Default Format"), this);
    addAction("text_default", action);
    action->setToolTip( i18n( "Change font and paragraph attributes to their default values" ) );
    connect(action, SIGNAL(triggered()), this, SLOT( textDefaultFormat() ));

    foreach(QString key, KoTextEditingRegistry::instance()->keys()) {
        KoTextEditingFactory *factory =  KoTextEditingRegistry::instance()->get(key);
        Q_ASSERT(factory);
        if(m_textEditingPlugins.contains(factory->objectId())) {
            kWarning(32500) << "Duplicate id for textEditingPlugin, ignoring one! (" << factory->objectId() << ")\n";
            continue;
        }
        m_textEditingPlugins.insert(factory->objectId(), factory->create());
    }

    action = new QAction(i18n("Paragraph..."), this);
    addAction("format_paragraph", action);
    action->setShortcut(Qt::ALT + Qt::CTRL + Qt::Key_P);
    connect(action, SIGNAL(triggered()), this, SLOT(formatParagraph()));

    action = new QAction(i18n("Record"), this);
    action->setCheckable(true);
    addAction("edit_record_changes", action);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(toggleTrackChanges(bool)));

    action = KStandardAction::selectAll(this, SLOT(selectAll()), this);
    addAction("edit_selectall", action);
}

TextTool::~TextTool() {
    qDeleteAll(m_textEditingPlugins);
}

void TextTool::paint( QPainter &painter, KoViewConverter &converter) {
    QTextBlock block = m_caret.block();
    if(! block.layout()) // not layouted yet.  The Shape paint method will trigger a layout
        return;

    if(painter.hasClipping()) {
        QRect shape = converter.documentToView(m_textShape->boundingRect()).toRect();
        if(painter.clipRegion().intersect( QRegion(shape) ).isEmpty())
            return;
    }

    painter.setMatrix( painter.matrix() * m_textShape->transformationMatrix(&converter) );
    double zoomX, zoomY;
    converter.zoom(&zoomX, &zoomY);
    painter.scale(zoomX, zoomY);
    Q_ASSERT(m_textShapeData);
    painter.translate(0, -m_textShapeData->documentOffset());

    if(m_textShapeData && m_caret.hasSelection()) {
        QAbstractTextDocumentLayout::PaintContext pc;
        QAbstractTextDocumentLayout::Selection selection;
        selection.cursor = m_caret;
        selection.format.setBackground(QBrush(Qt::yellow)); // TODO use configured selection color
        selection.format.setForeground(QBrush(Qt::black)); // TODO use configured selected-text color
        pc.selections.append(selection);

        QRectF clip = textRect(m_caret.position(), m_caret.anchor());
        painter.save();
        painter.setClipRect(clip, Qt::IntersectClip);
        m_textShapeData->document()->documentLayout()->draw( &painter, pc);
        painter.restore();
    }

    // paint caret
    QPen pen(Qt::black);
    if(! m_textShape->hasTransparency()) {
        QColor bg = m_textShape->background().color();
        QColor invert = QColor(255 - bg.red(), 255 - bg.green(), 255 - bg.blue());
        pen.setColor(invert);
    }
    painter.setPen(pen);
    block.layout()->drawCursor(&painter, QPointF(0,0), m_caret.position() - block.position());
}

void TextTool::mousePressEvent( KoPointerEvent *event ) {
    const bool canMoveCaret = !m_caret.hasSelection() || event->button() !=  Qt::RightButton;

    if(canMoveCaret && ! m_textShape->boundingRect().contains(event->point)) {
        QRectF area(event->point, QSizeF(1,1));
        foreach(KoShape *shape, m_canvas->shapeManager()->shapesAt(area, true)) {
            TextShape *textShape = dynamic_cast<TextShape*> (shape);
            if(textShape) {
                m_textShape = textShape;
                KoTextShapeData *d = static_cast<KoTextShapeData*> (textShape->userData());
                if(d->document() == m_textShapeData->document())
                    break; // stop looking.
            }
        }
        setShapeData(static_cast<KoTextShapeData*> (m_textShape->userData()));
    }

    if(canMoveCaret) {
        bool shiftPressed = event->modifiers() & Qt::ShiftModifier;
        if(m_caret.hasSelection() && !shiftPressed)
            repaintSelection(m_caret.position(), m_caret.anchor()); // will erase selection
        else if(! m_caret.hasSelection())
            repaintCaret();
        int prevPosition = m_caret.position();
        int position = pointToPosition(event->point);
        m_caret.setPosition(position, shiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
        if(shiftPressed) // altered selection.
            repaintSelection(prevPosition, m_caret.position());
        else
            repaintCaret();

        updateSelectionHandler();
        updateStyleManager();
    }
    updateActions();

    if(event->button() ==  Qt::RightButton) {
        QMenu menu(m_canvas->canvasWidget());
        menu.addAction(action("text_default"));
        menu.addAction(action("format_font"));

        foreach(QString key, KoTextEditingRegistry::instance()->keys()) {
            KoTextEditingFactory *factory =  KoTextEditingRegistry::instance()->get(key);
            if(factory->showInMenu()) {
                QAction *action = new QAction(factory->title(), &menu);
                action->setData(factory->objectId());
                menu.addAction(action);
            }
        }

        QAction * action = menu.exec(event->globalPos());
        KoTextEditingPlugin *plugin = 0;
        if(action)
           plugin = m_textEditingPlugins.value(qvariant_cast<QString>(action->data()));
        if(plugin) {
            if(m_caret.hasSelection()) {
                int from = m_caret.position();
                int to = m_caret.anchor();
                if(from > to) // make sure we call the plugin consistently
                    qSwap(from, to);
                plugin->checkSection(m_textShapeData->document(), from, to);
            }
            else
                plugin->finishedWord(m_textShapeData->document(), m_caret.position());
        }
    }
}

void TextTool::setShapeData(KoTextShapeData *data) {
    bool docChanged = data == 0 || m_textShapeData == 0 || m_textShapeData->document() != data->document();
    if(m_textShapeData && docChanged)
        disconnect(m_textShapeData->document(), SIGNAL(undoAvailable(bool)), this, SLOT(addUndoCommand()));
    m_textShapeData = data;
    if(m_textShapeData && docChanged) {
        connect(m_textShapeData->document(), SIGNAL(undoAvailable(bool)), this, SLOT(addUndoCommand()));
        m_caret = QTextCursor(m_textShapeData->document());

        if(m_textShape->demoText()) {
            m_textShapeData->document()->setUndoRedoEnabled(false); // removes undo history
            m_textShape->setDemoText(false); // remove demo text
        }
        m_textShapeData->document()->setUndoRedoEnabled(true); // allow undo history
    }
    if(m_trackChanges) {
        if(m_changeTracker == 0)
            m_changeTracker = new ChangeTracker(this);
        m_changeTracker->setDocument(m_textShapeData->document());
    }
}

void TextTool::updateSelectionHandler() {
    m_selectionHandler.setShape(m_textShape);
    m_selectionHandler.setShapeData(m_textShapeData);
    m_selectionHandler.setCaret(&m_caret);
    emit sigSelectionChanged(m_caret.hasSelection());
}

void TextTool::copy() {
    if(m_textShapeData == 0 || !m_caret.hasSelection()) return;
    int from = m_caret.position();
    int to = m_caret.anchor();
    if(to < from)
        qSwap(to, from);

    QByteArray bytes;
    QBuffer buffer( &bytes );
    buffer.open( QIODevice::WriteOnly );
    KoXmlWriter writer( &buffer );
    //writer.startDocument( "foobar" );
    m_textShapeData->saveOdf(&writer, from, to);
    //writer.endDocument();
    buffer.putChar( '\0' ); // null-terminate
    QMimeData *data = new QMimeData();
    data->setData("application/vnd.oasis.opendocument.text", bytes);
    QApplication::clipboard()->setMimeData(data);
kDebug() << "output: " << QString::fromUtf8(bytes) << endl;
}

int TextTool::pointToPosition(const QPointF & point) const {
    QPointF p = m_textShape->convertScreenPos(point);
    int caretPos = m_caret.block().document()->documentLayout()->hitTest(p, Qt::FuzzyHit);
    caretPos = qMax(caretPos, m_textShapeData->position());
    if(m_textShapeData->endPosition() == -1) {
        kWarning(32500) << "Clicking in not fully laid-out textframe\n";
        m_textShapeData->fireResizeEvent(); // requests a layout run ;)
    }
    caretPos = qMin(caretPos, m_textShapeData->endPosition());
    return caretPos;
}

void TextTool::mouseDoubleClickEvent( KoPointerEvent *event ) {
    Q_UNUSED(event); // all positioning has ben done by the first click
    m_caret.clearSelection();
    int pos = m_caret.position();
    m_caret.movePosition(QTextCursor::WordLeft);
    m_caret.movePosition(QTextCursor::WordRight, QTextCursor::KeepAnchor);
    if(qAbs(pos - m_caret.position()) <= 1) // clicked between two words
        m_caret.movePosition(QTextCursor::WordRight, QTextCursor::KeepAnchor);

    repaintSelection(m_caret.anchor(), m_caret.position());
}

void TextTool::mouseMoveEvent( KoPointerEvent *event ) {
    useCursor(Qt::IBeamCursor);
    if(event->buttons() == Qt::NoButton)
        return;
    int position = pointToPosition(event->point);
    if(position == m_caret.position()) return;
    if(position >= 0) {
        repaintCaret();
        int prevPos = m_caret.position();
        m_caret.setPosition(position, QTextCursor::KeepAnchor);
        repaintSelection(prevPos, m_caret.position());
    }

    updateSelectionHandler();
}

void TextTool::mouseReleaseEvent( KoPointerEvent *event ) {
    event->ignore();
    editingPluginEvents();
}

void TextTool::keyPressEvent(QKeyEvent *event) {
    int destinationPosition = -1; // for those that the moveOperation is not implemented;
    QTextCursor::MoveOperation moveOperation = QTextCursor::NoMove;
    if(event->key() == Qt::Key_Backspace) {
        if(!m_caret.hasSelection() && event->modifiers() & Qt::ControlModifier) // delete prev word.
            m_caret.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
        m_caret.deletePreviousChar();
        editingPluginEvents();
    }
    else if(event->key() == Qt::Key_Delete) {
        if(!m_caret.hasSelection() && event->modifiers() & Qt::ControlModifier) // delete next word.
            m_caret.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
        m_caret.deleteChar();
        editingPluginEvents();
    }
    else if((event->key() == Qt::Key_Left) && (event->modifiers() | Qt::ShiftModifier) == Qt::ShiftModifier)
        moveOperation = QTextCursor::Left;
    else if((event->key() == Qt::Key_Right) && (event->modifiers() | Qt::ShiftModifier) == Qt::ShiftModifier)
        moveOperation = QTextCursor::Right;
    else if((event->key() == Qt::Key_Up) && (event->modifiers() | Qt::ShiftModifier) == Qt::ShiftModifier)
        moveOperation = QTextCursor::Up;
    else if((event->key() == Qt::Key_Down) && (event->modifiers() | Qt::ShiftModifier) == Qt::ShiftModifier)
        moveOperation = QTextCursor::Down;
    else {
        // check for shortcuts.
        QKeySequence item(event->key() | ((Qt::ControlModifier | Qt::AltModifier) & event->modifiers()));
        if(hit(item, KStandardShortcut::Home))
            // Goto beginning of the document. Default: Ctrl-Home
            destinationPosition = 0;
        else if(hit(item, KStandardShortcut::End)) {
            // Goto end of the document. Default: Ctrl-End
            QTextBlock last = m_textShapeData->document()->end().previous();
            destinationPosition = last.position() + last.length() -1;
        }
        else if(hit(item, KStandardShortcut::Prior)) // page up
            // Scroll up one page. Default: Prior
            moveOperation = QTextCursor::StartOfLine; // TODO
        else if(hit(item, KStandardShortcut::Next))
            // Scroll down one page. Default: Next
            moveOperation = QTextCursor::StartOfLine; // TODO
        else if(hit(item, KStandardShortcut::BeginningOfLine))
            // Goto beginning of current line. Default: Home
            moveOperation = QTextCursor::StartOfLine;
        else if(hit(item, KStandardShortcut::EndOfLine))
            // Goto end of current line. Default: End
            moveOperation = QTextCursor::EndOfLine;
        else if(hit(item, KStandardShortcut::BackwardWord))
            moveOperation = QTextCursor::WordLeft;
        else if(hit(item, KStandardShortcut::ForwardWord))
            moveOperation = QTextCursor::NextWord;
#ifndef NDEBUG
        else if(event->key() == Qt::Key_F12) {
            m_caret.insertText(QString::fromUtf8("װעלט"));
        }
#endif
        else if((event->modifiers() & (Qt::ControlModifier | Qt::AltModifier)) || event->text().length() == 0) {
            event->ignore();
            return;
        }
        else if(event->text().at(0) == '\r') {
            m_selectionHandler.nextParagraph();
            updateActions();
            editingPluginEvents();
            ensureCursorVisible();
        }
        else {
            m_prevCursorPosition = m_caret.position();
            ensureCursorVisible();
            m_caret.insertText(event->text());
            editingPluginEvents();
        }
    }
    if(moveOperation != QTextCursor::NoMove || destinationPosition != -1) {
        useCursor(Qt::BlankCursor);
        bool shiftPressed = event->modifiers() & Qt::ShiftModifier;
        if(m_caret.hasSelection() && !shiftPressed)
            repaintSelection(m_caret.position(), m_caret.anchor()); // will erase selection
        else if(! m_caret.hasSelection())
            repaintCaret();
        // TODO if RTL toggle direction of cursor movement.
        int prevPosition = m_caret.position();
        if(moveOperation != QTextCursor::NoMove)
            m_caret.movePosition(moveOperation,
                shiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
        else
            m_caret.setPosition(destinationPosition,
                shiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
        if(shiftPressed) // altered selection.
            repaintSelection(prevPosition, m_caret.position());
        else
            repaintCaret();
        updateActions();
        editingPluginEvents();
        ensureCursorVisible();
    }

    updateSelectionHandler();
}

void TextTool::ensureCursorVisible() {
    if(m_textShapeData->endPosition() < m_caret.position() || m_textShapeData->position() > m_caret.position()) {
        KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*> (m_textShapeData->document()->documentLayout());
        Q_ASSERT(lay);
        foreach(KoShape* shape, lay->shapes()) {
            TextShape *textShape = dynamic_cast<TextShape*> (shape);
            Q_ASSERT(textShape);
            KoTextShapeData *d = static_cast<KoTextShapeData*> (textShape->userData());
            if(m_caret.position() >= d->position() && m_caret.position() <= d->endPosition()) {
                m_textShapeData = d;
                m_textShape = textShape;
                break;
            }
        }
    }

    QRectF cursorPos = textRect(m_caret.position(), m_caret.position());
    cursorPos.moveTop(cursorPos.top() - m_textShapeData->documentOffset());
    m_canvas->ensureVisible(m_textShape->transformationMatrix(0).mapRect(cursorPos));
}

void TextTool::keyReleaseEvent(QKeyEvent *event) {
    event->ignore();
}

void TextTool::updateActions() {
    m_allowActions = false;
    QTextCharFormat cf = m_caret.charFormat();
    m_actionFormatBold->setChecked(cf.fontWeight() > QFont::Normal);
    m_actionFormatItalic->setChecked(cf.fontItalic());
    m_actionFormatUnderline->setChecked(cf.fontUnderline());
    m_actionFormatStrikeOut->setChecked(cf.fontStrikeOut());
    bool super=false, sub=false;
    switch(cf.verticalAlignment()) {
        case QTextCharFormat::AlignSuperScript: super = true; break;
        case QTextCharFormat::AlignSubScript: sub = true; break;
        default:;
    }
    m_actionFormatSuper->setChecked(super);
    m_actionFormatSub->setChecked(sub);

    QTextBlockFormat bf = m_caret.blockFormat();
    switch(bf.alignment()) {
        case Qt::AlignLeft: m_actionAlignLeft->setChecked(true); break;
        case Qt::AlignRight: m_actionAlignRight->setChecked(true); break;
        case Qt::AlignHCenter: m_actionAlignCenter->setChecked(true); break;
        case Qt::AlignJustify: m_actionAlignBlock->setChecked(true); break;
    }
    m_actionFormatDecreaseIndent->setEnabled(m_caret.blockFormat().leftMargin() > 0.);
    m_allowActions = true;

    //action("text_default")->setEnabled(m_caret.hasSelection());

    emit charFormatChanged(cf);
    emit blockFormatChanged(bf);
}

void TextTool::updateStyleManager() {
    Q_ASSERT(m_textShapeData);
    KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*> (m_textShapeData->document()->documentLayout());
    if(lay)
        emit(styleManagerChanged(lay->styleManager()));
    else {
        emit(styleManagerChanged(0));
        kWarning(32500) << "Shape does not have a KoTextDocumentLayout\n";
    }
}

void TextTool::activate (bool temporary) {
    Q_UNUSED(temporary);
    KoSelection *selection = m_canvas->shapeManager()->selection();
    foreach(KoShape *shape, selection->selectedShapes()) {
        m_textShape = dynamic_cast<TextShape*> (shape);
        if(m_textShape)
            break;
    }
    if(m_textShape == 0) { // none found
        emit sigDone();
        return;
    }
    foreach(KoShape *shape, selection->selectedShapes()) {
        // deselect others.
        if(m_textShape == shape) continue;
        selection->deselect(shape);
    }
    setShapeData(static_cast<KoTextShapeData*> (m_textShape->userData()));
    useCursor(Qt::IBeamCursor, true);
    m_textShape->repaint();

    updateSelectionHandler();
    updateActions();
    updateStyleManager();
}

void TextTool::deactivate() {
    m_textShape = 0;
    if(m_textShapeData)
        m_textShapeData->document()->setUndoRedoEnabled(false); // erase undo history.
    setShapeData(0);

    updateSelectionHandler();
}

void TextTool::repaintCaret() {
    QTextBlock block = m_caret.block();
    if(block.isValid()) {
        QTextLine tl = block.layout()->lineForTextPosition(m_caret.position() - block.position());
        QRectF repaintRect;
        if(tl.isValid()) {
            repaintRect = tl.rect();
            repaintRect.setX(tl.cursorToX(m_caret.position() - block.position()));
            repaintRect.setWidth(6);
        }
        repaintRect.moveTop(repaintRect.y() - m_textShapeData->documentOffset());
        repaintRect = m_textShape->transformationMatrix(0).mapRect(repaintRect);
        m_canvas->updateCanvas(repaintRect);
    }
}

void TextTool::repaintSelection(int startPosition, int endPosition) {
    QList<TextShape *> shapes;
    if(m_textShapeData->position() > startPosition || m_textShapeData->endPosition() < endPosition) {
        KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*> (m_textShapeData->document()->documentLayout());
        Q_ASSERT(lay);
        foreach(KoShape* shape, lay->shapes()) {
            TextShape *textShape = dynamic_cast<TextShape*> (shape);
            if(textShape->textShapeData()->position() >= startPosition &&
                    textShape->textShapeData()->endPosition() <= endPosition)
                shapes.append(textShape);
        }
    }
    else // the simple case; the full selection is inside the current shape.
        shapes.append(m_textShape);

    // loop over all shapes that contain the text and update per shape.
    QRectF repaintRect = textRect(startPosition, endPosition);
    foreach(TextShape *ts, shapes) {
        QRectF rect = repaintRect;
        rect.moveTop(rect.y() - ts->textShapeData()->documentOffset());
        rect = ts->transformationMatrix(0).mapRect(rect);
        m_canvas->updateCanvas(ts->boundingRect().intersected(rect));
    }
}

QRectF TextTool::textRect(int startPosition, int endPosition) {
    if(startPosition > endPosition)
        qSwap(startPosition, endPosition);
    QTextBlock block = m_textShapeData->document()->findBlock(startPosition);
    QTextLine line1 = block.layout()->lineForTextPosition(startPosition - block.position());
    if(! line1.isValid())
        return QRectF();
    double startX = line1.cursorToX(startPosition - block.position());
    if(startPosition == endPosition)
        return QRectF(startX, line1.y(), 1, line1.height());

    QTextBlock block2 = m_textShapeData->document()->findBlock(endPosition);
    QTextLine line2 = block2.layout()->lineForTextPosition(endPosition - block2.position());
    if(! line2.isValid())
        return QRectF();
    double endX = line2.cursorToX(endPosition - block2.position());

    if(line1.textStart() + block.position() == line2.textStart()+ block2.position() )
        return QRectF(qMin(startX, endX), line1.y(), qAbs(startX - endX), line1.height());
    return QRectF(0, line1.y(), 10E6, line2.y() + line2.height() - line1.y());
}

KoToolSelection* TextTool::selection() {
    return &m_selectionHandler;
}

QWidget *TextTool::createOptionWidget() {
    QTabWidget *widget = new QTabWidget();
    widget->addTab(new SimpleStyleWidget(this, widget), i18n("Style"));
    StylesWidget *paragTab = new StylesWidget(StylesWidget::ParagraphStyle, widget);
    widget->addTab(paragTab, i18n("Paragraph"));
    StylesWidget *charTab =new StylesWidget(StylesWidget::CharacterStyle, widget);
    widget->addTab(charTab, i18n("Character"));

    connect(this, SIGNAL(styleManagerChanged(KoStyleManager *)), paragTab, SLOT(setStyleManager(KoStyleManager *)));
    connect(this, SIGNAL(styleManagerChanged(KoStyleManager *)), charTab, SLOT(setStyleManager(KoStyleManager *)));
    connect(this, SIGNAL(charFormatChanged(const QTextCharFormat &)),
            paragTab, SLOT(setCurrentFormat(const QTextCharFormat &)));
    connect(this, SIGNAL(blockFormatChanged(const QTextBlockFormat &)),
            paragTab, SLOT(setCurrentFormat(const QTextBlockFormat &)));
    connect(this, SIGNAL(charFormatChanged(const QTextCharFormat &)),
            charTab, SLOT(setCurrentFormat(const QTextCharFormat &)));

    connect(paragTab, SIGNAL(paragraphStyleSelected(KoParagraphStyle *)),
            &m_selectionHandler, SLOT(setStyle(KoParagraphStyle*)));
    connect(charTab, SIGNAL(characterStyleSelected(KoCharacterStyle *)),
            &m_selectionHandler, SLOT(setStyle(KoCharacterStyle*)));

    updateStyleManager();
    return widget;
}

void TextTool::addUndoCommand() {
    if(! m_allowAddUndoCommand) return;
    class UndoTextCommand : public QUndoCommand {
      public:
        UndoTextCommand(QTextDocument *document, TextTool *tool)
            : QUndoCommand(i18n("Text")),
            m_document(document),
            m_tool(tool)
        {
        }

        void undo () {
            if(m_document.isNull())
                return;
            if(! m_tool.isNull()) {
                m_tool->m_allowAddUndoCommand = false;
                if(m_tool->m_changeTracker)
                    m_tool->m_changeTracker->notifyForUndo();
                m_document->undo(&m_tool->m_caret);
            }
            else
                m_document->undo();
            if(! m_tool.isNull())
                m_tool->m_allowAddUndoCommand = true;
        }

        void redo () {
            if(m_document.isNull())
                return;
            if(! m_tool.isNull()) {
                m_tool->m_allowAddUndoCommand = false;
                 m_document->redo(&m_tool->m_caret);
            }
            else
                m_document->redo();
            if(! m_tool.isNull())
                m_tool->m_allowAddUndoCommand = true;
        }

        QPointer<QTextDocument> m_document;
        QPointer<TextTool> m_tool;
    };
    m_canvas->addCommand(new UndoTextCommand(m_textShapeData->document(), this));
}

void TextTool::nonbreakingSpace() {
    if(! m_allowActions) return;
    m_selectionHandler.insert(QString(QChar(Qt::Key_nobreakspace)));
}

void TextTool::nonbreakingHyphen() {
    if(! m_allowActions) return;
    m_selectionHandler.insert(QString(QChar(0x2013)));
}

void TextTool::softHyphen() {
    if(! m_allowActions) return;
    m_selectionHandler.insert(QString(QChar(Qt::Key_hyphen)));
}

void TextTool::lineBreak() {
    if(! m_allowActions) return;
    m_selectionHandler.insert(QString(QChar('\n')));
}

void TextTool::alignLeft() {
    if(! m_allowActions) return;
    m_selectionHandler.setHorizontalTextAlignment(Qt::AlignLeft);
}

void TextTool::alignRight() {
    if(! m_allowActions) return;
    m_selectionHandler.setHorizontalTextAlignment(Qt::AlignRight);
}

void TextTool::alignCenter() {
    if(! m_allowActions) return;
    m_selectionHandler.setHorizontalTextAlignment(Qt::AlignHCenter);
}

void TextTool::alignBlock() {
    if(! m_allowActions) return;
    m_selectionHandler.setHorizontalTextAlignment(Qt::AlignJustify);
}

void TextTool::superScript(bool on) {
    if(! m_allowActions) return;
    if(on)
        m_actionFormatSub->setChecked(false);
    m_selectionHandler.setVerticalTextAlignment(on ? Qt::AlignTop : Qt::AlignVCenter);
}

void TextTool::subScript(bool on) {
    if(! m_allowActions) return;
    if(on)
        m_actionFormatSuper->setChecked(false);
    m_selectionHandler.setVerticalTextAlignment(on ? Qt::AlignBottom : Qt::AlignVCenter);
}

void TextTool::increaseIndent() {
    if(! m_allowActions) return;
    m_selectionHandler.increaseIndent();
    m_actionFormatDecreaseIndent->setEnabled(m_caret.blockFormat().leftMargin() > 0.);
}

void TextTool::decreaseIndent() {
    if(! m_allowActions) return;
    m_selectionHandler.decreaseIndent();
    m_actionFormatDecreaseIndent->setEnabled(m_caret.blockFormat().leftMargin() > 0.);
}

void TextTool::textDefaultFormat() {
    // TODO
    kDebug() << "TextTool::textDefaultFormat\n";
}

void TextTool::insertIndexMarker() {
    // TODO handle result when we figure out how to report errors from a tool.
    m_selectionHandler.insertIndexMarker();
}

void TextTool::formatParagraph() {
    ParagraphSettingsDialog *dia = new ParagraphSettingsDialog(m_canvas->canvasWidget());
    dia->open(m_caret);
    dia->setUnit(m_canvas->unit());

    dia->show();
}

void TextTool::toggleTrackChanges(bool on) {
    m_trackChanges = on;
    if(on){
        if(m_textShapeData && m_changeTracker == 0)
            m_changeTracker = new ChangeTracker(this);
        if(m_changeTracker)
            m_changeTracker->setDocument(m_textShapeData->document());
    }
    else if(m_changeTracker)
        m_changeTracker->setDocument(0);
}

void TextTool::selectAll() {
    if(m_textShapeData == 0) return;
    QTextBlock lastBlock = m_textShapeData->document()->end().previous();
    m_caret.setPosition(lastBlock.position() + lastBlock.length() - 1);
    m_caret.setPosition(0, QTextCursor::KeepAnchor);
    repaintSelection(0, m_caret.anchor());
}


// ---------- editing plugins methods.
void TextTool::editingPluginEvents() {
    if(m_prevCursorPosition == -1 || m_prevCursorPosition == m_caret.position())
        return;

    QTextBlock block = m_caret.block();
    if(! block.contains(m_prevCursorPosition)) {
        finishedWord();
        finishedParagraph();
        m_prevCursorPosition = -1;
    }
    else {
        int from = m_prevCursorPosition;
        int to = m_caret.position();
        if(from > to)
            qSwap(from, to);
        QString section = block.text().mid(from, to - from);
        if(section.contains(' ')) {
            finishedWord();
            m_prevCursorPosition = -1;
        }
    }
}

void TextTool::finishedWord() {
    foreach(KoTextEditingPlugin* plugin, m_textEditingPlugins.values())
        plugin->finishedWord(m_textShapeData->document(), m_prevCursorPosition);
}

void TextTool::finishedParagraph() {
    foreach(KoTextEditingPlugin* plugin, m_textEditingPlugins.values())
        plugin->finishedParagraph(m_textShapeData->document(), m_prevCursorPosition);
}

#include "TextTool.moc"
