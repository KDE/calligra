/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "OperatorElement.h"

#include "AttributeManager.h"
#include "FormulaDebug.h"
#include "RowElement.h"

#include <QFontMetricsF>

OperatorElement::OperatorElement(BasicElement *parent)
    : TokenElement(parent)
{
}

QPainterPath OperatorElement::renderForFence(const QString &raw, Form form)
{
    Q_UNUSED(raw)
    Q_UNUSED(form)
    return QPainterPath();
}

QRectF OperatorElement::renderToPath(const QString &raw, QPainterPath &path) const
{
    AttributeManager manager;

    qreal rSpace = manager.parseMathSpace(m_dict.rSpace(), this);
    qreal lSpace = manager.parseMathSpace(m_dict.lSpace(), this);
    path.moveTo(path.currentPosition() + QPointF(lSpace, 0.0));
    QFont font = manager.font(this);
    path.addText(path.currentPosition(), font, raw);
    QRectF rect = path.boundingRect().adjusted(0, 0, lSpace + rSpace, 0);
    return rect;
    //    QFontMetricsF fm(font);
    //    return fm.boundingRect(QRect(), Qt::TextIncludeTrailingSpaces, raw).adjusted(0,0,lSpace+rSpace,0).adjusted(0,-fm.ascent(), 0, -fm.ascent());
}

/* stretching rules are:
 * If a stretchy operator or fence is a direct subexpression of an <MROW>, or is the sole direct subexpression of an <MTD> in an <MTR>, then it should stretch
 to cover the height and depth (above and below the axis) of the non-stretchy subexpressions in the <MROW> or <MTR> element, given the constraints mentioned
 above. This applies even if the <MTD> and/or <MTR> were inferred, as described in the <MTABLE> section.

 * If a stretchy operator is a direct subexpression of an <MUNDER>, <MOVER>, <MUNDEROVER>, or if it is the sole direct subexpression of an <MTD>, then it should
 stretch to cover the width of the other subexpressions in the given element (or in the same matrix column, in the case of a matrix), given the constraints
 mentioned above. This applies even if the <MTD> was inferred, as described in the <MTABLE> section.
 */
void OperatorElement::stretch()
{
    m_stretchVertically = m_stretchHorizontally = false;

    if (!parentElement())
        return;

    if (!m_dict.stretchy())
        return;

    switch (parentElement()->elementType()) {
    case TableData: // MTD
        if (parentElement()->childElements().count() == 1) {
            m_stretchHorizontally = true;
            if (parentElement()->parentElement() && parentElement()->parentElement()->elementType() == TableRow)
                m_stretchVertically = true;
        }
        break;
    case Under:
    case Over:
    case UnderOver:
        m_stretchHorizontally = true;
        break;
    default:
        // There are many element types that inherit Row, so just try casting to
        // a row to see if it inherits from a row
        if (dynamic_cast<RowElement *>(parentElement()) != nullptr)
            m_stretchVertically = true;
        else
            return;
        break;
    }
    if (m_stretchVertically) {
        // FIXME - take into account maximum stretch size
        qreal newHeight = parentElement()->childrenBoundingRect().height();
        qreal newBaseLine = baseLine() * newHeight / height();
        setBaseLine(newBaseLine);
        setHeight(newHeight);
    }
    if (m_stretchHorizontally) {
        setWidth(parentElement()->width());
    }
}

Form OperatorElement::determineOperatorForm() const
{
    // a bit of a hack - determine the operator's form with its position inside the
    // parent's element list. This is with the assumption that the parent is an
    // ( inferred ) row element. If that is not the case return standard Prefix ( ? )

    if (dynamic_cast<RowElement *>(parentElement()) == nullptr)
        return Prefix;
    if (parentElement()->childElements().isEmpty())
        return Prefix;
    if (parentElement()->childElements().first() == this)
        return Prefix;
    if (parentElement()->childElements().last() == this)
        return Postfix;
    return Infix;
}

Form OperatorElement::parseForm(const QString &value) const
{
    if (value == "prefix")
        return Prefix;
    else if (value == "infix")
        return Infix;
    else if (value == "postfix")
        return Postfix;
    else
        return InvalidForm;
}

ElementType OperatorElement::elementType() const
{
    return Operator;
}

bool OperatorElement::insertText(int position, const QString &text)
{
    if (m_rawString.isEmpty()) {
        m_dict.queryOperator(text, determineOperatorForm());
    }
    return TokenElement::insertText(position, text);
}

bool OperatorElement::readMathMLContent(const KoXmlElement &parent)
{
    bool tmp = TokenElement::readMathMLContent(parent);
    m_dict.queryOperator(m_rawString, determineOperatorForm());
    return tmp;
}
