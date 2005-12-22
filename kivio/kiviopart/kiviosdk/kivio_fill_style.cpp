/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "kivio_common.h"
#include "kivio_fill_style.h"

#include <kdebug.h>

/**
 * Default constructor
 *
 * Sets the color style to solid, the color to white, and the brush to a
 * a solid pattern.  It also sets the gradient pointer to null.
 */
KivioFillStyle::KivioFillStyle()
{
    m_colorStyle = kcsSolid;
    m_color = QColor(255,255,255);
    m_color2 = QColor(255,255,255);
    m_brushStyle = QBrush::SolidPattern;
    m_gradientType = KImageEffect::VerticalGradient;
}



/**
 * Copy constrcutor
 *
 * @param source The source object to copy from.
 *
 * Copies all attributes from source, to this object.
 */
KivioFillStyle::KivioFillStyle( const KivioFillStyle &source )
{
    m_colorStyle = source.colorStyle();
    m_color = source.color();
    m_brushStyle = source.brushStyle();
    m_gradientType = source.gradientType();
    m_color2 = source.color2();
}



/**
 * Copies this objects attributes into a target.
 *
 * @param pTarget The target object to copy into.
 *
 * Copies all attributes of this fillstyle into pTarget.
 */
void KivioFillStyle::copyInto( KivioFillStyle *pTarget ) const
{
    if( !pTarget )
        return;

    pTarget->setKivioColorStyle(m_colorStyle);
    pTarget->setColor(m_color);
    pTarget->setBrushStyle(m_brushStyle);
    pTarget->setGradientType(m_gradientType);
    pTarget->setColor2(m_color2);
}


/**
 * Load this object from an XML element
 *
 * @param e The element to load from
 */
bool KivioFillStyle::loadXML( const QDomElement &e )
{
  QDomElement ele;
  QDomNode node;

  m_color = XmlReadColor( e, "color", QColor(255,255,255).rgb() );
  m_color2 = XmlReadColor( e, "gradientColor", QColor(255,255,255).rgb() );

  m_colorStyle = static_cast<KivioColorStyle>(XmlReadInt( e, "colorStyle", kcsSolid ));

  m_gradientType = static_cast<KImageEffect::GradientType>(XmlReadInt(e, "gradientType", KImageEffect::VerticalGradient));

  return true;
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
  QDomElement e = doc.createElement("KivioFillStyle");

  XmlWriteColor( e, "color", m_color );
  XmlWriteColor( e, "gradientColor", m_color2 );

  XmlWriteInt( e, "colorStyle", static_cast<int>(m_colorStyle) );

  XmlWriteInt( e, "gradientType", static_cast<int>(m_gradientType) );

  return e;
}

QBrush KivioFillStyle::brush()
{
  QBrush b;
  b.setColor(m_color);

  switch(m_colorStyle) {
    case kcsSolid:
      b.setStyle(m_brushStyle);
      break;

    case kcsNone:
      b.setStyle(QBrush::NoBrush);
      break;

    case kcsGradient:
    case kcsPixmap:
    default:
      break;
  }

  return b;
}
