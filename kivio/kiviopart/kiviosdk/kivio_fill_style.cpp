/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "kivio_common.h"
#include "kivio_fill_style.h"
#include "kivio_gradient.h"

/**
 * Default constructor
 *
 * Sets the color style to solid, the color to white, and the brush to a
 * a solid pattern.  It also sets the gradient pointer to null.
 */
KivioFillStyle::KivioFillStyle()
    : m_pGradient(NULL)
{
    m_colorStyle = kcsSolid;
    m_color = QColor(255,255,255);
    m_brushStyle = QBrush::SolidPattern;
    m_pGradient = NULL;             // FIXME
}



/**
 * Destructor
 *
 * If we have a gradient, it destroys it.
 * FIXME: Should we keep all gradients in a central repository and have
 * the stencils simply point to them?  If so, then we shouldn't delete it
 * here, only set the pointer to null.
 */
KivioFillStyle::~KivioFillStyle()
{
    if( m_pGradient )
    {
        delete m_pGradient;
        m_pGradient = NULL;
    }
}



/**
 * Copy constrcutor
 *
 * @param source The source object to copy from.
 *
 * Copies all attributes from source, to this object.
 * FIXME: Gradient.. should it just be a pointer? See the destructor
 * for more details.
 */
KivioFillStyle::KivioFillStyle( const KivioFillStyle &source )
    : m_pGradient(NULL)
{
    m_colorStyle = source.m_colorStyle;
    m_color = source.m_color;
    m_brushStyle = m_brushStyle;
    
    m_pGradient = new KivioGradient( *(source.m_pGradient) );
}



/**
 * Copies this objects attributes into a target.
 *
 * @param pTarget The target object to copy into.
 *
 * Copies all attributes of this fillstyle into pTarget.
 * FIXME: gradient
 */
void KivioFillStyle::copyInto( KivioFillStyle *pTarget ) const
{
    if( !pTarget )
        return;

    pTarget->m_colorStyle = m_colorStyle;
    pTarget->m_color = m_color;
    pTarget->m_brushStyle = m_brushStyle;

    if( m_pGradient && pTarget->m_pGradient )
        m_pGradient->copyInto( pTarget->m_pGradient );
}


/**
 * Load this object from an XML element
 *
 * @param e The element to load from
 *
 * FIXME: Doesn't load gradient information.
 */
bool KivioFillStyle::loadXML( const QDomElement &e )
{
    QDomElement ele;
    QDomNode node;

    m_color = XmlReadColor( e, "color", QColor(255,255,255).rgb() );

    m_colorStyle = (KivioColorStyle)XmlReadInt( e, "colorStyle", kcsSolid );

    return false;
}



/**
 * Save this object to an XML element
 *
 * @param doc The document to create nodes for.
 *
 * @returns QDomElement is returned.
 */
QDomElement KivioFillStyle::saveXML( QDomDocument &doc )
{
    debug("+SAVE KivioFillStyle");
    // FIXME: make this complete.  It's incomplete because
    // kivio does not yet support gradients

    QDomElement e = doc.createElement("KivioFillStyle");

    XmlWriteColor( e, "color", m_color );

    debug("************+++++++++++ Writing colorstyle %d", (int)m_colorStyle );
    XmlWriteInt( e, "colorStyle", (int)m_colorStyle );

    return e;
}