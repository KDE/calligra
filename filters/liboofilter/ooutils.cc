/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
   Copyright (c) 2003 Lukas Tinkl <lukas@kde.org>
   Copyright (c) 2003 David Faure <faure@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "ooutils.h"
#include "stylestack.h"
#include <qdom.h>
#include <qcolor.h>
#include <koUnit.h>
#include <qregexp.h>
#include <kdebug.h>

QString OoUtils::expandWhitespace(const QDomElement& tag)
{
    //tags like <text:s text:c="4">

    int howmany=1;
    if (tag.hasAttribute("text:c"))
        howmany = tag.attribute("text:c").toInt();

    QString result;
    return result.fill(32, howmany);
}

bool OoUtils::parseBorder(const QString & tag, double * width, int * style, QColor * color)
{
    //string like "0.088cm solid #800000"

    if (tag.isEmpty() || tag=="none" || tag=="hidden") // in fact no border
        return false;

    QString _width = tag.section(' ', 0, 0);
    QString _style = tag.section(' ', 1, 1);
    QString _color = tag.section(' ', 2, 2);

    *width = KoUnit::parseValue(_width, 1.0);

    if (_style=="double")
        *style = 5;
    else
        *style = 0;

    if (_color.isEmpty())
        *color = QColor();
    else
        color->setNamedColor(_color);

    return true;
}

void OoUtils::importIndents( QDomElement& parentElement, const StyleStack& styleStack )
{
    if ( styleStack.hasAttribute( "fo:margin-left" ) || // 3.11.19
         styleStack.hasAttribute( "fo:margin-right" ) )
         // *text-indent must always be bound to either margin-left or margin-right
    {
        double marginLeft = KoUnit::parseValue( styleStack.attribute( "fo:margin-left" ) );
        double marginRight = KoUnit::parseValue( styleStack.attribute( "fo:margin-right" ) );
        double first = 0;
        if (styleStack.hasAttribute("style:auto-text-indent")) // style:auto-text-indent takes precedence
            // ### It has another meaning, no?
            first = KoUnit::parseValue( styleStack.attribute("style:auto-text-indent"));
        else if (styleStack.hasAttribute("fo:text-indent"))
            first = KoUnit::parseValue( styleStack.attribute("fo:text-indent"));

        if ( marginLeft != 0 || marginRight != 0 || first != 0 )
        {
            QDomElement indent = parentElement.ownerDocument().createElement( "INDENTS" );
            if( marginLeft != 0 )
                indent.setAttribute( "left", marginLeft );
            if( marginRight != 0 )
                indent.setAttribute( "right", marginLeft );
            if( first != 0 )
                indent.setAttribute( "first", first );
            parentElement.appendChild( indent );
        }
    }
}

void OoUtils::importLineSpacing( QDomElement& parentElement, const StyleStack& styleStack )
{
    if( styleStack.hasAttribute("fo:line-height") )
    {
        // Fixed line height
        QString value = styleStack.attribute( "fo:line-height" ); // 3.11.1
        if ( value != "normal" )
        {
            QDomElement lineSpacing = parentElement.ownerDocument().createElement( "LINESPACING" );
            if ( value == "100%" )
                lineSpacing.setAttribute("type","single");
            else if( value=="150%")
                lineSpacing.setAttribute("type","oneandhalf");
            else if( value=="200%")
                lineSpacing.setAttribute("type","double");
            else if ( value.find('%') > -1 )
            {
                double percent = value.toDouble();
                lineSpacing.setAttribute("type", "multiple");
                lineSpacing.setAttribute("spacingvalue", percent/100);
            }
            else // fixed value (use KoUnit::parseValue to get it in pt)
            {
                kdWarning() << "Unhandled value for fo:line-height: " << value << endl;
            }
            parentElement.appendChild( lineSpacing );
        }
    }
    // Line-height-at-least is mutually exclusive with line-height
    else if ( styleStack.hasAttribute("style:line-height-at-least") ) // 3.11.2
    {
        QString value = styleStack.attribute( "style:line-height-at-least" );
        // kotext has "at least" but that's for the linespacing, not for the entire line height!
        // Strange. kotext also has "at least" for the whole line height....
        // Did we make the wrong choice in kotext?
        //kdWarning() << "Unimplemented support for style:line-height-at-least: " << value << endl;
        // Well let's see if this makes a big difference.
        QDomElement lineSpacing = parentElement.ownerDocument().createElement("LINESPACING");
        lineSpacing.setAttribute("type", "at least");
        lineSpacing.setAttribute("spacingvalue", KoUnit::parseValue(value));
        parentElement.appendChild(lineSpacing);
    }
    // Line-spacing is mutually exclusive with line-height and line-height-at-least
    else if ( styleStack.hasAttribute("style:line-spacing") ) // 3.11.3
    {
        double value = KoUnit::parseValue( styleStack.attribute( "style:line-spacing" ) );
        if ( value != 0.0 )
        {
            QDomElement lineSpacing = parentElement.ownerDocument().createElement( "LINESPACING" );
            lineSpacing.setAttribute( "type", "custom" );
            lineSpacing.setAttribute( "spacingvalue", value );
            parentElement.appendChild( lineSpacing );
        }
    }

}

