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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "stylefactory.h"

#include <QColor>
#include <qdatetime.h>
#include <KoUnit.h>
#include <kdebug.h>

StyleFactory::StyleFactory()
{
    m_strokeDashStyles.setAutoDelete( true );
    m_gradientStyles.setAutoDelete( true );
    m_hatchStyles.setAutoDelete( true );
    m_markerStyles.setAutoDelete( true );
    m_fillImageStyles.setAutoDelete( true );
    m_listStyles.setAutoDelete( true );
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
    StrokeDashStyle * sd;
    for ( sd = m_strokeDashStyles.first(); sd ; sd = m_strokeDashStyles.next() )
        sd->toXML( doc, styles );

    GradientStyle * g;
    for ( g = m_gradientStyles.first(); g ; g = m_gradientStyles.next() )
        g->toXML( doc, styles );

    MarkerStyle * m;
    for ( m = m_markerStyles.first(); m ; m = m_markerStyles.next() )
        m->toXML( doc, styles );

    HatchStyle * h;
    for ( h = m_hatchStyles.first(); h ; h = m_hatchStyles.next() )
        h->toXML( doc, styles );

    GraphicStyle * gr;
    gr = m_graphicStyles.first(); // skip the "standard" style
    gr->toXML( doc, styles );
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
    ListStyle * l;
    for ( l = m_listStyles.first(); l ; l = m_listStyles.next() )
        l->toXML( doc, autoStyles );

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

QString StyleFactory::createStrokeDashStyle( int style )
{
    StrokeDashStyle * newStrokeDashStyle, * sd;
    newStrokeDashStyle = new StrokeDashStyle( style );
    for ( sd = m_strokeDashStyles.first(); sd ; sd = m_strokeDashStyles.next() )
    {
        if ( sd->name() == newStrokeDashStyle->name() )
        {
            delete newStrokeDashStyle;
            return sd->name();
        }
    }

    m_strokeDashStyles.append( newStrokeDashStyle );
    return newStrokeDashStyle->name();
}

QString StyleFactory::createGradientStyle( QDomElement & gradient )
{
    GradientStyle * newGradientStyle, * g;
    newGradientStyle = new GradientStyle( gradient, m_gradientStyles.count() + 1 );
    for ( g = m_gradientStyles.first(); g ; g = m_gradientStyles.next() )
    {
        if ( g->name() == newGradientStyle->name() )
        {
            delete newGradientStyle;
            return g->name();
        }
    }

    m_gradientStyles.append( newGradientStyle );
    return newGradientStyle->name();
}

QString StyleFactory::createMarkerStyle( int style )
{
    MarkerStyle * newMarkerStyle, * m;
    newMarkerStyle = new MarkerStyle( style );
    for ( m = m_markerStyles.first(); m ; m = m_markerStyles.next() )
    {
        if ( m->name() == newMarkerStyle->name() )
        {
            delete newMarkerStyle;
            return m->name();
        }
    }

    m_markerStyles.append( newMarkerStyle );
    return newMarkerStyle->name();
}

QString StyleFactory::createHatchStyle( int style, QString & color )
{
    HatchStyle * newHatchStyle, * h;
    newHatchStyle = new HatchStyle( style, color );
    for ( h = m_hatchStyles.first(); h ; h = m_hatchStyles.next() )
    {
        if ( h->name() == newHatchStyle->name() )
        {
            delete newHatchStyle;
            return h->name();
        }
    }

    m_hatchStyles.append( newHatchStyle );
    return newHatchStyle->name();
}

QString StyleFactory::createListStyle( QDomElement & e )
{
    ListStyle * newListStyle, * l;
    newListStyle = new ListStyle( e, m_listStyles.count() + 1 );
    for ( l = m_listStyles.first(); l ; l = m_listStyles.next() )
    {
        if ( *l == *newListStyle )
        {
            delete newListStyle;
            return l->name();
        }
    }

    m_listStyles.append( newListStyle );
    return newListStyle->name();
}

QString StyleFactory::createPageStyle( QDomElement & e )
{
    PageStyle * newPageStyle, * p;
    newPageStyle = new PageStyle( this, e, m_pageStyles.count() + 1 );
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
    newTextStyle = new TextStyle( e, m_textStyles.count() + 1 );
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
    newGraphicStyle = new GraphicStyle( this, e, m_graphicStyles.count() );
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
    newParagraphStyle = new ParagraphStyle( e, m_paragraphStyles.count() + 1 );
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

StrokeDashStyle::StrokeDashStyle( int style )
{
    switch ( style )
    {
    case 2:
        m_name = "Fine Dashed";
        m_style = "rect";
        m_dots1 = "1";
        m_dots1_length = "0.508cm";
        m_dots2 = "1";
        m_dots2_length = "0.508cm";
        m_distance = "0.508cm";
        break;
    case 3:
        m_name = "Fine Dotted";
        m_style = "rect";
        m_dots1 = "1";
        m_distance = "0.257cm";
        break;
    case 4:
        m_name = "Ultrafine 1 Dot 1 Dash";
        m_style = "rect";
        m_dots1 = "1";
        m_dots1_length = "0.051cm";
        m_dots2 = "1";
        m_dots2_length = "0.254cm";
        m_distance = "0.127cm";
        break;
    case 5:
        m_name = "2 Dots 1 Dash";
        m_style = "rect";
        m_dots1 = "2";
        m_dots2 = "1";
        m_dots2_length = "0.203cm";
        m_distance = "0.203cm";
        break;
    }
}

void StrokeDashStyle::toXML( QDomDocument & doc, QDomElement & e ) const
{
    QDomElement strokeDash = doc.createElement( "draw:stroke-dash" );
    strokeDash.setAttribute( "draw:name", m_name );
    if ( !m_style.isNull() )
        strokeDash.setAttribute( "draw:style", m_style );
    if ( !m_dots1.isNull() )
        strokeDash.setAttribute( "draw:dots1", m_dots1 );
    if ( !m_dots1_length.isNull() )
        strokeDash.setAttribute( "draw:dots1-length", m_dots1_length );
    if ( !m_dots2.isNull() )
        strokeDash.setAttribute( "draw:dots2", m_dots2 );
    if ( !m_dots2_length.isNull() )
        strokeDash.setAttribute( "draw:dots2-length", m_dots2_length );
    if ( !m_distance.isNull() )
        strokeDash.setAttribute( "draw:distance", m_distance );

    e.appendChild( strokeDash );
}

GradientStyle::GradientStyle( QDomElement & gradient, int index )
{
    m_name = QString( "Gradient %1" ).arg( index );
    m_start_intensity = "100%";
    m_end_intensity = "100%";
    m_border = "0%";

    int type = 1;
    if ( gradient.nodeName() == "PAGE" )
    {
        // gradient from page background
        QDomElement backColor1 = gradient.namedItem( "BACKCOLOR1" ).toElement();
        QDomElement backColor2 = gradient.namedItem( "BACKCOLOR2" ).toElement();
        QDomElement bcType = gradient.namedItem( "BCTYPE" ).toElement();
        QDomElement bGradient = gradient.namedItem( "BGRADIENT" ).toElement();

        if ( !backColor1.isNull() )
            m_start_color = backColor1.attribute( "color" );
        if ( !backColor2.isNull() )
            m_end_color = backColor2.attribute( "color" );
        if ( !bcType.isNull() )
            type = bcType.attribute( "value" ).toInt();
        if ( !bGradient.isNull() )
        {
            if ( bGradient.attribute( "unbalanced" ) == "0" )
            {
                m_cx = "50%";
                m_cy = "50%";
            }
            else
            {
                int cx = bGradient.attribute( "xfactor" ).toInt();
                int cy = bGradient.attribute( "yfactor" ).toInt();
                m_cx = QString( "%1%" ).arg( cx / 4 + 50 );
                m_cy = QString( "%1%" ).arg( cy / 4 + 50 );
            }
        }

    }
    else
    {
        // gradient from object
        if ( gradient.hasAttribute( "color1" ) )
            m_start_color = gradient.attribute( "color1" );
        if ( gradient.hasAttribute( "color2" ) )
            m_end_color = gradient.attribute( "color2" );
        if ( gradient.hasAttribute( "type" ) )
            type = gradient.attribute( "type" ).toInt();
        if ( gradient.hasAttribute( "unbalanced" ) )
        {
            if ( gradient.attribute( "unbalanced" ) == "0" )
            {
                m_cx = "50%";
                m_cy = "50%";
            }
            else
            {
                int cx = gradient.attribute( "xfactor" ).toInt();
                int cy = gradient.attribute( "yfactor" ).toInt();
                m_cx = QString( "%1%" ).arg( cx / 4 + 50 );
                m_cy = QString( "%1%" ).arg( cy / 4 + 50 );
            }
        }

    }

    switch ( type )
    {
    case 1:
        m_style = "linear";
        m_angle = "0";
        break;
    case 2:
        m_style = "linear";
        m_angle = "900";
        break;
    case 3:
        m_style = "linear";
        m_angle = "450";
        break;
    case 4:
        m_style = "linear";
        m_angle = "135";
        break;
    case 5:
        m_style = "radial";
        m_angle = "0";
        break;
    case 6:
        m_style = "square";
        m_angle = "0";
        break;
    case 7:
        m_style = "axial";
        m_angle = "0";
        break;
    }
}

void GradientStyle::toXML( QDomDocument & doc, QDomElement & e ) const
{
    QDomElement gradient = doc.createElement( "draw:gradient" );
    gradient.setAttribute( "draw:name", m_name );
    if ( !m_style.isNull() )
        gradient.setAttribute( "draw:style", m_style );
    if ( !m_start_color.isNull() )
        gradient.setAttribute( "draw:start-color", m_start_color );
    if ( !m_end_color.isNull() )
        gradient.setAttribute( "draw:end-color", m_end_color );
    if ( !m_start_intensity.isNull() )
        gradient.setAttribute( "draw:start-intensity", m_start_intensity );
    if ( !m_end_intensity.isNull() )
        gradient.setAttribute( "draw:end-intensity", m_end_intensity );
    if ( !m_angle.isNull() )
        gradient.setAttribute( "draw:angle", m_angle );
    if ( !m_border.isNull() )
        gradient.setAttribute( "draw:border", m_border );
    if ( !m_cx.isNull() )
        gradient.setAttribute( "draw:cx", m_cx );
    if ( !m_cy.isNull() )
        gradient.setAttribute( "draw:cy", m_cy );

    e.appendChild( gradient );
}

MarkerStyle::MarkerStyle( int style )
{
    // Markers are not working because OOImpress depends on the sequence
    // of the attributes in the draw:marker tag. svg:ViewBox has to be in
    // front of svg:d in order to work.

    switch ( style )
    {
    case 1:
        m_name = "Arrow";
        m_viewBox = "0 0 20 30";
        m_d = "m10 0-10 30h20z";
        break;
    case 2:
        m_name = "Square";
        m_viewBox = "0 0 10 10";
        m_d = "m0 0h10v10h-10z";
        break;
    case 3:
        m_name = "Circle";
        m_viewBox = "0 0 1131 1131";
        m_d = "m462 1118-102-29-102-51-93-72-72-93-51-102-29-102-13-105 13-102 29-106 51-102 72-89 93-72 102-50 102-34 106-9 101 9 106 34 98 50 93 72 72 89 51 102 29 106 13 102-13 105-29 102-51 102-72 93-93 72-98 51-106 29-101 13z";
        break;
    case 4:
        m_name = "Line Arrow";
        m_viewBox = "0 0 1122 2243";
        m_d = "m0 2108v17 17l12 42 30 34 38 21 43 4 29-8 30-21 25-26 13-34 343-1532 339 1520 13 42 29 34 39 21 42 4 42-12 34-30 21-42v-39-12l-4 4-440-1998-9-42-25-39-38-25-43-8-42 8-38 25-26 39-8 42z";
        break;
    case 5:
        m_name = "Dimension Lines";
        m_viewBox = "0 0 836 110";
        m_d = "m0 0h278 278 280v36 36 38h-278-278-280v-36-36z";
        break;
    case 6:
    case 7:
        m_name = "Double Arrow";
        m_viewBox = "0 0 1131 1918";
        m_d = "m737 1131h394l-564-1131-567 1131h398l-398 787h1131z";
        break;
    }
}

void MarkerStyle::toXML( QDomDocument & doc, QDomElement & e ) const
{
    QDomElement marker = doc.createElement( "draw:marker" );
    marker.setAttribute( "draw:name", m_name );
    if ( !m_viewBox.isNull() )
        marker.setAttribute( "svg:viewBox", m_viewBox );
    if ( !m_d.isNull() )
        marker.setAttribute( "svg:d", m_d );

    e.appendChild( marker );
}

HatchStyle::HatchStyle( int style, QString & color )
{
    m_color = color;

    switch ( style )
    {
    case 9:
        m_name = m_color + " 0 Degrees";
        m_style = "single";
        m_distance = "0.102cm";
        m_rotation = "0";
        break;
    case 10:
        m_name = m_color + " 90 Degrees";
        m_style = "single";
        m_distance = "0.102cm";
        m_rotation = "900";
        break;
    case 11:
        m_name = m_color + " Crossed 0 Degrees";
        m_style = "double";
        m_distance = "0.076cm";
        m_rotation = "900";
        break;
    case 12:
        m_name = m_color + " 45 Degrees";
        m_style = "single";
        m_distance = "0.102cm";
        m_rotation = "450";
        break;
    case 13:
        m_name = m_color + " -45 Degrees";
        m_style = "single";
        m_distance = "0.102cm";
        m_rotation = "3150";
        break;
    case 14:
        m_name = m_color + " Crossed 45 Degrees";
        m_style = "double";
        m_distance = "0.076cm";
        m_rotation = "450";
        break;
    }
}

void HatchStyle::toXML( QDomDocument & doc, QDomElement & e ) const
{
    QDomElement hatch = doc.createElement( "draw:hatch" );
    hatch.setAttribute( "draw:name", m_name );
    if ( !m_style.isNull() )
        hatch.setAttribute( "draw:style", m_style );
    if ( !m_color.isNull() )
        hatch.setAttribute( "draw:color", m_color );
    if ( !m_distance.isNull() )
        hatch.setAttribute( "draw:distance", m_distance );
    if ( !m_rotation.isNull() )
        hatch.setAttribute( "draw:rotation", m_rotation );

    e.appendChild( hatch );
}

FillImageStyle::FillImageStyle( QString & name )
{

}

void FillImageStyle::toXML( QDomDocument & doc, QDomElement & e ) const
{

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

PageStyle::PageStyle( StyleFactory * styleFactory, QDomElement & e, const uint index )
{
    QDomElement backMaster = e.namedItem( "BACKMASTER" ).toElement();
    if( !backMaster.isNull())
    {
        int tmp=0;
	if(backMaster.hasAttribute("displayBackground"))
		tmp = backMaster.attribute("displayBackground").toInt();
	 m_bg_visible = (tmp==1) ? "true" : "false";	
	 tmp = 0;
         if(backMaster.hasAttribute("displayMasterPageObject"))
	 	tmp = backMaster.attribute("displayMasterPageObject").toInt();
	 m_bg_objects_visible = (tmp==1) ? "true" : "false";	 
    }
    else
    {
    	m_bg_visible = "true";
    	m_bg_objects_visible = "true";
    }
    
    m_name = QString( "dp%1" ).arg( index );

    // check if this is an empty page tag
    if ( !e.hasChildNodes() )
        return;

    QDomElement backType = e.namedItem( "BACKTYPE" ).toElement();
    if ( backType.isNull() || backType.attribute( "value" ) == "0" )
    {
        // color
        QDomElement bcType = e.namedItem( "BCTYPE" ).toElement();
        if ( bcType.isNull() || bcType.attribute( "value" ) == "0" )
        {
            // plain
            QDomElement backColor = e.namedItem( "BACKCOLOR1" ).toElement();
            m_fill = "solid";
            m_fill_color = backColor.attribute( "color" );
        }
        else
        {
            // gradient
            m_fill = "gradient";
            m_fill_gradient_name = styleFactory->createGradientStyle( e );
        }
    }
    else
    {
        // picture
    }

    QDomElement pageDuration = e.namedItem( "PGTIMER" ).toElement();
    if ( !pageDuration.isNull() )
    {

        QTime time;
        time = time.addSecs( pageDuration.attribute("timer").toInt() );
        QString hours( QString::number( time.hour() ).rightJustified( 2, '0' ) );
        QString ms( QString::number( time.minute() ).rightJustified( 2, '0' ) );
        QString sec( QString::number( time.second() ).rightJustified( 2, '0' ) );


        //ISO8601 chapter 5.5.3.2
        //QDate doesn't encode it as this format.
        m_page_duration = QString( "PT%1H%2M%3S" ).arg( hours ).arg( ms ).arg( sec );
    }

    QDomElement pageEffect = e.namedItem( "PGEFFECT" ).toElement();
    if ( !pageEffect.isNull() )
    {
        int tmp=0;
        if(pageEffect.hasAttribute("value"))
            tmp=pageEffect.attribute("value").toInt();
        kDebug(30518)<<" tmp :"<<tmp<<endl;
        switch( tmp )
        {
        case -1:
            m_page_effect = "random";
            break;
        case 1:
            m_page_effect = "close-vertical";
            break;
        case 2:
            m_page_effect = "close-horizontal";
            break;
        case 3:
            m_page_effect = "fade-to-center";
            break;
        case 4:
            m_page_effect = "open-vertical";
            break;
        case 5:
            m_page_effect = "open-horizontal";
            break;
        case 6:
            m_page_effect = "fade-from-center";
            break;
        case 7:
        case 8:
        case 9:
        case 10:
            kDebug(30518)<<" this style is not defined :"<<tmp<<endl;
            break;
        case 11:
            m_page_effect = "spiralin-left";
            break;
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
            kDebug(30518)<<" this style is not defined :"<<tmp<<endl;
            break;
        case 19:
            m_page_effect = "fade-from-top";
            break;
        case 20:
            kDebug(30518)<<" this style is not defined :"<<tmp<<endl;
            break;
        case 21:
            m_page_effect = "fade-from-bottom";
            break;
        case 22:
            m_page_effect = "roll-from-bottom";
            break;
        case 23:
            kDebug(30518)<<" this style is not defined :"<<tmp<<endl;
            break;
        case 24:
            m_page_effect = "roll-from-right";
            break;
        case 25:
            kDebug(30518)<<" this style is not defined :"<<tmp<<endl;
            break;
        case 26:
            kDebug(30518)<<" this style is not defined :"<<tmp<<endl;
            break;
        case 27:
            kDebug(30518)<<" this style is not defined :"<<tmp<<endl;
            break;
        case 28:
            kDebug(30518)<<" this style is not defined :"<<tmp<<endl;
            break;
        case 29:
            kDebug(30518)<<" this style is not defined :"<<tmp<<endl;
            break;
        case 30:
            kDebug(30518)<<" this style is not defined :"<<tmp<<endl;
            break;
        case 31:
            kDebug(30518)<<" this style is not defined :"<<tmp<<endl;
            break;
        case 32:
            kDebug(30518)<<" this style is not defined :"<<tmp<<endl;
            break;
        case 33:
            kDebug(30518)<<" this style is not defined :"<<tmp<<endl;
            break;
        case 34:
            kDebug(30518)<<" this style is not defined :"<<tmp<<endl;
            break;
        case 35:
            m_page_effect = "dissolve";
            break;
        case 36:
            m_page_effect = "fade-from-lowerright";
            break;
        case 37:
            m_page_effect = "fade-from-upperright";
            break;
        case 38:
            m_page_effect = "fade-from-lowerleft";
            break;
        case 39:
            m_page_effect = "fade-from-upperleft";
            break;
        case 40:
            kDebug(30518)<<" this style is not defined :"<<tmp<<endl;
            break;
        default:
            kDebug(30518)<<" style page effect not define : "<<tmp<<endl;
            break;
        }
    }
}

void PageStyle::toXML( QDomDocument & doc, QDomElement & e ) const
{
    QDomElement style = doc.createElement( "style:style" );
    style.setAttribute( "style:name", m_name );
    style.setAttribute( "style:family", "drawing-page" );

    QDomElement properties = doc.createElement( "style:properties" );
    properties.setAttribute( "presentation:background-visible", m_bg_visible );
    properties.setAttribute( "presentation:background-objects-visible",
                             m_bg_objects_visible );
    if ( !m_page_duration.isEmpty() )
    {
        properties.setAttribute( "presentation:duration", m_page_duration );
        properties.setAttribute( "presentation:transition-type", "automatic" );
    }
    if ( !m_page_effect.isEmpty() )
        properties.setAttribute( "presentation:transition-style",
                                 m_page_effect );
    if ( !m_fill.isNull() )
        properties.setAttribute( "draw:fill", m_fill );
    if ( !m_fill_color.isNull() )
        properties.setAttribute( "draw:fill-color", m_fill_color );
    if ( !m_fill_image_name.isNull() )
        properties.setAttribute( "draw:fill-image-name", m_fill_image_name );
    if ( !m_fill_image_width.isNull() )
        properties.setAttribute( "draw:fill-image-width", m_fill_image_width );
    if ( !m_fill_image_height.isNull() )
        properties.setAttribute( "draw:fill-image-height", m_fill_image_height );
    if ( !m_fill_image_ref_point.isNull() )
        properties.setAttribute( "draw:fill-image-ref-point", m_fill_image_ref_point );
    if ( !m_fill_gradient_name.isNull() )
        properties.setAttribute( "draw:fill-gradient-name", m_fill_gradient_name );
    if ( !m_repeat.isNull() )
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
             m_fill_gradient_name == pageStyle.m_fill_gradient_name &&
             m_repeat == pageStyle.m_repeat &&
        m_page_effect == pageStyle.m_page_effect &&
        m_page_duration == pageStyle.m_page_duration );
}

TextStyle::TextStyle( QDomElement & e, const uint index )
{
    m_name = QString( "T%1" ).arg( index );
    if ( e.hasAttribute( "family" ) )
        m_font_family = e.attribute( "family" );
    if ( e.hasAttribute( "pointSize" ) )
        m_font_size = QString( "%1pt" ).arg( e.attribute( "pointSize" ) );
    if ( e.hasAttribute( "color" ) )
        m_color = e.attribute( "color" );
    if ( e.hasAttribute( "bold" ) && e.attribute( "bold" ) == "1" )
        m_font_weight = "bold";
    if ( e.hasAttribute( "italic" ) && e.attribute( "italic" ) == "1" )
        m_font_style = "italic";
    if ( e.hasAttribute( "strikeOut" ) )
    {
        if ( e.attribute( "strikeOut" ) == "single" )
            m_text_crossing_out = "single-line";
        else if ( e.attribute( "strikeOut" ) == "single-bold" )
            m_text_crossing_out = "thick-line";
        else if ( e.attribute( "strikeOut" ) == "double" )
            m_text_crossing_out = "double-line";
    }
    if ( e.hasAttribute( "underline" ) )
    {
        QString underline = e.attribute( "underline" );
        QString style = e.attribute( "underlinestyleline" );
        m_text_underline_color = e.attribute( "underlinecolor" );

        if ( style == "solid" )
        {
            if ( underline == "1" )
                m_text_underline = "single";
            else if ( underline == "single-bold" )
                m_text_underline = "bold";
            else if ( underline == "double" )
                m_text_underline = "double";
            else if ( underline == "wave" )
                m_text_underline = "wave";
        }
        else if ( style == "dot" )
        {
            if ( underline == "1" )
                m_text_underline = "dotted";
            else if ( underline == "single-bold" )
                m_text_underline = "bold-dotted";
        }
        else if ( style == "dash" )
            m_text_underline = "dash";
    }
}

void TextStyle::toXML( QDomDocument & doc, QDomElement & e ) const
{
    QDomElement style = doc.createElement( "style:style" );
    style.setAttribute( "style:name", m_name );
    style.setAttribute( "style:family", "text" );

    QDomElement properties = doc.createElement( "style:properties" );
    if ( !m_font_size.isNull() )
        properties.setAttribute( "fo:font-size", m_font_size );
    if ( !m_font_family.isNull() )
        properties.setAttribute( "fo:font-family", m_font_family );
    if ( !m_font_family_generic.isNull() )
        properties.setAttribute( "fo:font-family-generic", m_font_family_generic );
    if ( !m_color.isNull() )
        properties.setAttribute( "fo:color", m_color );
    if ( !m_font_pitch.isNull() )
        properties.setAttribute( "style:font-pitch", m_font_pitch );
    if ( !m_font_style.isNull() )
        properties.setAttribute( "fo:font-style", m_font_style );
    if ( !m_font_weight.isNull() )
        properties.setAttribute( "fo:font-weight", m_font_weight );
    if ( !m_text_shadow.isNull() )
        properties.setAttribute( "fo:text-shadow", m_text_shadow );
    if ( !m_text_underline.isNull() )
        properties.setAttribute( "style:text-underline", m_text_underline );
    if ( !m_text_underline_color.isNull() )
        properties.setAttribute( "style:text-underline-color", m_text_underline_color );
    if ( !m_text_crossing_out.isNull() )
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

GraphicStyle::GraphicStyle( StyleFactory * styleFactory, QDomElement & e, const uint index )
{
    QDomNode pen = e.namedItem( "PEN" );
    QDomNode brush = e.namedItem( "BRUSH" );
    QDomNode linebegin = e.namedItem( "LINEBEGIN" );
    QDomNode lineend = e.namedItem( "LINEEND" );
    QDomNode gradient = e.namedItem( "GRADIENT" );
    QDomNode shadow = e.namedItem( "SHADOW" );
    QDomNode textObject = e.namedItem( "TEXTOBJ" );
    if ( !textObject.isNull() )
    {
        QDomElement textObjectElement = textObject.toElement();
        if ( textObjectElement.hasAttribute( "verticalAlign" ) )
        {
            m_textAlignment = textObjectElement.attribute("verticalAlign");
            if ( m_textAlignment == "center" )
                m_textAlignment = "middle";
        }
        if ( textObjectElement.hasAttribute( "bleftpt" ) )
        {
            m_textMarginLeft = QString( "%1pt" ).arg( textObjectElement.attribute( "bleftpt" ) );
        }
        if ( textObjectElement.hasAttribute( "bbottompt" ) )
        {
            m_textMarginBottom = QString( "%1pt" ).arg( textObjectElement.attribute( "bbottompt" ) );
        }
        if ( textObjectElement.hasAttribute( "btoppt" ) )
        {
            m_textMarginTop = QString( "%1pt" ).arg( textObjectElement.attribute( "btoppt" ) );
        }
        if ( textObjectElement.hasAttribute( "brightpt" ) )
        {
            m_textMarginRight = QString( "%1pt" ).arg( textObjectElement.attribute( "brightpt" ) );
        }

    }
    kDebug(30518)<<" alignment :"<<m_textAlignment<<endl;

    m_name = QString( "gr%1" ).arg( index );
    if ( !pen.isNull() )
    {
        QDomElement p = pen.toElement();
        m_stroke_width = StyleFactory::toCM( p.attribute( "width" ) );
        m_stroke_color = p.attribute( "color" );

        int style = p.attribute( "style" ).toInt();
        if ( style == 1 )
            m_stroke = "solid";
        else if ( style >= 2 && style <= 5 )
        {
            m_stroke = "dash";
            m_stroke_dash = styleFactory->createStrokeDashStyle( style );
        }
        else
            m_stroke = "none";
    }

    if ( !brush.isNull() )
    {
        QDomElement b = brush.toElement();
        m_fill_color = b.attribute( "color" );

        int style = b.attribute( "style" ).toInt();
        if ( style == 1 )
            m_fill = "solid";
        else if ( style >= 9 && style <= 14 )
        {
            m_fill = "hatch";
            m_fill_hatch_name = styleFactory->createHatchStyle( style, m_fill_color );
        }
        else if ( style >= 2 && style <= 8 )
        {
            if ( style == 2 )
                m_transparency = "94%";
            else if ( style == 3 )
                m_transparency = "88%";
            else if ( style == 4 )
                m_transparency = "63%";
            else if ( style == 5 )
                m_transparency = "50%";
            else if ( style == 6 )
                m_transparency = "37%";
            else if ( style == 7 )
                m_transparency = "12%";
            else if ( style == 8 )
                m_transparency = "6%";
        }
    }
    else if ( !gradient.isNull() )
    {
        QDomElement g = gradient.toElement();
        m_fill = "gradient";
        m_fill_gradient_name = styleFactory->createGradientStyle( g );
    }
    else
        m_fill = "none";

    if ( !linebegin.isNull() )
    {
        QDomElement lb = linebegin.toElement();
        m_marker_start_width = "0.25cm";

        int style = lb.attribute( "value" ).toInt();
        m_marker_start = styleFactory->createMarkerStyle( style );
    }

    if ( !lineend.isNull() )
    {
        QDomElement le = lineend.toElement();
        m_marker_end_width = "0.25cm";

        int style = le.attribute( "value" ).toInt();
        m_marker_end = styleFactory->createMarkerStyle( style );
    }

    if ( !shadow.isNull() )
    {
        QDomElement s = shadow.toElement();
        m_shadow = "visible";
        m_shadow_color = s.attribute( "color" );

        int direction = s.attribute( "direction" ).toInt();
        QString distance = StyleFactory::toCM( s.attribute( "distance" ) );
        switch ( direction )
        {
        case 1:
            m_shadow_offset_x = "-" + distance;
            m_shadow_offset_y = "-" + distance;
            break;
        case 2:
            m_shadow_offset_x = "0cm";
            m_shadow_offset_y = "-" + distance;
            break;
        case 3:
            m_shadow_offset_x = distance;
            m_shadow_offset_y = "-" + distance;
            break;
        case 4:
            m_shadow_offset_x = distance;
            m_shadow_offset_y = "0cm";
            break;
        case 5:
            m_shadow_offset_x = distance;
            m_shadow_offset_y = distance;
            break;
        case 6:
            m_shadow_offset_x = "0cm";
            m_shadow_offset_y = distance;
            break;
        case 7:
            m_shadow_offset_x = "-" + distance;
            m_shadow_offset_y = distance;
            break;
        case 8:
            m_shadow_offset_x = "-" + distance;
            m_shadow_offset_y = "0cm";
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
    if ( !m_stroke.isNull() )
        properties.setAttribute( "draw:stroke", m_stroke );
    if ( !m_stroke_dash.isNull() )
        properties.setAttribute( "draw:stroke-dash", m_stroke_dash );
    if ( !m_stroke_color.isNull() )
        properties.setAttribute( "svg:stroke-color", m_stroke_color );
    if ( !m_stroke_width.isNull() )
        properties.setAttribute( "svg:stroke-width", m_stroke_width );
    if ( !m_shadow.isNull() )
        properties.setAttribute( "draw:shadow", m_shadow );
    if ( !m_shadow_offset_x.isNull() )
        properties.setAttribute( "draw:shadow-offset-x", m_shadow_offset_x );
    if ( !m_shadow_offset_y.isNull() )
        properties.setAttribute( "draw:shadow-offset-y", m_shadow_offset_y );
    if ( !m_shadow_color.isNull() )
        properties.setAttribute( "draw:shadow-color", m_shadow_color );
    if ( !m_margin_left.isNull() )
        properties.setAttribute( "fo:margin-left", m_margin_left );
    if ( !m_margin_right.isNull() )
        properties.setAttribute( "fo:margin-right", m_margin_right );
    if ( !m_margin_top.isNull() )
        properties.setAttribute( "fo:margin-top", m_margin_top );
    if ( !m_margin_bottom.isNull() )
        properties.setAttribute( "fo:margin-bottom", m_margin_bottom );
    if ( !m_color.isNull() )
        properties.setAttribute( "fo:color", m_color );
    if ( !m_text_outline.isNull() )
        properties.setAttribute( "style:text-outline", m_text_outline );
    if ( !m_text_crossing_out.isNull() )
        properties.setAttribute( "style:text-crossing-out", m_text_crossing_out );
    if ( !m_font_family.isNull() )
        properties.setAttribute( "fo:font-family", m_font_family );
    if ( !m_font_size.isNull() )
        properties.setAttribute( "fo:font-size", m_font_size );
    if ( !m_font_style.isNull() )
        properties.setAttribute( "fo:font-style", m_font_style );
    if ( !m_text_shadow.isNull() )
        properties.setAttribute( "fo:text-shadow", m_text_shadow );
    if ( !m_text_underline.isNull() )
        properties.setAttribute( "style:text-underline", m_text_underline );
    if ( !m_font_weight.isNull() )
        properties.setAttribute( "fo:font-weight", m_font_weight );
    if ( !m_line_height.isNull() )
        properties.setAttribute( "fo:line-height", m_line_height );
    if ( !m_text_align.isNull() )
        properties.setAttribute( "fo:text-align", m_text_align );
    if ( !m_fill.isNull() )
        properties.setAttribute( "draw:fill", m_fill );
    if ( !m_fill_color.isNull() )
        properties.setAttribute( "draw:fill-color", m_fill_color );
    if ( !m_fill_hatch_name.isNull() )
        properties.setAttribute( "draw:fill-hatch-name", m_fill_hatch_name );
    if ( !m_enable_numbering.isNull() )
        properties.setAttribute( "text:enable-numbering", m_enable_numbering );
    if ( !m_marker_start.isNull() )
        properties.setAttribute( "draw:marker-start", m_marker_start );
    if ( !m_marker_start_width.isNull() )
        properties.setAttribute( "draw:marker-start-width", m_marker_start_width );
    if ( !m_marker_end.isNull() )
        properties.setAttribute( "draw:marker-end", m_marker_end );
    if ( !m_marker_end_width.isNull() )
        properties.setAttribute( "draw:marker-end-width", m_marker_end_width );
    if ( !m_fill_gradient_name.isNull() )
        properties.setAttribute( "draw:fill-gradient-name", m_fill_gradient_name );
    if ( !m_transparency.isNull() )
        properties.setAttribute( "draw:transparency", m_transparency );
    if ( !m_textAlignment.isNull() )
        properties.setAttribute( "draw:textarea-vertical-align", m_textAlignment );
    if ( !m_textMarginLeft.isNull() )
        properties.setAttribute( "fo:padding-left", m_textMarginLeft );
    if ( !m_textMarginBottom.isNull() )
        properties.setAttribute( "fo:padding-bottom", m_textMarginBottom );
    if ( !m_textMarginTop.isNull() )
        properties.setAttribute( "fo:padding-top", m_textMarginTop );
    if ( !m_textMarginRight.isNull() )
        properties.setAttribute( "fo:padding-right", m_textMarginRight );


    style.appendChild( properties );
    e.appendChild( style );
}

bool GraphicStyle::operator==( const GraphicStyle & graphicStyle ) const
{
    return ( m_stroke == graphicStyle.m_stroke &&
             m_stroke_dash == graphicStyle.m_stroke_dash &&
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
             m_fill_hatch_name == graphicStyle.m_fill_hatch_name &&
             m_enable_numbering == graphicStyle.m_enable_numbering &&
             m_marker_start == graphicStyle.m_marker_start &&
             m_marker_start_width == graphicStyle.m_marker_start_width &&
             m_marker_end == graphicStyle.m_marker_end &&
             m_marker_end_width == graphicStyle.m_marker_end_width &&
             m_fill_gradient_name == graphicStyle.m_fill_gradient_name &&
             m_transparency == graphicStyle.m_transparency &&
             m_textAlignment == graphicStyle.m_textAlignment &&
        m_textMarginLeft == graphicStyle.m_textMarginLeft &&
        m_textMarginBottom == graphicStyle.m_textMarginBottom &&
        m_textMarginTop == graphicStyle.m_textMarginTop &&
        m_textMarginRight == graphicStyle.m_textMarginRight);
}

ParagraphStyle::ParagraphStyle( QDomElement & e, const uint index )
{
    // some defaults that may be overwritten
    m_margin_left = "0cm";
    m_margin_right = "0cm";
    m_text_indent = "0cm";

    QDomNode shadow = e.namedItem( "SHADOW" );
    QDomNode indents = e.namedItem( "INDENTS" );
    QDomNode offsets = e.namedItem( "OFFSETS" );
    QDomNode leftBorder = e.namedItem( "LEFTBORDER" );
    QDomNode rightBorder = e.namedItem( "RIGHTBORDER" );
    QDomNode topBorder = e.namedItem( "TOPBORDER" );
    QDomNode bottomBorder = e.namedItem( "BOTTOMBORDER" );
    QDomNode lineSpacing = e.namedItem( "LINESPACING" );
    QDomNode counter = e.namedItem( "COUNTER" );

    m_name = QString( "P%1" ).arg( index );
    if ( e.hasAttribute( "align" ) )
    {
        int align = e.attribute( "align" ).toInt();
        switch ( align )
        {
        case 0: // left
            m_text_align = "start";
            break;
        case 2: // right
            m_text_align = "end";
            break;
        case 4: // center
            m_text_align = "center";
            break;
        case 8: // justify
            m_text_align = "justify";
            break;
        }
    }

    if ( !shadow.isNull() )
    {
        QDomElement s = shadow.toElement();
        QString distance = QString( "%1pt" ).arg( s.attribute( "distance" ) );
        m_text_shadow = distance + " " + distance;
    }

    if ( !indents.isNull() )
    {
        QDomElement i = indents.toElement();
        m_margin_left = StyleFactory::toCM( i.attribute( "left" ) );
        m_margin_right = StyleFactory::toCM( i.attribute( "right" ) );
        m_text_indent = StyleFactory::toCM( i.attribute( "first" ) );
    }

    if ( !offsets.isNull() )
    {
        QDomElement o = offsets.toElement();
        m_margin_top = StyleFactory::toCM( o.attribute( "before" ) );
        m_margin_bottom = StyleFactory::toCM( o.attribute( "after" ) );
    }

    if ( !leftBorder.isNull() )
        m_border_left = parseBorder( leftBorder.toElement() );
    if ( !rightBorder.isNull() )
        m_border_right = parseBorder( rightBorder.toElement() );
    if ( !topBorder.isNull() )
        m_border_top = parseBorder( topBorder.toElement() );
    if ( !bottomBorder.isNull() )
        m_border_bottom = parseBorder( bottomBorder.toElement() );

    if ( !lineSpacing.isNull() )
    {
        QDomElement l = lineSpacing.toElement();
        QString type = l.attribute( "type" );

        if ( type == "single" )
            m_line_height = "100%";
        else if ( type == "oneandhalf" )
            m_line_height = "150%";
        else if ( type == "double" )
            m_line_height = "200%";
        else if ( type == "multiple" )
            m_line_height = QString( "%1%" ).arg( l.attribute( "spacingvalue" ).toInt() * 100 );
        else if ( type == "custom" )
            m_line_spacing = StyleFactory::toCM( l.attribute( "spacingvalue" ) );
        else if ( type == "atleast" )
            m_line_height_at_least = StyleFactory::toCM( l.attribute( "spacingvalue" ) );
    }

    if ( !counter.isNull() )
        m_enable_numbering = "true";
}

void ParagraphStyle::toXML( QDomDocument & doc, QDomElement & e ) const
{
    QDomElement style = doc.createElement( "style:style" );
    style.setAttribute( "style:name", m_name );
    style.setAttribute( "style:family", "paragraph" );

    QDomElement properties = doc.createElement( "style:properties" );
    if ( !m_margin_left.isNull() )
        properties.setAttribute( "fo:margin-left", m_margin_left );
    if ( !m_margin_right.isNull() )
        properties.setAttribute( "fo:margin-right", m_margin_right );
    if ( !m_text_indent.isNull() )
        properties.setAttribute( "fo:text-indent", m_text_indent );
    if ( !m_text_align.isNull() )
        properties.setAttribute( "fo:text-align", m_text_align );
    if ( !m_enable_numbering.isNull() )
        properties.setAttribute( "text:enable-numbering", m_enable_numbering );
    if ( !m_text_shadow.isNull() )
        properties.setAttribute( "fo:text-shadow", m_text_shadow );
    if ( !m_margin_top.isNull() )
        properties.setAttribute( "fo:margin-top", m_margin_top );
    if ( !m_margin_bottom.isNull() )
        properties.setAttribute( "fo:margin-bottom", m_margin_bottom );
    if ( !m_border_left.isNull() )
        properties.setAttribute( "fo:border-left", m_border_left );
    if ( !m_border_right.isNull() )
        properties.setAttribute( "fo:border-right", m_border_right );
    if ( !m_border_top.isNull() )
        properties.setAttribute( "fo:border-top", m_border_top );
    if ( !m_border_bottom.isNull() )
        properties.setAttribute( "fo:border-bottom", m_border_bottom );
    if ( !m_line_height.isNull() )
        properties.setAttribute( "fo:line-height", m_line_height );
    if ( !m_line_height_at_least.isNull() )
        properties.setAttribute( "style:line-height-at-least", m_line_height_at_least );
    if ( !m_line_spacing.isNull() )
        properties.setAttribute( "style:line-spacing", m_line_spacing );

    style.appendChild( properties );
    e.appendChild( style );
}

bool ParagraphStyle::operator==( const ParagraphStyle & paragraphStyle ) const
{
    return ( m_margin_left == paragraphStyle.m_margin_left &&
             m_margin_right == paragraphStyle.m_margin_right &&
             m_text_indent == paragraphStyle.m_text_indent &&
             m_text_align == paragraphStyle.m_text_align &&
             m_enable_numbering == paragraphStyle.m_enable_numbering &&
             m_text_shadow == paragraphStyle.m_text_shadow &&
             m_margin_top == paragraphStyle.m_margin_top &&
             m_margin_bottom == paragraphStyle.m_margin_bottom &&
             m_border_left == paragraphStyle.m_border_left &&
             m_border_right == paragraphStyle.m_border_right &&
             m_border_top == paragraphStyle.m_border_top &&
             m_border_bottom == paragraphStyle.m_border_bottom &&
             m_line_height == paragraphStyle.m_line_height &&
             m_line_height_at_least == paragraphStyle.m_line_height_at_least &&
             m_line_spacing == paragraphStyle.m_line_spacing );
}

QString ParagraphStyle::parseBorder( QDomElement e )
{
    QString style;
    int _style = e.attribute( "style" ).toInt();
    if ( _style == 5 )
        style = "double";
    else
        style = "solid";

    QString width = StyleFactory::toCM( e.attribute( "width" ) );

    QColor color( e.attribute( "red" ).toInt(),
                  e.attribute( "green" ).toInt(),
                  e.attribute( "blue" ).toInt() );

    return QString( "%1 %2 %3" ).arg( width ).arg( style ).arg( color.name() );
}

ListStyle::ListStyle( QDomElement & e, const uint index )
{
    // setting some default values
    m_min_label_width = 0.6;
    m_color = "#000000";
    m_font_size = "100%";

    m_name = QString( "L%1" ).arg( index );

    if ( e.hasAttribute( "type" ) )
    {
        int type = e.attribute( "type" ).toInt();
        switch ( type )
        {
        case 1: // arabic numbers
            m_listLevelStyle = LLS_NUMBER;
            m_num_suffix = ".";
            m_num_format = "1";
            break;
        case 2: // lower alphabetical
            m_listLevelStyle = LLS_NUMBER;
            m_num_suffix = ".";
            m_num_format = "a";
            break;
        case 3: // upper alphabetical
            m_listLevelStyle = LLS_NUMBER;
            m_num_suffix = ".";
            m_num_format = "A";
            break;
        case 4: // lower roman
            m_listLevelStyle = LLS_NUMBER;
            m_num_suffix = ".";
            m_num_format = "i";
            break;
        case 5: // upper roman
            m_listLevelStyle = LLS_NUMBER;
            m_num_suffix = ".";
            m_num_format = "I";
            break;
        case 6: // custom
            m_listLevelStyle = LLS_BULLET;
            if ( e.hasAttribute( "text" ) )
                m_bullet_char = e.attribute( "text" );
            break;
        case 8: // circle bullet
            m_listLevelStyle = LLS_BULLET;
            break;
        case 9: // square bullet
            m_listLevelStyle = LLS_BULLET;
            break;
        case 10: // disc bullet
            m_listLevelStyle = LLS_BULLET;
            break;
        case 11: // box bullet
            m_listLevelStyle = LLS_BULLET;
            break;
        }
    }

    if ( e.hasAttribute( "bulletfont" ) )
        m_font_family = e.attribute( "bulletfont" );
}

void ListStyle::toXML( QDomDocument & doc, QDomElement & e ) const
{
    QDomElement style = doc.createElement( "text:list-style" );
    style.setAttribute( "style:name", m_name );

    for ( int level = 1; level <= 10; level++ )
    {
        QDomElement listLevelStyle;
        if ( m_listLevelStyle == LLS_NUMBER )
        {
            listLevelStyle = doc.createElement( "text:list-level-style-number" );
            listLevelStyle.setAttribute( "text:level", level );
            if ( !m_num_suffix.isNull() )
                listLevelStyle.setAttribute( "style:num-suffix", m_num_suffix );
            if ( !m_num_format.isNull() )
                listLevelStyle.setAttribute( "style:num-format", m_num_format );
        }
        else
        {
            listLevelStyle = doc.createElement( "text:list-level-style-bullet" );
            listLevelStyle.setAttribute( "text:level", level );
            if ( !m_bullet_char.isNull() )
                listLevelStyle.setAttribute( "text:bullet-char", m_bullet_char );
        }

        QDomElement properties = doc.createElement( "style:properties" );
        if ( level > 1 )
        {
            properties.setAttribute( "text:min-label-width",
                                     QString( "%1cm" ).arg( m_min_label_width ) );
            properties.setAttribute( "text:space-before",
                                     QString( "%1cm" ).arg( m_min_label_width * ( level - 1 ) ) );
        }

        if ( !m_color.isNull() )
            properties.setAttribute( "fo:color", m_color );
        if ( !m_font_size.isNull() )
            properties.setAttribute( "fo:font-size", m_font_size );
        if ( !m_font_family.isNull() )
            properties.setAttribute( "fo:font-family", m_font_family );

        listLevelStyle.appendChild( properties );
        style.appendChild( listLevelStyle );
    }
    e.appendChild( style );
}

bool ListStyle::operator==( const ListStyle & listStyle ) const
{
    return ( m_listLevelStyle == listStyle.m_listLevelStyle &&
             m_num_suffix == listStyle.m_num_suffix &&
             m_num_format == listStyle.m_num_format &&
             m_bullet_char == listStyle.m_bullet_char &&
             m_min_label_width == listStyle.m_min_label_width &&
             m_color == listStyle.m_color &&
             m_font_size == listStyle.m_font_size &&
             m_font_family == listStyle.m_font_family );
}
