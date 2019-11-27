/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#include "KPrPageLayout.h"

#include "KPrPlaceholder.h"
#include "StageDebug.h"

#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoGenStyle.h>
#include <KoGenStyles.h>
#include <KoPASavingContext.h>

#include <QStandardPaths>
#include <QBuffer>
#include <QSize>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>


KPrPageLayout::KPrPageLayout()
: m_layoutType( Page )
{
}

KPrPageLayout::~KPrPageLayout()
{
    qDeleteAll( m_placeholders );
}

bool KPrPageLayout::loadOdf( const KoXmlElement &element, const QRectF & pageRect )
{
    if ( element.hasAttributeNS( KoXmlNS::style, "display-name" ) ) {
        m_name = element.attributeNS( KoXmlNS::style, "display-name" );
    }
    else {
        m_name = element.attributeNS( KoXmlNS::style, "name" );
    }

    KoXmlElement child;
    forEachElement( child, element ) {
        if ( child.tagName() == "placeholder" && child.namespaceURI() == KoXmlNS::presentation ) {
            KPrPlaceholder * placeholder = new KPrPlaceholder;
            if ( placeholder->loadOdf( child, pageRect ) ) {
                m_placeholders.append( placeholder );
                if ( placeholder->presentationObject() == "handout" ) {
                    m_layoutType = Handout;
                }
            }
            else {
                warnStage << "loading placeholder failed";
                delete placeholder;
            }
        }
        else {
            warnStage << "unknown tag" << child.namespaceURI() << child.tagName() << "when loading page layout";
        }
    }

    bool retval = true;
    if ( m_placeholders.isEmpty() ) {
        warnStage << "no placeholder for page layout" << m_name << "found";
        retval = false;
    }
    else {
        /* 
         * do fixups for wrong saved data from OO somehow they save negative values for width and height sometimes
         * <style:presentation-page-layout style:name="AL10T12">
         *   <presentation:placeholder presentation:object="title" svg:x="2.057cm" svg:y="1.743cm" svg:width="23.911cm" svg:height="3.507cm"/>
         *   <presentation:placeholder presentation:object="outline" svg:x="2.057cm" svg:y="5.838cm" svg:width="11.669cm" svg:height="13.23cm"/>
         *   <presentation:placeholder presentation:object="object" svg:x="14.309cm" svg:y="5.838cm" svg:width="-0.585cm" svg:height="6.311cm"/>
         *   <presentation:placeholder presentation:object="object" svg:x="14.309cm" svg:y="12.748cm" svg:width="-0.585cm" svg:height="-0.601cm"/>
         * </style:presentation-page-layout>
         */
        QList<KPrPlaceholder *>::iterator it( m_placeholders.begin() );
        KPrPlaceholder * last = *it;
        ++it;
        for ( ; it != m_placeholders.end(); ++it ) {
            ( *it )->fix( last->rect( QSizeF( 1, 1 ) ) );
            last = *it;
        }
    }
    return retval;
}

QString KPrPageLayout::saveOdf( KoPASavingContext & context ) const
{
    KoGenStyle style( KoGenStyle::PresentationPageLayoutStyle );

    style.addAttribute( "style:display-name", m_name );

    QBuffer buffer;
    buffer.open( QIODevice::WriteOnly );
    KoXmlWriter elementWriter( &buffer );

    QList<KPrPlaceholder *>::const_iterator it( m_placeholders.begin() );
    for ( ; it != m_placeholders.end(); ++it ) {
        ( *it )->saveOdf( elementWriter );
    }

    QString placeholders = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
    style.addChildElement( "placeholders", placeholders );

    // return the style name so we can save the ptr -> style in the saving context so the pages can use it during saving
    return context.mainStyles().insert( style, "pl" );
}

QList<KPrPlaceholder *> KPrPageLayout::placeholders() const
{
    return m_placeholders;
}

