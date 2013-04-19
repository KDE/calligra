/* This file is part of the KDE project
*
* Copyright (C) 2013 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
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

#include <FormulaCursor.h>
#include <FormulaDocument.h>
#include <qtmmlwidget/qtmmlwidget.h>
#include <ChangeFormulaCommand.h>
#include <AlterFormulaCommand.h>

FormulaCursor::FormulaCursor(FormulaDocument *document)
    : QObject()
    , m_caretTimer(this)
    , m_document(document)
{
    Q_ASSERT(document);
    init();
}

FormulaCursor::~FormulaCursor()
{
    setNode(0);
}

void FormulaCursor::init() 
{
    m_node = 0;
    m_caretTimer.setInterval(500);
    connect(&m_caretTimer, SIGNAL(timeout()), this, SLOT(blinkCaret()));
}

void FormulaCursor::setNode(MmlNode *node)
{
    if (m_node) {
        m_node->m_showCursor = false;
    }

    m_node = node;

    if (m_node) {
        m_node->m_showCursor = true;
    }

    if (!m_node) {
        m_caretTimer.stop();
    } else {
        m_caretTimer.stop();
        m_caretTimer.start();
    }
}

void FormulaCursor::blinkCaret()
{
    if (m_node) {
        m_node->m_showCursor = !m_node->m_showCursor;
    }
}

void FormulaCursor::nextNode()
{
    if (!m_node) {
        return;
    }

    if (m_node->m_node_type == MmlNode::TextNode &&
            static_cast<MmlTextNode *>(m_node)->m_cursorIndex < ((static_cast<MmlTextNode *>(m_node)->text()).length())) {
        static_cast<MmlTextNode *>(m_node)->m_cursorIndex++;
        return;
    }

    MmlNode *newNode = 0;
    if (m_node->hasChildNodes()) {
        newNode = m_node->firstChild();
    } else if (MmlNode *nextMmlNode = nextNode(m_node)) {
        newNode = nextMmlNode;
    }

    if (newNode->m_node_type == MmlNode::TextNode) {
        static_cast<MmlTextNode *>(newNode)->m_cursorIndex = 0;
    }

    if (newNode) {
        setNode(newNode);
    }
}

void FormulaCursor::previousNode()
{
    if (!m_node) {
        return;
    }

    if (m_node->m_node_type == MmlNode::TextNode &&
            static_cast<MmlTextNode *>(m_node)->m_cursorIndex > 0) {
        static_cast<MmlTextNode *>(m_node)->m_cursorIndex--;
        return;
    }

    MmlNode *newNode = 0;
    if (MmlNode *previousMmlNode = previousNode(m_node)) {
        newNode = previousMmlNode;
    } else if (m_node->hasChildNodes()) {
        newNode = m_node->firstChild()->lastSibling();
    }

    if (newNode->m_node_type == MmlNode::TextNode) {
        static_cast<MmlTextNode *>(newNode)->m_cursorIndex = ((static_cast<MmlTextNode *>(newNode)->text()).length()) - 1;
    }

    if (newNode) {
        setNode(newNode);
    }
}

MmlNode *FormulaCursor::nextNode(MmlNode *node)
{
    if (!node->isLastSibling()) {
        return node->nextSibling();
    } else if(node->parent()) {
        return nextNode(node->parent());
    }

    return 0;
}

MmlNode *FormulaCursor::previousNode(MmlNode *node)
{
    if(node->parent()){
        return node->parent();
    } else if (!node->isFirstSibling()) {
        return node->previousSibling();
    }

    return 0;
}

KUndo2Command *FormulaCursor::deleteNode()
{
    if (m_node->m_node_type == MmlNode::TextNode) {
        MmlTextNode *currentCursorNode = static_cast<MmlTextNode *>(m_node);
        QString currentText = currentCursorNode->text();
        currentText.remove(currentCursorNode->m_cursorIndex, 1);
        return new AlterFormulaCommand(this, currentText, currentCursorNode->m_cursorIndex);
    } else {
        return new ChangeFormulaCommand(this);
    }
}

KUndo2Command *FormulaCursor::insertText(const QString &text)
{
    if (m_node->m_node_type == MmlNode::TextNode) {
        MmlTextNode *currentCursorNode = static_cast<MmlTextNode *>(m_node);
        QString currentText = currentCursorNode->text();
        currentText.insert(currentCursorNode->m_cursorIndex, text);
        return new AlterFormulaCommand(this, currentText, currentCursorNode->m_cursorIndex + 1);
    }
    return 0;
}

KUndo2Command *FormulaCursor::deleteText()
{
    if (m_node->m_node_type == MmlNode::TextNode) {
        MmlTextNode *currentCursorNode = static_cast<MmlTextNode *>(m_node);
        QString currentText = currentCursorNode->text();
        currentText.remove(currentCursorNode->m_cursorIndex - 1, 1);
        return new AlterFormulaCommand(this, currentText, currentCursorNode->m_cursorIndex - 1);
    }
    return 0;
}
void FormulaCursor::activate()
{
    setNode(m_document->rootNode()->firstChild());
}

void FormulaCursor::deactivate()
{
    setNode(0);
}
