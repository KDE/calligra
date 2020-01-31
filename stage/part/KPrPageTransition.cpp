/* This file is part of the KDE project
 * Copyright (C) 2020 Dag Andersen <danders@get2net.dk>
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

#include "KPrPageTransition.h"

#include <KoXmlReader.h>
#include <KoGenStyle.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoXmlNS.h>


KPrPageTransition::KPrPageTransition()
: m_type(Manual)
, m_duration(0.0)
{
    m_odfNames[Manual] = "manual";
    m_odfNames[Automatic] = "automatic";
    m_odfNames[SemiAutomatic] = "semi-automatic";
}

KPrPageTransition::Type KPrPageTransition::type() const
{
    return m_type;
}

void KPrPageTransition::setType(KPrPageTransition::Type type)
{
    m_type = type;
}

QString KPrPageTransition::odfName() const
{
    return m_odfNames[m_type];
}

qreal KPrPageTransition::duration() const
{
    return m_duration;
}

void KPrPageTransition::setDuration(qreal duration)
{
    m_duration = duration;
}

int KPrPageTransition::milliseconds() const
{
    return m_duration * 1000;
}

void KPrPageTransition::saveOdfAttributes(KoGenStyle &style) const
{
    style.addProperty("presentation:transition-type", odfName());
    style.addProperty("presentation:duration", QString("PT%1S").arg(QString::number(m_duration)));
}

bool KPrPageTransition::loadOdfAttributes(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    KoOdfStylesReader& stylesReader = context.odfLoadingContext().stylesReader();
    const KoXmlElement * styleElement = stylesReader.findContentAutoStyle( element.attributeNS( KoXmlNS::draw, "style-name" ), "drawing-page" );
    if ( styleElement ) {
        KoXmlElement element = styleElement->namedItemNS( KoXmlNS::style, "drawing-page-properties" ).toElement();
        if (!element.isNull()) {
            if (element.hasAttributeNS(KoXmlNS::presentation, "transition-type")) {
                m_type = m_odfNames.key(element.attributeNS(KoXmlNS::presentation, "transition-type"));
            }
            if (element.hasAttributeNS(KoXmlNS::presentation, "duration")) {
                // NOTE: This is kept very simple as duration is specified as 'PTnS' (by LO and us)
                // In general it can conform to the pattern: 'PnYndMnDTnHnMnS'
                // However, as only seconds are actually used, reduces to 'PTnS'
                // E.g. 'PT3.5S' for tree and a half seconds
                m_duration = element.attributeNS(KoXmlNS::presentation, "duration").remove("PT").remove('S').toDouble();
            }
        }
    }
    return true;
}

QDebug operator<<(QDebug dbg, const KPrPageTransition &t)
{
    return dbg.noquote().nospace() << "KPrPageTransition[" << t.odfName() << ',' << t.duration() << ']';
}