QPixmap KPrPageLayout::thumbnail() const
{
    QSvgRenderer renderer;

    QSize size( 80, 60 );
    QPixmap pic( size );
    pic.fill();
    QPainter p( &pic );

    // TODO: use QStandardPaths for that instead
    QString file = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("calligrastage/pics/layout-elements.svg"));
    if ( renderer.load( file ) ) {
        QList<KPrPlaceholder *>::const_iterator it( m_placeholders.begin() );
        for ( ; it != m_placeholders.end(); ++it ) {
            debugStage << "-----------------" <<( *it )->presentationObject() << ( *it )->rect( size );
            renderer.render( &p, ( *it )->presentationObject(), ( *it )->rect( size ) );
        }
    }
    else {
        warnStage << "could not load" << file;
    }

    return pic;
}

KPrPageLayout::Type KPrPageLayout::type() const
{
    return m_layoutType;
}

bool comparePlaceholder( const KPrPlaceholder * p1, const KPrPlaceholder * p2 )
{
    return ( * p1 ) < ( * p2 );
}

bool KPrPageLayout::operator<( const KPrPageLayout & other ) const
{
    if ( m_placeholders.size() == other.m_placeholders.size() ) {
        QList<KPrPlaceholder *> placeholders( m_placeholders );
        QList<KPrPlaceholder *> otherPlaceholders( other.m_placeholders );
        std::sort( placeholders.begin(), placeholders.end(), comparePlaceholder );
        std::sort( otherPlaceholders.begin(), otherPlaceholders.end(), comparePlaceholder );

        QList<KPrPlaceholder *>::ConstIterator it( placeholders.constBegin() );
        QList<KPrPlaceholder *>::ConstIterator otherIt( otherPlaceholders.constBegin() );
        debugStage << "KPrPageLayout::operator< start" << ( *it )->rect( QSizeF( 1, 1 ) ) << ( *otherIt )->rect( QSizeF( 1, 1 ) );

        for ( ; it != placeholders.constEnd(); ++it, ++otherIt ) {
            debugStage << "KPrPageLayout::operator<" << ( *it )->rect( QSizeF( 1, 1 ) ) << ( *otherIt )->rect( QSizeF( 1, 1 ) );
            if ( *( *it ) == *( *otherIt ) ) {
                debugStage << "KPrPageLayout::operator< 0" << ( *( *it ) < *( *otherIt ) );
                continue;
            }
            debugStage << "KPrPageLayout::operator< 1" << ( *( *it ) < *( *otherIt ) );
            return *( *it ) < *( *otherIt );
        }
        debugStage << "KPrPageLayout::operator< 2" << false;
        return false;
        // sort of the different placeholders by position and type
    }
    debugStage << "KPrPageLayout::operator< 3" << ( m_placeholders.size() < other.m_placeholders.size() );
    return m_placeholders.size() < other.m_placeholders.size();
}

bool comparePlaceholderByPosition( const KPrPlaceholder * p1, const KPrPlaceholder * p2 )
{
    return KPrPlaceholder::comparePosition( *p1,* p2 );
}

bool KPrPageLayout::compareByContent( const KPrPageLayout & pl1, const KPrPageLayout & pl2 )
{
    if ( pl1.m_placeholders.size() == pl2.m_placeholders.size() ) {
        QList<KPrPlaceholder *> placeholders( pl1.m_placeholders );
        QList<KPrPlaceholder *> otherPlaceholders( pl2.m_placeholders );
        std::sort( placeholders.begin(), placeholders.end(), comparePlaceholderByPosition );
        std::sort( otherPlaceholders.begin(), otherPlaceholders.end(), comparePlaceholderByPosition );

        QList<KPrPlaceholder *>::iterator it( placeholders.begin() );
        QList<KPrPlaceholder *>::iterator otherIt( otherPlaceholders.begin() );

        for ( ; it != placeholders.end(); ++it, ++otherIt ) {
            QString presentationObject1 = ( *it )->presentationObject();
            QString presentationObject2 = ( *otherIt )->presentationObject();
            if ( presentationObject1 == presentationObject2 ) {
                continue;
            }
            return presentationObject1 < presentationObject2;
        }
        return false;
    }
    return pl1.m_placeholders.size() < pl2.m_placeholders.size();
}
