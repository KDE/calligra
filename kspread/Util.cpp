/* This file is part of the KDE project
   Copyright 2006,2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 1998,1999 Torben Weis <weis@kde.org>

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

// Local
#include "Util.h"

#include <ctype.h>


#include <kdebug.h>

#include "Formula.h"
#include "kspread_limits.h"
#include "Localization.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "Region.h"
#include "Sheet.h"
#include "Style.h"

#include <QPen>

using namespace KSpread;


//used in Cell::encodeFormula and
//  dialogs/kspread_dlg_paperlayout.cc
int KSpread::Util::decodeColumnLabelText(const QString &labelText)
{
    int col = 0;
    const int offset = 'a' - 'A';
    int counterColumn = 0;
    const uint totalLength = labelText.length();
    uint labelTextLength;
    for (labelTextLength = 0; labelTextLength < totalLength; labelTextLength++) {
        const char c = labelText[labelTextLength].toLatin1();
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')))
            break;
    }
    if (labelTextLength == 0) {
        kWarning(36001) << "No column label text found for col:" << labelText;
        return 0;
    }
    for (uint i = 0; i < labelTextLength; i++) {
        const char c = labelText[i].toLatin1();
        counterColumn = (int) ::pow(26.0 , static_cast<int>(labelTextLength - i - 1));
        if (c >= 'A' && c <= 'Z')
            col += counterColumn * (c - 'A' + 1);  // okay here (Werner)
        else if (c >= 'a' && c <= 'z')
            col += counterColumn * (c - 'A' - offset + 1);
    }
    return col;
}

int KSpread::Util::decodeRowLabelText(const QString &labelText)
{
    QRegExp rx("([A-Za-z]+)([0-9]+)");
    if(rx.exactMatch(labelText))
        return rx.cap(2).toInt();
    return 0;
}

QString KSpread::Util::encodeColumnLabelText(int column)
{
    return Cell::columnName(column);
}

QDomElement KSpread::NativeFormat::createElement(const QString & tagName, const QFont & font, QDomDocument & doc)
{
    QDomElement e(doc.createElement(tagName));

    e.setAttribute("family", font.family());
    e.setAttribute("size", font.pointSize());
    e.setAttribute("weight", font.weight());
    if (font.bold())
        e.setAttribute("bold", "yes");
    if (font.italic())
        e.setAttribute("italic", "yes");
    if (font.underline())
        e.setAttribute("underline", "yes");
    if (font.strikeOut())
        e.setAttribute("strikeout", "yes");
    //e.setAttribute( "charset", KGlobal::charsets()->name( font ) );

    return e;
}

QDomElement KSpread::NativeFormat::createElement(const QString & tagname, const QPen & pen, QDomDocument & doc)
{
    QDomElement e(doc.createElement(tagname));
    e.setAttribute("color", pen.color().name());
    e.setAttribute("style", (int)pen.style());
    e.setAttribute("width", (int)pen.width());
    return e;
}

QFont KSpread::NativeFormat::toFont(KoXmlElement & element)
{
    bool ok;
    int size = element.attribute("size").toInt(&ok);
    if (!ok)
        return QFont();

    QFont f;
    f.setFamily(element.attribute("family"));
    if (size > 0)
        f.setPointSize(size);

    f.setWeight(element.attribute("weight").toInt(&ok));
    if (!ok)
        return QFont();

    if (element.hasAttribute("italic") && element.attribute("italic") == "yes")
        f.setItalic(true);

    if (element.hasAttribute("bold") && element.attribute("bold") == "yes")
        f.setBold(true);

    if (element.hasAttribute("underline") && element.attribute("underline") == "yes")
        f.setUnderline(true);

    if (element.hasAttribute("strikeout") && element.attribute("strikeout") == "yes")
        f.setStrikeOut(true);

    /* Uncomment when charset is added to kspread_dlg_layout
       + save a document-global charset
       if ( element.hasAttribute( "charset" ) )
         KGlobal::charsets()->setQFont( f, element.attribute("charset") );
        else
    */
    // ######## Not needed anymore in 3.0?
    //KGlobal::charsets()->setQFont( f, KGlobal::locale()->charset() );

    return f;
}

