/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "PaddedElement.h"

PaddedElement::PaddedElement(BasicElement *parent)
    : RowElement(parent)
{
}

void PaddedElement::layout(const AttributeManager *am)
{
    Q_UNUSED(am)
}

ElementType PaddedElement::elementType() const
{
    return Padded;
}

/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
/*
void PaddedElement::calcSizes( const ContextStyle& context,
                               ContextStyle::TextStyle tstyle,
                               ContextStyle::IndexStyle istyle,
                               StyleAttributes& style )
{
    qreal factor = style.sizeFactor();

    luPixel width = 0;
    luPixel height = 0;
    luPixel depth = 0;
    luPixel spaceBefore = 0;

    if ( !isEmpty() ) {
        // First, get content height and width
        for ( iterator it = begin(); it != end(); ++it ) {
            if ( it == begin() ) {
                spaceBefore = context.ptToPixelX( getSpaceBefore( context, tstyle, factor ) );
            }
            it->calcSizes( context, tstyle, istyle, style );
            width += it->getWidth() + spaceBefore;
            luPixel baseline = it->getBaseline();
            if ( baseline > -1 ) {
                height = qMax( height, baseline );
                depth = qMax( depth, it->getHeight() - baseline );
            }
            else {
                luPixel bl = it->getHeight()/2 + context.axisHeight( tstyle, factor );
                height = qMax( height, bl );
                depth = qMax( depth, it->getHeight() - bl );
            }
        }
    }
    else {
        width = context.getEmptyRectWidth( factor );
        height = context.getEmptyRectHeight( factor );
        depth = 0;
    }

    luPixel left = calcSize( context, m_lspaceType, m_lspaceRelative, m_lspace, width, height, 0 );
    luPixel right = calcSize( context, m_widthType, m_widthRelative, m_width, width, height, width ) + left;
    luPixel down = calcSize( context, m_depthType, m_depthRelative, m_depth, width, height, depth );
    luPixel up = calcSize( context, m_heightType, m_heightRelative, m_height, width, height, height );

    // Check borders
    if ( right < 0 ) right = 0;
    if ( up + down < 0 ) up = down = 0;

    if ( ! isEmpty() ) {
        width = left;
        // Let's do all normal elements that have a base line.
        for ( iterator it = begin(); it != end(); ++it ) {
            luPixel spaceBefore = 0;
            if ( it == begin() ) {
                spaceBefore = context.ptToPixelX( getSpaceBefore( context, tstyle, factor ) );
            }
            it->calcSizes( context, tstyle, istyle, style );
            it->setX( width + spaceBefore );
            width += it->getWidth() + spaceBefore;
        }

        setWidth( right );
        setHeight( up + down );
        setBaseline( up );
        setChildrenPositions();
    }
    else {
        setWidth( right );
        setHeight( up + down );
        setBaseline( up );
    }
}
*/
