/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006 Martin Pfeiffer <hubipete@gmx.net>
                 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FormulaEditor.h"

#include "BasicElement.h"
#include "ElementFactory.h"
#include "FixedElement.h"
#include "FormulaCommand.h"
#include "FormulaDebug.h"
#include "IdentifierElement.h"
#include "NumberElement.h"
#include "OperatorElement.h"
#include "RowElement.h"
#include "TableDataElement.h"
#include "TableElement.h"
#include "TableRowElement.h"

#include <QObject>
#include <QPainter>
#include <QPen>
#include <algorithm>

#include <KLocalizedString>
#include <KoOdfLoadingContext.h>
#include <kundo2command.h>

FormulaEditor::FormulaEditor(FormulaCursor cursor, FormulaData *data)
{
    m_cursor = cursor;
    m_data = data;
}

FormulaEditor::FormulaEditor(FormulaData *data)
{
    m_cursor = FormulaCursor(data->formulaElement(), 0);
    m_data = data;
}

void FormulaEditor::paint(QPainter &painter) const
{
    m_cursor.paint(painter);
}

FormulaCommand *FormulaEditor::insertText(const QString &text)
{
    FormulaCommand *undo = nullptr;
    m_inputBuffer = text;
    if (m_cursor.insideToken()) {
        TokenElement *token = static_cast<TokenElement *>(m_cursor.currentElement());
        if (m_cursor.hasSelection()) {
            undo = new FormulaCommandReplaceText(token, m_cursor.selection().first, m_cursor.selection().second - m_cursor.selection().first, text);
        } else {
            undo = new FormulaCommandReplaceText(token, m_cursor.position(), 0, text);
        }
    } else {
        TokenElement *token = static_cast<TokenElement *>(ElementFactory::createElement(tokenType(text[0]), nullptr));
        token->insertText(0, text);
        undo = insertElement(token);
        if (undo) {
            undo->setRedoCursorPosition(FormulaCursor(token, text.length()));
        }
    }
    if (undo) {
        undo->setText(kundo2_i18n("Add text"));
    }
    return undo;
}

FormulaCommand *FormulaEditor::insertMathML(const QString &data)
{
    // setup a DOM structure and start the actual loading process
    KoXmlDocument tmpDocument;
    tmpDocument.setContent(QString(data), false, nullptr, nullptr, nullptr);
    BasicElement *element = ElementFactory::createElement(tmpDocument.documentElement().tagName(), nullptr);
    element->readMathML(tmpDocument.documentElement()); // and load the new formula
    FormulaCommand *command = insertElement(element);
    debugFormula << "Inserting " << tmpDocument.documentElement().tagName();
    if (command == nullptr) {
        delete element;
    }
    return command;
}

FormulaCommand *FormulaEditor::changeTable(bool insert, bool rows)
{
    FormulaCommand *undo;
    TableDataElement *data = m_cursor.currentElement()->parentTableData();
    if (data) {
        TableElement *table = static_cast<TableElement *>(data->parentElement()->parentElement());
        int rowNumber = table->childElements().indexOf(data->parentElement());
        int columnNumber = data->parentElement()->childElements().indexOf(data);
        if (rows) {
            // Changing rows
            if (insert) {
                undo = new FormulaCommandReplaceRow(formulaData(), cursor(), table, rowNumber, 0, 1);
                if (undo) {
                    undo->setText(kundo2_i18n("Insert row"));
                }
            } else {
                undo = new FormulaCommandReplaceRow(formulaData(), cursor(), table, rowNumber, 1, 0);
                if (undo) {
                    undo->setText(kundo2_i18n("Remove row"));
                }
            }
        } else {
            // Changing columns
            if (insert) {
                undo = new FormulaCommandReplaceColumn(formulaData(), cursor(), table, columnNumber, 0, 1);
                if (undo) {
                    undo->setText(kundo2_i18n("Insert column"));
                }
            } else {
                undo = new FormulaCommandReplaceColumn(formulaData(), cursor(), table, columnNumber, 1, 0);
                if (undo) {
                    undo->setText(kundo2_i18n("Remove column"));
                }
            }
        }
    } else {
        return nullptr;
    }
    return undo;
}