QPen KSpread::NativeFormat::toPen(KoXmlElement & element)
{
    bool ok;
    QPen p;

    p.setStyle((Qt::PenStyle)element.attribute("style").toInt(&ok));
    if (!ok)
        return QPen();

    p.setWidth(element.attribute("width").toInt(&ok));
    if (!ok)
        return QPen();

    p.setColor(QColor(element.attribute("color")));

    return p;
}

bool util_isPointValid(const QPoint& point)
{
    if (point.x() >= 0
            &&  point.y() >= 0
            &&  point.x() <= KS_colMax
            &&  point.y() <= KS_rowMax
       )
        return true;
    else
        return false;
}

bool util_isRectValid(const QRect& rect)
{
    if (util_isPointValid(rect.topLeft())
            &&  util_isPointValid(rect.bottomRight())
       )
        return true;
    else
        return false;
}


//not used anywhere
int KSpread::Util::penCompare(QPen const & pen1, QPen const & pen2)
{
    if (pen1.style() == Qt::NoPen && pen2.style() == Qt::NoPen)
        return 0;

    if (pen1.style() == Qt::NoPen)
        return -1;

    if (pen2.style() == Qt::NoPen)
        return 1;

    if (pen1.width() < pen2.width())
        return -1;

    if (pen1.width() > pen2.width())
        return 1;

    if (pen1.style() < pen2.style())
        return -1;

    if (pen1.style() > pen2.style())
        return 1;

    if (pen1.color().name() < pen2.color().name())
        return -1;

    if (pen1.color().name() > pen2.color().name())
        return 1;

    return 0;
}


QString KSpread::Odf::convertRefToBase(const QString & sheet, const QRect & rect)
{
    QPoint bottomRight(rect.bottomRight());

    QString s('$');
    s += sheet;
    s += ".$";
    s += Cell::columnName(bottomRight.x());
    s += '$';
    s += QString::number(bottomRight.y());

    return s;
}

QString KSpread::Odf::convertRefToRange(const QString & sheet, const QRect & rect)
{
    QPoint topLeft(rect.topLeft());
    QPoint bottomRight(rect.bottomRight());

    if (topLeft == bottomRight)
        return Odf::convertRefToBase(sheet, rect);

    QString s('$');
    s += sheet;
    s += ".$";
    s += /*Util::encodeColumnLabelText*/Cell::columnName(topLeft.x());
    s += '$';
    s += QString::number(topLeft.y());
    s += ":.$";
    s += /*Util::encodeColumnLabelText*/Cell::columnName(bottomRight.x());
    s += '$';
    s += QString::number(bottomRight.y());

    return s;
}

// e.g.: Sheet4.A1:Sheet4.E28
//used in Sheet::saveOdf
QString KSpread::Odf::convertRangeToRef(const QString & sheetName, const QRect & _area)
{
    return sheetName + '.' + Cell::name(_area.left(), _area.top()) + ':' + sheetName + '.' + Cell::name(_area.right(), _area.bottom());
}

QString KSpread::Odf::encodePen(const QPen & pen)
{
//     kDebug()<<"encodePen( const QPen & pen ) :"<<pen;
    // NOTE Stefan: QPen api docs:
    //              A line width of zero indicates a cosmetic pen. This means
    //              that the pen width is always drawn one pixel wide,
    //              independent of the transformation set on the painter.
    QString s = QString("%1pt ").arg((pen.width() == 0) ? 1 : pen.width());
    switch (pen.style()) {
    case Qt::NoPen:
        return "none";
    case Qt::SolidLine:
        s += "solid";
        break;
    case Qt::DashLine:
        s += "dashed";
        break;
    case Qt::DotLine:
        s += "dotted";
        break;
    case Qt::DashDotLine:
        s += "dot-dash";
        break;
    case Qt::DashDotDotLine:
        s += "dot-dot-dash";
        break;
    default: break;
    }
    //kDebug() << " encodePen :" << s;
    if (pen.color().isValid()) {
        s += ' ';
        s += Style::colorName(pen.color());
    }
    return s;
}

