/* This file is part of the KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KFormula13ContentHandler.h"


KFormula13ContentHandler::KFormula13ContentHandler()
        : QXmlDefaultHandler()
{
}

KFormula13ContentHandler::~KFormula13ContentHandler()
{
}

bool KFormula13ContentHandler::startDocument()
{

    QDomDocumentType dt = QDomImplementation().createDocumentType("math",
                          "-//W3C//DTD MathML 2.0//EN",
                          "http://www.w3.org/TR/MathML2/dtd/mathml2.dtd");
    m_mathMlDocument = QDomDocument(dt);
    QDomProcessingInstruction in = m_mathMlDocument.createProcessingInstruction("xml",
                                   "version=\"1.0\" encoding=\"UTF-8\"");
    m_mathMlDocument.insertBefore(in, m_mathMlDocument.documentElement());
    m_currentElement = m_mathMlDocument.documentElement();

    return true;
}

bool KFormula13ContentHandler::startElement(const QString&,
        const QString& localName,
        const QString&,
        const QXmlAttributes& atts)
{
    if (localName == "CONTENT" || localName == "FORMULASETTINGS" ||
            localName == "FORMULA" || localName == "DENOMINATOR" ||
            localName == "NUMERATOR")
        return true;

    parseMatrix();

    QDomElement element;
    m_currentElement.appendChild(element);
    m_currentElement = element;

    if (localName == "SEQUENCE")
        m_currentElement.setTagName("mrow");
    else if (localName == "BRACKET") {
        m_currentElement.setTagName("mfenced");
        writeBracketAttributes(atts);
    } else if (localName == "SPACE") {
        m_currentElement.setTagName("mspace");
        writeSpaceAttributes(atts);
    } else if (localName == "OVERLINE") {
        m_currentElement.setTagName("mover");
        QDomElement tmp;
        tmp.setTagName("mo");
        tmp.setNodeValue("&OverBar;");
        m_currentElement.parentNode().appendChild(tmp);
    } else if (localName == "UNDERLINE") {
        m_currentElement.setTagName("munder");
        QDomElement tmp;
        tmp.setTagName("mo");
        tmp.setNodeValue("&UnderBar;");
        m_currentElement.parentNode().appendChild(tmp);
    } else if (localName == "FRACTION") {
        m_currentElement.setTagName("mfrac");
        if (atts.value("NOLINE").toInt() == 1)
            m_currentElement.setAttribute("linethickness", "0");
    } else if (localName == "ROOT")
        m_currentElement.setTagName("msqrt");
    else if (localName == "ROOTINDEX") {
        m_currentElement.setTagName("mrow");
        m_currentElement.parentNode().toElement().setTagName("mroot");
    } else if (localName == "MATRIX") {
        m_currentElement.setTagName("mtable");
        int rows = atts.value("ROWS").toInt();
        int cols = atts.value("COLUMNS").toInt();
        m_matrixStack.push(QPair<int, int>(rows, cols));
    } else if (localName == "MULTILINE")
        m_currentElement.setTagName("mtext");
    else if (localName == "TEXT") {
        //    m_currentElement.
    }

    return true;
}

bool KFormula13ContentHandler::endElement(const QString&,
        const QString& localName,
        const QString&)
{
    if (localName == "CONTENT" || localName == "FORMULASETTINGS" ||
            localName == "FORMULA" || localName == "DENOMINATOR" ||
            localName == "NUMERATOR" || localName == "TEXT")
        return true;

    if (localName == "MATRIX")     // a matrix has been completely parsed
        m_matrixStack.pop();

    if (localName == "SEQUENCE" && m_currentElement.tagName() == "mtext" &&
            m_currentElement.parentNode().childNodes().count() == 1) {
        QDomNode parent = m_currentElement.parentNode();
        parent.parentNode().appendChild(m_currentElement);
        m_currentElement.parentNode().removeChild(parent);
    }

    m_currentElement = m_currentElement.parentNode().toElement();

    if (m_currentElement.tagName() == "mtd")  // move up to trigger mtr in parseMatrix()
        m_currentElement = m_currentElement.parentNode().toElement();

    return true;
}

void KFormula13ContentHandler::writeBracketAttributes(const QXmlAttributes& atts)
{
    switch (atts.value("LEFT").toInt()) {
    case 91:
        m_currentElement.setAttribute("open", "[");
        break;
    case 123:
        m_currentElement.setAttribute("open", "{");
        break;
    case 256:
        m_currentElement.setAttribute("open", "|");
        break;
    }

    switch (atts.value("RIGHT").toInt()) {
    case 93:
        m_currentElement.setAttribute("close", "]");
        break;
    case 125:
        m_currentElement.setAttribute("close", "}");
        break;
    case 257:
        m_currentElement.setAttribute("close", "|");
        break;
    }
}

void KFormula13ContentHandler::writeSpaceAttributes(const QXmlAttributes& atts)
{
    switch (atts.value("WIDTH")) {
    case "thin":
        m_currentElement.setAttribute("width", "thinmathspace");
        break;
    case "medium":
        m_currentElement.setAttribute("width", "mediummathspace");
        break;
    case "thick":
        m_currentElement.setAttribute("width", "thickmathspace");
        break;
    case "quad":
        m_currentElement.setAttribute("width", "veryverythickmathspace");
        break;
    }
}

void KFormula13ContentHandler::parseMatrix()
{
    if (m_matrixStack.isEmpty())        // if there is no matrix, return
        return;

    QDomElement element;
    int count = m_currentElement.childNodes().count();
    if (m_currentElement.tagName() == "mtable" && count < m_matrixStack.top().first) {  // check count < rowCount thought there should never be count == rowCount
        element.setTagName("mtr");
        m_currentElement.appendChild(element);
        m_currentElement = element;
    }

    if (m_currentElement.tagName() == "mtr" && count < m_matrixStack.top().second) {  // we need a new table entry so create one and set it as m_currentElement
        element.setTagName("mtd");
        m_currentElement.appendChild(element);
        m_currentElement = element;
    } else if (m_currentElement.tagName() == "mtr" && count == m_matrixStack.top().second) {  // the matrix has been parsed or the row is full and we need a new mtr,
        m_currentElement = m_currentElement.parentNode().toElement();  // move up
        parseMatrix();         // and reparse matrix with m_currentElement == mtable
    }
}