FormulaCommand *FormulaEditor::insertElement(BasicElement *element)
{
    FormulaCommand *undo = nullptr;
    if (m_cursor.insideInferredRow()) {
        RowElement *tmprow = static_cast<RowElement *>(m_cursor.currentElement());
        QList<BasicElement *> list;
        list << element;
        if (m_cursor.hasSelection()) {
            undo = new FormulaCommandReplaceElements(tmprow, m_cursor.selection().first, m_cursor.selection().second - m_cursor.selection().first, list, true);
        } else {
            undo = new FormulaCommandReplaceElements(tmprow, m_cursor.position(), 0, list, false);
        }
    } else if (m_cursor.insideToken() && element->elementType() == Glyph) {
        // TODO: implement the insertion of glyphs
    }
    if (undo) {
        undo->setText(kundo2_i18n("Insert formula elements."));
        undo->setUndoCursorPosition(cursor());
    }
    return undo;
}

FormulaCommand *FormulaEditor::remove(bool elementBeforePosition)
{
    FormulaCommand *undo = nullptr;
    if (m_cursor.insideInferredRow()) {
        RowElement *tmprow = static_cast<RowElement *>(m_cursor.currentElement());
        if (m_cursor.isSelecting()) {
            undo = new FormulaCommandReplaceElements(tmprow,
                                                     m_cursor.selection().first,
                                                     m_cursor.selection().second - m_cursor.selection().first,
                                                     QList<BasicElement *>());
        } else {
            if (elementBeforePosition && !m_cursor.isHome()) {
                undo = new FormulaCommandReplaceElements(tmprow, m_cursor.position() - 1, 1, QList<BasicElement *>());
            } else if (!elementBeforePosition && !m_cursor.isEnd()) {
                undo = new FormulaCommandReplaceElements(tmprow, m_cursor.position(), 1, QList<BasicElement *>());
            }
        }
    } else if (m_cursor.insideToken()) {
        TokenElement *tmptoken = static_cast<TokenElement *>(m_cursor.currentElement());
        if (m_cursor.hasSelection()) {
            undo = new FormulaCommandReplaceText(tmptoken, m_cursor.selection().first, m_cursor.selection().second - m_cursor.selection().first, "");
        } else {
            if (elementBeforePosition && !m_cursor.isHome()) {
                undo = new FormulaCommandReplaceText(tmptoken, m_cursor.position() - 1, 1, "");
            } else if (!elementBeforePosition && !m_cursor.isEnd()) {
                undo = new FormulaCommandReplaceText(tmptoken, m_cursor.position(), 1, "");
            }
        }
    }
    if (undo) {
        undo->setText(kundo2_i18n("Remove formula elements"));
        undo->setUndoCursorPosition(cursor());
    }
    return undo;
}

void FormulaEditor::setData(FormulaData *data)
{
    m_data = data;
}

FormulaData *FormulaEditor::formulaData() const
{
    return m_data;
}

QString FormulaEditor::inputBuffer() const
{
    return m_inputBuffer;
}

QString FormulaEditor::tokenType(const QChar &character) const
{
    QChar::Category chat = character.category();
    if (character.isNumber()) {
        return "mn";
    } else if (chat == QChar::Punctuation_Connector || chat == QChar::Punctuation_Dash || chat == QChar::Punctuation_Open || chat == QChar::Punctuation_Close
               || chat == QChar::Punctuation_InitialQuote || chat == QChar::Punctuation_FinalQuote || chat == QChar::Symbol_Math) {
        return "mo";
    } else if (character.isLetter()) {
        return "mi";
    }
    return "mi";
}

FormulaCursor &FormulaEditor::cursor()
{
    return m_cursor;
}

void FormulaEditor::setCursor(FormulaCursor &cursor)
{
    m_cursor = cursor;
}