void OoUtils::importTopBottomMargin( QDomElement& parentElement, const StyleStack& styleStack )
{
    if( styleStack.hasAttribute("fo:margin-top") || // 3.11.22
        styleStack.hasAttribute("fo:margin-bottom"))
    {
        double mtop = KoUnit::parseValue( styleStack.attribute( "fo:margin-top" ) );
        double mbottom = KoUnit::parseValue( styleStack.attribute("fo:margin-bottom" ) );
        if( mtop != 0 || mbottom != 0 )
        {
            QDomElement offset = parentElement.ownerDocument().createElement( "OFFSETS" );
            if( mtop != 0 )
                offset.setAttribute( "before", mtop );
            if( mbottom != 0 )
                offset.setAttribute( "after", mbottom );
            parentElement.appendChild( offset );
        }
    }
}

void OoUtils::importTabulators( QDomElement& parentElement, const StyleStack& styleStack )
{
    if ( !styleStack.hasChildNode( "style:tab-stops" ) ) // 3.11.10
        return;
    QDomElement tabStops = styleStack.childNode( "style:tab-stops" ).toElement();
    //kdDebug() << k_funcinfo << tabStops.childNodes().count() << " tab stops in layout." << endl;
    for ( QDomNode it = tabStops.firstChild(); !it.isNull(); it = it.nextSibling() )
    {
        QDomElement tabStop = it.toElement();
        Q_ASSERT( tabStop.tagName() == "style:tab-stop" );
        QString type = tabStop.attribute( "style:type" ); // left, right, center or char

        QDomElement elem = parentElement.ownerDocument().createElement( "TABULATOR" );
        int kOfficeType = 0;
        if ( type == "left" )
            kOfficeType = 0;
        else if ( type == "center" )
            kOfficeType = 1;
        else if ( type == "right" )
            kOfficeType = 2;
        else if ( type == "char" ) {
            QString delimiterChar = tabStop.attribute( "style:char" ); // single character
            elem.setAttribute( "alignchar", delimiterChar );
            kOfficeType = 3; // "alignment on decimal point"
        }

        elem.setAttribute( "type", kOfficeType );

        double pos = KoUnit::parseValue( tabStop.attribute( "style:position" ) );
        elem.setAttribute( "ptpos", pos );

        // TODO Convert leaderChar's unicode value to the KOffice enum
        // (blank/dots/line/dash/dash-dot/dash-dot-dot, 0 to 5)
        QString leaderChar = tabStop.attribute( "style:leader-char" ); // single character
        if ( !leaderChar.isEmpty() )
        {
            int filling = 0;
            QChar ch = leaderChar[0];
            switch (ch.latin1()) {
            case '.':
                filling = 1; break;
            case '-':
            case '_':  // TODO in KWord: differentiate --- and ___
                filling = 2; break;
            default:
                // KWord doesn't have support for "any char" as filling.
                // Instead it has dash-dot and dash-dot-dot - but who uses that in a tabstop?
                break;
            }
            elem.setAttribute( "filling", filling );
        }
        parentElement.appendChild( elem );
    }

}

