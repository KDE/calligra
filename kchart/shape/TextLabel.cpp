/* This file is part of the KDE project

   Copyright 2007 Johannes Simon <johannes.simon@gmail.com>

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
 * Boston, MA 02110-1301, USA.
*/

// Local
#include "TextLabel.h"

// Qt
#include <QColor>

// KOffice
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>

using namespace KChart;

class TextLabel::Private
{
public:
    Private();
    ~Private();
    
    LabelType type;
    
    ChartShape *chart;
};

TextLabel::TextLabel( ChartShape *parent )
    : d( new Private ),
      SimpleTextShape()
{
    Q_ASSERT( parent );
    
    d->chart = parent;
    parent->addChild( this );
}

TextLabel::~TextLabel()
{
}

double TextLabel::fontSize() const
{
    return font().pointSizeF();
}

void TextLabel::setFontSize( double size )
{
    QFont oldFont = font();
    oldFont.setPointSizeF( size );
    setFont( oldFont );
}

void TextLabel::setType( LabelType type )
{
    d->type = type;
}

bool TextLabel::loadOdf( KoXmlElement &labelElement, KoShapeLoadingContext &context )
{
    // TODO: Read optional attributes
    // 1. Table range
    // 2. Position and size
    // 3. Style name
    KoXmlElement  pElement = KoXml::namedItemNS( labelElement,
                                            KoXmlNS::text, "p" );
    
    setText( pElement.text() );
    
    return true;
}

void TextLabel::saveOdf( KoXmlWriter &bodyWriter, KoGenStyles &mainStyles )
{
    switch ( d->type ) {
    case TitleLabelType:
        bodyWriter.startElement( "chart:title" );
        break;
    case SubTitleLabelType:
        bodyWriter.startElement( "chart:subtitle" );
        break;
    case FooterLabelType:
        bodyWriter.startElement( "chart:footer" );
        break;
    }
    bodyWriter.addAttributePt( "svg:x", position().x() );
    bodyWriter.addAttributePt( "svg:y", position().y() );
    // TODO: Save TextLabel color
    bodyWriter.addAttribute( "chart:style-name", saveOdfFont( mainStyles, font(), QColor() ) );
    bodyWriter.startElement( "text:p" );
    bodyWriter.addTextNode( text() );
    bodyWriter.endElement(); // text:p
    bodyWriter.endElement(); // chart:title/subtitle/footer
}