QPen KSpread::Odf::decodePen(const QString &border)
{
    QPen pen;
    //string like "0.088cm solid #800000"
    if (border.isEmpty() || border == "none" || border == "hidden") { // in fact no border
        pen.setStyle(Qt::NoPen);
        return pen;
    }
    //code from koborder, for the moment kspread doesn't use koborder
    // ## isn't it faster to use QStringList::split than parse it 3 times?
    QString _width = border.section(' ', 0, 0);
    QByteArray _style = border.section(' ', 1, 1).toLatin1();
    QString _color = border.section(' ', 2, 2);

    pen.setWidth((int)(KoUnit::parseValue(_width, 1.0)));

    if (_style == "none")
        pen.setStyle(Qt::NoPen);
    else if (_style == "solid")
        pen.setStyle(Qt::SolidLine);
    else if (_style == "dashed")
        pen.setStyle(Qt::DashLine);
    else if (_style == "dotted")
        pen.setStyle(Qt::DotLine);
    else if (_style == "dot-dash")
        pen.setStyle(Qt::DashDotLine);
    else if (_style == "dot-dot-dash")
        pen.setStyle(Qt::DashDotDotLine);
    else
        kDebug() << " style undefined :" << _style;

    if (_color.isEmpty())
        pen.setColor(QColor());
    else
        pen.setColor(QColor(_color));

    return pen;
}

//Return true when it's a reference to cell from sheet.
bool KSpread::Util::localReferenceAnchor(const QString &_ref)
{
    bool isLocalRef = (_ref.indexOf("http://") != 0 &&
                       _ref.indexOf("https://") != 0 &&
                       _ref.indexOf("mailto:") != 0 &&
                       _ref.indexOf("ftp://") != 0  &&
                       _ref.indexOf("file:") != 0);
    return isLocalRef;
}


QString KSpread::Odf::decodeFormula(const QString& expression, const KLocale* locale, QString namespacePrefix)
{
    // parsing state
    enum { Start, InNumber, InString, InIdentifier, InReference, InSheetName } state = Start;

    // use locale settings
    QString decimal = locale ? locale->decimalSymbol() : ".";

    QString result;
    QString reference;

    int i = 0;
    if ((!expression.isEmpty()) && (expression[0] == '=')) {
        result = '=';
        ++i;
    }

    // main loop
    while (i < expression.length()) {
        switch (state) {
        case Start: {
            // check for number
            if (expression[i].isDigit())
                state = InNumber;

            // a string?
            else if (expression[i] == '"') {
                state = InString;
                result.append(expression[i++]);
            }

            // decimal dot ?
            else if (expression[i] == '.')
                state = InNumber;

            // beginning with alphanumeric ?
            // could be identifier, cell, range, or function...
            else if (isIdentifier(expression[i]))
                state = InIdentifier;

            // [ marks sheet name for 3-d cell, e.g ['Sales Q3'.A4]
            else if (expression[i].unicode() == '[') {
                ++i;
                state = InReference;
                // NOTE Stefan: As long as KSpread does not support fixed sheets eat the dollar sign.
                if (expression[i] == '$') ++i;
            }

            // look for operator match
            else {
                int op;
                QString s;

                // check for two-chars operator, such as '<=', '>=', etc
                s.append(expression[i]);
                if (i + 1 < expression.length())
                    s.append(expression[i+1]);
                op = matchOperator(s);

                // check for one-char operator, such as '+', ';', etc
                if (op == Token::InvalidOp) {
                    s = QString(expression[i]);
                    op = matchOperator(s);
                }

                // any matched operator ?
                if (op == Token::Equal)
                    result.append("==");
                else
                    result.append(s);
                if (op != Token::InvalidOp) {
                    int len = s.length();
                    i += len;
                } else {
                    ++i;
                    state = Start;
                }
            }
            break;
        }
        case InReference: {
            if (expression[i] == ']') {
                result.append(Region::loadOdf(reference));
                reference.clear();
                state = Start;
            } else if (expression[i] == '\'') {
                reference.append('\'');
                state = InSheetName;
            } else
                reference.append(expression[i]);
            ++i;
            break;
        }
        case InSheetName: {
            reference.append(expression[i]);
            if (expression[i] == '\'') {
                // an escaped apostrophe?
                if (i + 1 < expression.count() && expression[i+1] == '\'')
                    ++i; // eat it
                else // the end
                    state = InReference;
            }
            ++i;
            break;
        }
        case InNumber: {
            // consume as long as it's digit
            if (expression[i].isDigit())
                result.append(expression[i++]);
            // convert '.' to decimal separator
            else if (expression[i] == '.') {
                result.append(decimal);
                ++i;
            }
            // exponent ?
            else if (expression[i].toUpper() == 'E') {
                result.append('E');
                ++i;
            }
            // we're done with integer number
            else
                state = Start;
            break;
        }
        case InString: {
            // consume until "
            if (expression[i] != '"')
                result.append(expression[i++]);
            // we're done
            else {
                result.append(expression[i]);
                ++i;
                state = Start;
            }
            break;
        }
        case InIdentifier: {
            // handle problematic functions
            if (expression.mid(i).startsWith("ERROR.TYPE")) {
                // replace it
                result.append("ERRORTYPE");
                i += 10; // number of characters in "ERROR.TYPE"
            } else if (expression.mid(i).startsWith("LEGACY.NORMSDIST")) {
                // replace it
                result.append("LEGACYNORMSDIST");
                i += 16; // number of characters in "LEGACY.NORMSDIST"
            } else if (expression.mid(i).startsWith("LEGACY.NORMSINV")) {
                // replace it
                result.append("LEGACYNORMSINV");
                i += 15; // number of characters in "LEGACY.NORMSINV"
            } else if (namespacePrefix == "oooc:" && expression.mid(i).startsWith("TABLE") && !isIdentifier(expression[i+5])) {
                result.append("MULTIPLE.OPERATIONS");
                i += 5;
            }


            // consume as long as alpha, dollar sign, underscore, or digit
            if (isIdentifier(expression[i])  || expression[i].isDigit())
                result.append(expression[i++]);
            // we're done
            else
                state = Start;
            break;
        }
        default:
            break;
        }
    }
    return result;
}

