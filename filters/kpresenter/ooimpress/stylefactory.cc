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
    m_textStyles.setAutoDelete( true );
    m_graphicStyles.setAutoDelete( true );
    m_paragraphStyles.setAutoDelete( true );
    m_presentationStyles.setAutoDelete( true );
}

StyleFactory::~StyleFactory()
{
}

void StyleFactory::addOfficeStyles( QDomDocument & doc, QDomElement & styles )
{
}

void StyleFactory::addOfficeMaster( QDomDocument & doc, QDomElement & master )
{
    for ( PageStyle * p = m_pageStyles.first(); p ; p = m_pageStyles.next() )
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
    for ( PageStyle * p = m_pageStyles.first(); p ; p = m_pageStyles.next() )
    {
        p->toXML( doc, automatic );
    }
}

void StyleFactory::addAutomaticStyles( QDomDocument & doc, QDomElement & autoStyles )
{
    addTextStyles( doc, autoStyles );
    addGraphicStyles( doc, autoStyles );
    addParagraphStyles( doc, autoStyles );
    addPresentationStyles( doc, autoStyles );
}

QString StyleFactory::createPageStyle( QDomElement & e )
{
    PageStyle * newPageStyle = new PageStyle( e, m_pageStyles.count() );
    for ( PageStyle * p = m_pageStyles.first(); p ; p = m_pageStyles.next() )
    {
        if ( p == newPageStyle )
        {
            delete newPageStyle;
            return p->style();
        }
    }

    m_pageStyles.append( newPageStyle );
    return newPageStyle->style();
}

QString StyleFactory::createTextStyle( QDomElement & e )
{

}

QString StyleFactory::createGraphicStyle( QDomElement & e )
{

}

QString StyleFactory::createParagraphStyle( QDomElement & e )
{

}

QString StyleFactory::createPresentationStyle( QDomElement & e )
{

}

void StyleFactory::addTextStyles( QDomDocument & doc, QDomElement & autoStyles )
{

}

void StyleFactory::addGraphicStyles( QDomDocument & doc, QDomElement & autoStyles )
{

}

void StyleFactory::addParagraphStyles( QDomDocument & doc, QDomElement & autoStyles )
{

}

void StyleFactory::addPresentationStyles( QDomDocument & doc, QDomElement & autoStyles )
{

}

QString StyleFactory::toCM( const QString & point )
{
    double pt = point.toFloat();
    double cm = KoUnit::toCM( pt );
    return QString( "%1cm" ).arg ( cm );
}

PageStyle::PageStyle( QDomElement & page, const uint index )
{
    QDomNode borders = page.namedItem( "PAPERBORDERS" );
    QDomElement p = page.toElement();
    QDomElement b = borders.toElement();

    m_name = QString( "PM%1" ).arg( index );
    m_style = QString( "Default%1" ).arg( index );
    m_margin_top = StyleFactory::toCM( b.attribute( "ptTop" ) );
    m_margin_bottom = StyleFactory::toCM( b.attribute( "ptBottom" ) );
    m_margin_left = StyleFactory::toCM( b.attribute( "ptLeft" ) );
    m_margin_right = StyleFactory::toCM( b.attribute( "ptRight" ) );
    m_page_width = StyleFactory::toCM( p.attribute( "ptWidth" ) );
    m_page_height = StyleFactory::toCM( p.attribute( "ptHeight" ) );
    m_orientation = "landscape";
}

void PageStyle::toXML( QDomDocument & doc, QDomElement & e ) const
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

bool PageStyle::operator==( const PageStyle & pageStyle ) const
{
    return ( m_margin_top == pageStyle.m_margin_top &&
             m_margin_bottom == pageStyle.m_margin_bottom &&
             m_margin_left == pageStyle.m_margin_left &&
             m_margin_right == pageStyle.m_margin_right &&
             m_page_width == pageStyle.m_page_width &&
             m_page_height == pageStyle.m_page_height &&
             m_orientation == pageStyle.m_orientation );
}