void OoUtils::importBorders( QDomElement& parentElement, const StyleStack& styleStack )
{
    // First case, we set all the borders at once
    if ( styleStack.hasAttribute( "fo:border" ) )
    {
        double width;
        int style;
        QColor color;
        if (OoUtils::parseBorder(styleStack.attribute("fo:border"), &width, &style, &color))
        {
            QDomElement lbElem = parentElement.ownerDocument().createElement("LEFTBORDER");
            lbElem.setAttribute("width", width);
            lbElem.setAttribute("style", style);
            if (color.isValid()) {
                lbElem.setAttribute("red", color.red());
                lbElem.setAttribute("green", color.green());
                lbElem.setAttribute("blue", color.blue());
            }
            parentElement.appendChild(lbElem);

            QDomElement rbElem = parentElement.ownerDocument().createElement("RIGHTBORDER");
            rbElem = lbElem.cloneNode(false).toElement();
            parentElement.appendChild(rbElem);

            QDomElement tbElem = parentElement.ownerDocument().createElement("TOPBORDER");
            tbElem = lbElem.cloneNode(false).toElement();
            parentElement.appendChild(tbElem);

            QDomElement bbElem = parentElement.ownerDocument().createElement("BOTTOMBORDER");
            bbElem = lbElem.cloneNode(false).toElement();
            parentElement.appendChild(bbElem);
        }
    }
         // Second case: we set each border independently
         else if ( styleStack.hasAttribute( "fo:border-left" )
                   || styleStack.hasAttribute( "fo:border-right" )
                   || styleStack.hasAttribute( "fo:border-top" )
                   || styleStack.hasAttribute( "fo:border-bottom" ) )
    {
        if (styleStack.hasAttribute("fo:border-left"))
        {
            double width;
            int style;
            QColor color;
            if (OoUtils::parseBorder(styleStack.attribute("fo:border-left"), &width, &style, &color))
            {
                QDomElement lbElem = parentElement.ownerDocument().createElement("LEFTBORDER");
                lbElem.setAttribute("width", width);
                lbElem.setAttribute("style", style);
                if (color.isValid()) {
                    lbElem.setAttribute("red", color.red());
                    lbElem.setAttribute("green", color.green());
                    lbElem.setAttribute("blue", color.blue());
                }
                parentElement.appendChild(lbElem);
            }
        }

        if (styleStack.hasAttribute("fo:border-right"))
        {
            double width;
            int style;
            QColor color;
            if (OoUtils::parseBorder(styleStack.attribute("fo:border-right"), &width, &style, &color))
            {
                QDomElement lbElem = parentElement.ownerDocument().createElement("RIGHTBORDER");
                lbElem.setAttribute("width", width);
                lbElem.setAttribute("style", style);
                if (color.isValid()) {
                    lbElem.setAttribute("red", color.red());
                    lbElem.setAttribute("green", color.green());
                    lbElem.setAttribute("blue", color.blue());
                }
                parentElement.appendChild(lbElem);
            }
        }

        if (styleStack.hasAttribute("fo:border-top"))
        {
            double width;
            int style;
            QColor color;
            if (OoUtils::parseBorder(styleStack.attribute("fo:border-top"), &width, &style, &color))
            {
                QDomElement lbElem = parentElement.ownerDocument().createElement("TOPBORDER");
                lbElem.setAttribute("width", width);
                lbElem.setAttribute("style", style);
                if (color.isValid()) {
                    lbElem.setAttribute("red", color.red());
                    lbElem.setAttribute("green", color.green());
                    lbElem.setAttribute("blue", color.blue());
                }
                parentElement.appendChild(lbElem);
            }
        }

        if (styleStack.hasAttribute("fo:border-bottom"))
        {
            double width;
            int style;
            QColor color;
            if (OoUtils::parseBorder(styleStack.attribute("fo:border-bottom"), &width, &style, &color))
            {
                QDomElement lbElem = parentElement.ownerDocument().createElement("BOTTOMBORDER");
                lbElem.setAttribute("width", width);
                lbElem.setAttribute("style", style);
                if (color.isValid()) {
                    lbElem.setAttribute("red", color.red());
                    lbElem.setAttribute("green", color.green());
                    lbElem.setAttribute("blue", color.blue());
                }
                parentElement.appendChild(lbElem);
            }
        }
    }
}

// TODO use this in OoImpressImport!
void OoUtils::importUnderline( const QString& in, QString& underline, QString& styleline )
{
    underline = "single";
    if ( in == "none" )
        underline = "0";
    else if ( in == "single" )
        styleline = "solid";
    else if ( in == "double" )
    {
        underline = in;
        styleline = "solid";
    }
    else if ( in == "dotted" || in == "bold-dotted" ) // bold-dotted not in libkotext
        styleline = "dot";
    else if ( in == "dash"
              // those are not in libkotext:
              || in == "long-dash"
              || in == "bold-dash"
              || in == "bold-long-dash"
        )
        styleline = "dash";
    else if ( in == "dot-dash"
              || in == "bold-dot-dash") // not in libkotext
        styleline = "dashdot"; // tricky ;)
    else if ( in == "dot-dot-dash"
              || in == "bold-dot-dot-dash") // not in libkotext
        styleline = "dashdotdot"; // this is getting fun...
    else if ( in == "wave"
              || in == "bold-wave" // not in libkotext
              || in == "double-wave" // not in libkotext
              || in == "small-wave" ) // not in libkotext
    {
        underline = in;
        styleline = "solid";
    }
    else if( in == "bold" )
    {
        underline = "single-bold";
        styleline = "solid";
    }
    else
        kdWarning() << k_funcinfo << " unsupported text-underline value: " << in << endl;
}