QString KSpread::Odf::encodeFormula(const QString& expr, const KLocale* locale)
{
    // use locale settings
    const QString decimal = locale ? locale->decimalSymbol() : ".";

    QString result('=');

    Formula formula;
    Tokens tokens = formula.scan(expr, locale);

    if (!tokens.valid() || tokens.count() == 0)
        return expr; // no altering on error

    for (int i = 0; i < tokens.count(); ++i) {
        const QString tokenText = tokens[i].text();
        const Token::Type type = tokens[i].type();

        switch (type) {
        case Token::Cell:
        case Token::Range: {
            result.append('[');
            // FIXME Stefan: Hack to get the apostrophes right. Fix and remove!
            const int pos = tokenText.lastIndexOf('!');
            if (pos != -1 && tokenText.left(pos).contains(' '))
                result.append(Region::saveOdf('\'' + tokenText.left(pos) + '\'' + tokenText.mid(pos)));
            else
                result.append(Region::saveOdf(tokenText));
            result.append(']');
            break;
        }
        case Token::Float: {
            QString tmp(tokenText);
            result.append(tmp.replace(decimal, "."));
            break;
        }
        case Token::Operator: {
            if (tokens[i].asOperator() == Token::Equal)
                result.append('=');
            else
                result.append(tokenText);
            break;
        }
        case Token::Identifier: {
            if (tokenText == "ERRORTYPE") {
                // need to replace this
                result.append("ERROR.TYPE");
            } else if (tokenText == "LEGACYNORMSDIST") {
                result.append("LEGACY.NORMSDIST");
            } else if (tokenText == "LEGACYNORMSINV") {
                result.append("LEGACY.NORMSINV");
            } else {
                // dump it out unchanged
                result.append(tokenText);
            }
            break;

        }
        case Token::Boolean:
        case Token::Integer:
        case Token::String:
        default:
            result.append(tokenText);
            break;
        }
    }
    return result;
}
