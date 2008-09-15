/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#include "KPrPlaceholders.h"

#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoShapeLayer.h>
#include <KoPADocument.h>
#include "KPrPageLayout.h"
#include "KPrPlaceholder.h"
#include "KPrPlaceholderShape.h"

KPrPlaceholders::KPrPlaceholders()
:m_layout( 0 )
{
}

KPrPlaceholders::~KPrPlaceholders()
{
}

void KPrPlaceholders::setLayout( KPrPageLayout * layout, KoPADocument * document, const QList<KoShape *> & shapes, const QSizeF & pageSize )
{
    Q_ASSERT( m_initialized );
    m_layout = layout;

    Q_ASSERT( !shapes.isEmpty() );
    KoShapeLayer * layer = dynamic_cast<KoShapeLayer*>( shapes[0] );

    QMap<QString, QList<QRectF> > placeholders;
    if ( layout ) {
        foreach ( KPrPlaceholder * placeholder, layout->placeholders() ) {
            placeholders[placeholder->presentationObject()].append( placeholder->rect( pageSize ) );
        }
    }

    Placeholders::iterator it( m_placeholders.begin() );

    while ( it != m_placeholders.end() ) {
        Placeholders::iterator next( it );
        ++next;
        QMap<QString, QList<QRectF> >::iterator itPlaceholder( placeholders.find( it->presentationClass ) );
        if ( itPlaceholder != placeholders.end() && !itPlaceholder.value().isEmpty() ) {
            QRectF rect = itPlaceholder.value().takeFirst();
            if ( itPlaceholder.value().isEmpty() ) {
                placeholders.erase( itPlaceholder );
            }
            // shape reposition
            // TODO command
            it->shape->setSize( rect.size() );
            it->shape->setPosition( rect.topLeft() );
        }
        else {
            if ( it->isPlaceholder ) {
                // shape remove
                // TODO command
                // it is already deleted by the shapeRemoved it = m_placeholders.erase( it );
                // this is done as it gets deleted by shapeRemoved
                Placeholders::iterator next( it );
                ++next;
                document->removeShape( it->shape );
            }
        }
        it = next;
    }

    QMap<QString, QList<QRectF> >::const_iterator itPlaceholder( placeholders.begin() );
    for ( ; itPlaceholder != placeholders.end(); ++itPlaceholder ) {
        const QList<QRectF> & list( itPlaceholder.value() );
        QList<QRectF>::const_iterator listIt( list.begin() );
        for ( ; listIt != list.end(); ++listIt ) {
             KoShape * shape = new KPrPlaceholderShape();
             shape->setAdditionalAttribute( "presentation:placeholder", "true" );
             shape->setAdditionalAttribute( "presentation:class", itPlaceholder.key() );
             shape->setSize( ( * listIt ).size() );
             shape->setPosition( ( * listIt ).topLeft() );
             shape->setParent( layer );
             document->addShape( shape );
             // TODO command
        }
    }
}

void KPrPlaceholders::init( KPrPageLayout * layout, const QList<KoShape *> & shapes )
{
    m_layout = layout;
    add( shapes );

    m_initialized = true;
}

KPrPageLayout * KPrPlaceholders::layout() const
{
    Q_ASSERT( m_initialized );
    return m_layout;
}

void KPrPlaceholders::shapeAdded( KoShape * shape )
{
    Q_ASSERT( m_initialized );
// if presentation:class add to index no matter if it is a placeholder or not
    QString presentationClass = shape->additionalAttribute( "presentation:class" );
    QString placeholder = shape->additionalAttribute( "presentation:placeholder" );
    if ( !presentationClass.isNull() ) {
        m_placeholders.get<1>().insert( Placeholder( presentationClass, shape, placeholder == "true" ) );
    }
}

void KPrPlaceholders::shapeRemoved( KoShape * shape )
{
    Q_ASSERT( m_initialized );
// if it is a placeholder remove it
// if presentation:class is set and not a placeholder remove it and add a placeholder
//    this needs to be checked as on undo/redo we might get a problem
// other do nothing
    QString presentationClass = shape->additionalAttribute( "presentation:class" );
    if ( !presentationClass.isNull() ) {
        // TODO m_placeholders.erase()
        PlaceholdersByShape::iterator it( m_placeholders.get<2>().find( shape ) );
        if ( it != m_placeholders.get<2>().end() ) {
            m_placeholders.get<2>().erase( it );
        }
    }
}

void KPrPlaceholders::add( const QList<KoShape *> & shapes )
{
    foreach( KoShape* shape, shapes ) {
        QString presentationClass = shape->additionalAttribute( "presentation:class" );
        QString placeholder = shape->additionalAttribute( "presentation:placeholder" );
        if ( !presentationClass.isNull() ) {
            m_placeholders.get<1>().insert( Placeholder( presentationClass, shape, placeholder == "true" ) );
        }
        KoShapeContainer* container = dynamic_cast<KoShapeContainer*>( shape );
        if ( container ) {
            add( container->iterator() );
        }
    }
}
