/* This file is part of the KDE project
   Copyright (C) 2003 Percy Leonhardt

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

#include "stylefactory.h"

#include <qdom.h>

#include <koUnit.h>

StyleFactory::StyleFactory()
{
    m_pageStyles.setAutoDelete( true );
    m_textStyles.setAutoDelete( true );
    m_graphicStyles.setAutoDelete( true );
    m_paragraphStyles.setAutoDelete( true );
    m_pageMasterStyles.setAutoDelete( true );

    // create standard graphic style
    GraphicStyle * graphicStyle;
    graphicStyle = new GraphicStyle ( "standard", "solid", "0cm", "0x000000",
                                      "hidden", "0.3cm", "0.3cm", "0x808080",
                                      "0cm", "0cm", "0cm", "0cm", "0x000000",
                                      "false", "none", "Thorndale", "24pt",
                                      "normal", "none", "none", "normal",
                                      "100%", "start", "solid", "0x00b8ff",
                                      "false" );

    m_graphicStyles.append( graphicStyle );
}

StyleFactory::~StyleFactory()
{
}

void StyleFactory::addOfficeStyles( QDomDocument & doc, QDomElement & styles )
{
    GraphicStyle * g;
    g = m_graphicStyles.first(); // skip the "standard" style
    g->toXML( doc, styles );
}

void StyleFactory::addOfficeMaster( QDomDocument & doc, QDomElement & master )
{
    PageMasterStyle * p;
    for ( p = m_pageMasterStyles.first(); p ; p = m_pageMasterStyles.next() )
    {
        QDomElement masterPage = doc.createElement( "style:master-page" );
        masterPage.setAttribute( "style:name", p->style() );
        masterPage.setAttribute( "style:page-master-name", p->name() );
        masterPage.setAttribute( "draw:style-name", "dp1" );
        master.appendChild( masterPage );
    }
}

void StyleFactory::addOfficeAutomatic( QDomDocument & doc, QDomElement & automatic )
{
    PageMasterStyle * p;
    for ( p = m_pageMasterStyles.first(); p ; p = m_pageMasterStyles.next() )
    {
        p->toXML( doc, automatic );
    }
}

void StyleFactory::addAutomaticStyles( QDomDocument & doc, QDomElement & autoStyles )
{
    PageStyle * p;
    for ( p = m_pageStyles.first(); p ; p = m_pageStyles.next() )
        p->toXML( doc, autoStyles );

    TextStyle * t;
    for ( t = m_textStyles.first(); t ; t = m_textStyles.next() )
        t->toXML( doc, autoStyles );

    GraphicStyle * g;
    g = m_graphicStyles.first(); // skip the "standard" style
    for ( g = m_graphicStyles.next(); g ; g = m_graphicStyles.next() )
        g->toXML( doc, autoStyles );

    ParagraphStyle * pg;
    for ( pg = m_paragraphStyles.first(); pg ; pg = m_paragraphStyles.next() )
        pg->toXML( doc, autoStyles );
}

QString StyleFactory::createPageStyle( QDomElement & e )
{
    PageStyle * newPageStyle, * p;
    newPageStyle = new PageStyle( e, m_pageStyles.count() );
    for ( p = m_pageStyles.first(); p ; p = m_pageStyles.next() )
    {
        if ( *p == *newPageStyle )
        {
            delete newPageStyle;
            return p->name();
        }
    }

    m_pageStyles.append( newPageStyle );
    return newPageStyle->name();
}

QString StyleFactory::createTextStyle( QDomElement & e )
{
    TextStyle * newTextStyle, * t;
    newTextStyle = new TextStyle( e, m_textStyles.count() );
    for ( t = m_textStyles.first(); t ; t = m_textStyles.next() )
    {
        if ( *t == *newTextStyle )
        {
            delete newTextStyle;
            return t->name();
        }
    }

    m_textStyles.append( newTextStyle );
    return newTextStyle->name();
}

QString StyleFactory::createGraphicStyle( QDomElement & e )
{
    GraphicStyle * newGraphicStyle, * g;
    newGraphicStyle = new GraphicStyle( e, m_graphicStyles.count() );
    for ( g = m_graphicStyles.first(); g ; g = m_graphicStyles.next() )
    {
        if ( *g == *newGraphicStyle )
        {
            delete newGraphicStyle;
            return g->name();
        }
    }

    m_graphicStyles.append( newGraphicStyle );
    return newGraphicStyle->name();
}

QString StyleFactory::createParagraphStyle( QDomElement & e )
{
    ParagraphStyle * newParagraphStyle, * p;
    newParagraphStyle = new ParagraphStyle( e, m_paragraphStyles.count() );
    for ( p = m_paragraphStyles.first(); p ; p = m_paragraphStyles.next() )
    {
        if ( *p == *newParagraphStyle )
        {
            delete newParagraphStyle;
            return p->name();
        }
    }

    m_paragraphStyles.append( newParagraphStyle );
    return newParagraphStyle->name();
}

QString StyleFactory::createPageMasterStyle( QDomElement & e )
{
    PageMasterStyle * newPMStyle, * p;
    newPMStyle = new PageMasterStyle( e, m_pageMasterStyles.count() );
    for ( p = m_pageMasterStyles.first(); p ; p = m_pageMasterStyles.next() )
    {
        if ( *p == *newPMStyle )
        {
            delete newPMStyle;
            return p->style();
        }
    }

    m_pageMasterStyles.append( newPMStyle );
    return newPMStyle->style();
}

QString StyleFactory::toCM( const QString & point )
{
    double pt = point.toFloat();
    double cm = KoUnit::toCM( pt );
    return QString( "%1cm" ).arg ( cm );
}

PageMasterStyle::PageMasterStyle( QDomElement & e, const uint index )
{
    QDomNode borders = e.namedItem( "PAPERBORDERS" );
    QDomElement b = borders.toElement();

    m_name = QString( "PM%1" ).arg( index );
    m_style = QString( "Default%1" ).arg( index );
    m_margin_top = StyleFactory::toCM( b.attribute( "ptTop" ) );
    m_margin_bottom = StyleFactory::toCM( b.attribute( "ptBottom" ) );
    m_margin_left = StyleFactory::toCM( b.attribute( "ptLeft" ) );
    m_margin_right = StyleFactory::toCM( b.attribute( "ptRight" ) );
    m_page_width = StyleFactory::toCM( e.attribute( "ptWidth" ) );
    m_page_height = StyleFactory::toCM( e.attribute( "ptHeight" ) );
    m_orientation = "landscape";
}

void PageMasterStyle::toXML( QDomDocument & doc, QDomElement & e ) const
{
    QDomElement style = doc.createElement( "style:page-master" );
    style.setAttribute( "style:name", "PM0" );

    QDomElement properties = doc.createElement( "style:properties" );
    properties.setAttribute( "fo:margin-top", m_margin_top );
    properties.setAttribute( "fo:margin-bottom", m_margin_bottom );
    properties.setAttribute( "fo:margin-left", m_margin_left );
    properties.setAttribute( "fo:margin-right", m_margin_right );
    properties.setAttribute( "fo:page-width", m_page_width );
    properties.setAttribute( "fo:page-height", m_page_height );
    properties.setAttribute( "fo:print-orientation", m_orientation );

    style.appendChild( properties );
    e.appendChild( style );
}

bool PageMasterStyle::operator==( const PageMasterStyle & pageMasterStyle ) const
{
    return ( m_margin_top == pageMasterStyle.m_margin_top &&
             m_margin_bottom == pageMasterStyle.m_margin_bottom &&
             m_margin_left == pageMasterStyle.m_margin_left &&
             m_margin_right == pageMasterStyle.m_margin_right &&
             m_page_width == pageMasterStyle.m_page_width &&
             m_page_height == pageMasterStyle.m_page_height &&
             m_orientation == pageMasterStyle.m_orientation );
}

PageStyle::PageStyle( QDomElement & e, const uint index )
{

}

void PageStyle::toXML( QDomDocument & doc, QDomElement & e ) const
{
    QDomElement style = doc.createElement( "style:style" );
    style.setAttribute( "style:name", m_name );
    style.setAttribute( "style:family", "drawing-page" );

    QDomElement properties = doc.createElement( "style:properties" );
    if ( m_bg_visible != QString::null )
        properties.setAttribute( "presentation:background-visible", m_bg_visible );
    if ( m_bg_objects_visible != QString::null )
        properties.setAttribute( "presentation:background-objects-visible",
                                 m_bg_objects_visible );
    if ( m_fill != QString::null )
        properties.setAttribute( "draw:fill", m_fill );
    if ( m_fill_color != QString::null )
        properties.setAttribute( "draw:fill-color", m_fill_color );
    if ( m_fill_image_name != QString::null )
        properties.setAttribute( "draw:fill-image-name", m_fill_image_name );
    if ( m_fill_image_width != QString::null )
        properties.setAttribute( "draw:fill-image-width", m_fill_image_width );
    if ( m_fill_image_height != QString::null )
        properties.setAttribute( "draw:fill-image-height", m_fill_image_height );
    if ( m_fill_image_ref_point != QString::null )
        properties.setAttribute( "draw:fill-image-ref-point", m_fill_image_ref_point );
    if ( m_fill_gradient != QString::null )
        properties.setAttribute( "draw:fill-gradient-name", m_fill_gradient );
    if ( m_repeat != QString::null )
        properties.setAttribute( "style:repeat", m_repeat );

    style.appendChild( properties );
    e.appendChild( style );
}

bool PageStyle::operator==( const PageStyle & pageStyle ) const
{
    return ( m_bg_visible == pageStyle.m_bg_visible &&
             m_bg_objects_visible == pageStyle.m_bg_objects_visible &&
             m_fill == pageStyle.m_fill &&
             m_fill_color == pageStyle.m_fill_color &&
             m_fill_image_name == pageStyle.m_fill_image_name &&
             m_fill_image_width == pageStyle.m_fill_image_width &&
             m_fill_image_height == pageStyle.m_fill_image_height &&
             m_fill_image_ref_point == pageStyle.m_fill_image_ref_point &&
             m_fill_gradient == pageStyle.m_fill_gradient &&
             m_repeat == pageStyle.m_repeat );
}

TextStyle::TextStyle( QDomElement & e, const uint index )
{

}

void TextStyle::toXML( QDomDocument & doc, QDomElement & e ) const
{
    QDomElement style = doc.createElement( "style:style" );
    style.setAttribute( "style:name", m_name );
    style.setAttribute( "style:family", "text" );

    QDomElement properties = doc.createElement( "style:properties" );
    if ( m_font_size != QString::null )
        properties.setAttribute( "fo:font-size", m_font_size );
    if ( m_font_family != QString::null )
        properties.setAttribute( "fo:font-family", m_font_family );
    if ( m_font_family_generic != QString::null )
        properties.setAttribute( "fo:font-family-generic", m_font_family_generic );
    if ( m_color != QString::null )
        properties.setAttribute( "fo:color", m_color );
    if ( m_font_pitch != QString::null )
        properties.setAttribute( "style:font-pitch", m_font_pitch );
    if ( m_font_style != QString::null )
        properties.setAttribute( "fo:font-style", m_font_style );
    if ( m_font_weight != QString::null )
        properties.setAttribute( "fo:font-weight", m_font_weight );
    if ( m_text_shadow != QString::null )
        properties.setAttribute( "fo:text-shadow", m_text_shadow );
    if ( m_text_underline != QString::null )
        properties.setAttribute( "style:text-underline", m_text_underline );
    if ( m_text_underline_color != QString::null )
        properties.setAttribute( "style:text-underline-color", m_text_underline_color );
    if ( m_text_crossing_out != QString::null )
        properties.setAttribute( "style:text-crossing-out", m_text_crossing_out );

    style.appendChild( properties );
    e.appendChild( style );
}

bool TextStyle::operator==( const TextStyle & textStyle ) const
{
    return ( m_font_size == textStyle.m_font_size &&
             m_font_family == textStyle.m_font_family &&
             m_font_family_generic == textStyle.m_font_family_generic &&
             m_color == textStyle.m_color &&
             m_font_pitch == textStyle.m_font_pitch &&
             m_font_style == textStyle.m_font_style &&
             m_font_weight == textStyle.m_font_weight &&
             m_text_shadow == textStyle.m_text_shadow &&
             m_text_underline == textStyle.m_text_underline &&
             m_text_underline_color == textStyle.m_text_underline_color &&
             m_text_crossing_out == textStyle.m_text_crossing_out );
}

GraphicStyle::GraphicStyle( QDomElement & e, const uint index )
{
    QDomNode pen = e.namedItem( "PEN" );
    QDomNode brush = e.namedItem( "BRUSH" );

    m_name = QString( "gr%1" ).arg( index );
    if ( !pen.isNull() )
    {
        QDomElement p = pen.toElement();
        m_stroke_width = StyleFactory::toCM( p.attribute( "width" ) );
        if ( m_stroke_width != "0cm" )
            m_stroke = "solid";
        m_stroke_color = p.attribute( "color" );
    }

    if ( !brush.isNull() )
    {
        QDomElement b = brush.toElement();
        m_fill_color = b.attribute( "color" );
        int style = b.attribute( "style" ).toInt();
        switch( style )
        {
        case 1:
            m_fill = "solid";
            break;
        }
    }
}

GraphicStyle::GraphicStyle( const char * name,
                            const char * stroke, const char * stroke_color,
                            const char * stroke_width, const char * shadow,
                            const char * shadow_offset_x, const char * shadow_offset_y,
                            const char * shadow_color, const char * margin_left,
                            const char * margin_right, const char * margin_top,
                            const char * margin_bottom, const char * color,
                            const char * text_outline, const char * text_crossing_out,
                            const char * font_family, const char * font_size,
                            const char * font_style, const char * text_shadow,
                            const char * text_underline, const char * font_weight,
                            const char * line_height, const char * text_align,
                            const char * fill, const char * fill_color,
                            const char * enable_numbering )
    : m_name( name )
    , m_stroke( stroke )
    , m_stroke_color( stroke_color )
    , m_stroke_width( stroke_width )
    , m_shadow( shadow )
    , m_shadow_offset_x( shadow_offset_x )
    , m_shadow_offset_y( shadow_offset_y )
    , m_shadow_color( shadow_color )
    , m_margin_left( margin_left )
    , m_margin_right( margin_right )
    , m_margin_top( margin_top )
    , m_margin_bottom( margin_bottom )
    , m_color( color )
    , m_text_outline( text_outline )
    , m_text_crossing_out( text_crossing_out )
    , m_font_family( font_family )
    , m_font_size( font_size )
    , m_font_style( font_style )
    , m_text_shadow( text_shadow )
    , m_text_underline( text_underline )
    , m_font_weight( font_weight )
    , m_line_height( line_height )
    , m_text_align( text_align )
    , m_fill( fill )
    , m_fill_color( fill_color )
    , m_enable_numbering( enable_numbering )
{
}


void GraphicStyle::toXML( QDomDocument & doc, QDomElement & e ) const
{
    QDomElement style = doc.createElement( "style:style" );
    style.setAttribute( "style:name", m_name );
    style.setAttribute( "style:family", "graphics" );
    if ( m_name != "standard" )
        style.setAttribute( "style:parent-style-name", "standard" );

    QDomElement properties = doc.createElement( "style:properties" );
    if ( m_stroke != QString::null )
        properties.setAttribute( "draw:stroke", m_stroke );
    if ( m_stroke_color != QString::null )
        properties.setAttribute( "svg:stroke-color", m_stroke_color );
    if ( m_stroke_width != QString::null )
        properties.setAttribute( "svg:stroke-width", m_stroke_width );
    if ( m_shadow != QString::null )
        properties.setAttribute( "draw:shadow", m_shadow );
    if ( m_shadow_offset_x != QString::null )
        properties.setAttribute( "draw:shadow-offset-x", m_shadow_offset_x );
    if ( m_shadow_offset_y != QString::null )
        properties.setAttribute( "draw:shadow-offset-y", m_shadow_offset_y );
    if ( m_shadow_color != QString::null )
        properties.setAttribute( "draw:shadow-color", m_shadow_color );
    if ( m_margin_left != QString::null )
        properties.setAttribute( "fo:margin-left", m_margin_left );
    if ( m_margin_right != QString::null )
        properties.setAttribute( "fo:margin-right", m_margin_right );
    if ( m_margin_top != QString::null )
        properties.setAttribute( "fo:margin-top", m_margin_top );
    if ( m_margin_bottom != QString::null )
        properties.setAttribute( "fo:margin-bottom", m_margin_bottom );
    if ( m_color != QString::null )
        properties.setAttribute( "fo:color", m_color );
    if ( m_text_outline != QString::null )
        properties.setAttribute( "style:text-outline", m_text_outline );
    if ( m_text_crossing_out != QString::null )
        properties.setAttribute( "style:text-crossing-out", m_text_crossing_out );
    if ( m_font_family != QString::null )
        properties.setAttribute( "fo:font-family", m_font_family );
    if ( m_font_size != QString::null )
        properties.setAttribute( "fo:font-size", m_font_size );
    if ( m_font_style != QString::null )
        properties.setAttribute( "fo:font-style", m_font_style );
    if ( m_text_shadow != QString::null )
        properties.setAttribute( "fo:text-shadow", m_text_shadow );
    if ( m_text_underline != QString::null )
        properties.setAttribute( "style:text-underline", m_text_underline );
    if ( m_font_weight != QString::null )
        properties.setAttribute( "fo:font-weight", m_font_weight );
    if ( m_line_height != QString::null )
        properties.setAttribute( "fo:line-height", m_line_height );
    if ( m_text_align != QString::null )
        properties.setAttribute( "fo:text-align", m_text_align );
    if ( m_fill != QString::null )
        properties.setAttribute( "draw:fill", m_fill );
    if ( m_fill_color != QString::null )
        properties.setAttribute( "draw:fill-color", m_fill_color );
    if ( m_enable_numbering != QString::null )
        properties.setAttribute( "text:enable-numbering", m_enable_numbering );

    style.appendChild( properties );
    e.appendChild( style );
}

bool GraphicStyle::operator==( const GraphicStyle & graphicStyle ) const
{
    return ( m_stroke == graphicStyle.m_stroke &&
             m_stroke_color == graphicStyle.m_stroke_color &&
             m_stroke_width == graphicStyle.m_stroke_width &&
             m_shadow == graphicStyle.m_shadow &&
             m_shadow_offset_x == graphicStyle.m_shadow_offset_x &&
             m_shadow_offset_y == graphicStyle.m_shadow_offset_y &&
             m_shadow_color == graphicStyle.m_shadow_color &&
             m_margin_left == graphicStyle.m_margin_left &&
             m_margin_right == graphicStyle.m_margin_right &&
             m_margin_top == graphicStyle.m_margin_top &&
             m_margin_bottom == graphicStyle.m_margin_bottom &&
             m_color == graphicStyle.m_color &&
             m_text_outline == graphicStyle.m_text_outline &&
             m_text_crossing_out == graphicStyle.m_text_crossing_out &&
             m_font_family == graphicStyle.m_font_family &&
             m_font_size == graphicStyle.m_font_size &&
             m_font_style == graphicStyle.m_font_style &&
             m_text_shadow == graphicStyle.m_text_shadow &&
             m_text_underline == graphicStyle.m_text_underline &&
             m_font_weight == graphicStyle.m_font_weight &&
             m_line_height == graphicStyle.m_line_height &&
             m_text_align == graphicStyle.m_text_align &&
             m_fill == graphicStyle.m_fill &&
             m_fill_color == graphicStyle.m_fill_color &&
             m_enable_numbering == graphicStyle.m_enable_numbering );
}

ParagraphStyle::ParagraphStyle( QDomElement & e, const uint index )
{

}

void ParagraphStyle::toXML( QDomDocument & doc, QDomElement & e ) const
{
    QDomElement style = doc.createElement( "style:style" );
    style.setAttribute( "style:name", m_name );
    style.setAttribute( "style:family", "paragraph" );

    QDomElement properties = doc.createElement( "style:properties" );
    if ( m_margin_left != QString::null )
        properties.setAttribute( "fo:margin-left", m_margin_left );
    if ( m_margin_right != QString::null )
        properties.setAttribute( "fo:margin-right", m_margin_right );
    if ( m_text_indent != QString::null )
        properties.setAttribute( "fo:text-indent", m_text_indent );
    if ( m_text_align != QString::null )
        properties.setAttribute( "fo:text-align", m_text_align );
    if ( m_enable_numbering != QString::null )
        properties.setAttribute( "text:enable-numbering", m_enable_numbering );

    style.appendChild( properties );
    e.appendChild( style );
}

bool ParagraphStyle::operator==( const ParagraphStyle & paragraphStyle ) const
{
    return ( m_margin_left == paragraphStyle.m_margin_left &&
             m_margin_right == paragraphStyle.m_margin_right &&
             m_text_indent == paragraphStyle.m_text_indent &&
             m_text_align == paragraphStyle.m_text_align &&
             m_enable_numbering == paragraphStyle.m_enable_numbering );
}

