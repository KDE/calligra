/* This file is part of the KDE project
 * Copyright (C) 2009 Jan Hambrecht <jaham@gmx.net>
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

#include "SvgCssHelper.h"
#include <KoXmlReader.h>
#include <KDebug>

SvgCssHelper::SvgCssHelper()
{
}

SvgCssHelper::~SvgCssHelper()
{
}

void SvgCssHelper::parseStylesheet(const KoXmlElement &e)
{
    if (!e.childNodesCount())
        return;

    KoXmlNode c = e.firstChild();
    if (c.isCDATASection()) {
        KoXmlCDATASection cdata = c.toCDATASection();
        QString data = cdata.data().simplified();
        QStringList defs = data.split('}', QString::SkipEmptyParts);
        for (int i = 0; i < defs.count(); ++i) {
            QStringList def = defs[i].split('{');
            if( def.count() != 2 )
                continue;
            QString selector = def[0].simplified();
            if (selector.isEmpty())
                break;
            QString style = def[1].simplified();
            if (style.isEmpty())
                break;
            QStringList selectors = selector.split(',', QString::SkipEmptyParts);
            for (int i = 0; i < selectors.count(); ++i ) {
                kDebug(30514) << selectors[i] << style;
                m_cssStyles[selectors[i].simplified()] = style;
            }
        }
    }
}

QStringList SvgCssHelper::matchStyles(const KoXmlElement &e) const
{
    QStringList cssStyles;

    if (m_cssStyles.count()) {
        // match universal selector
        if (m_cssStyles.contains("*")) {
            cssStyles.append(m_cssStyles["*"]);
        }

        // match type selector
        if (m_cssStyles.contains(e.tagName())) {
            cssStyles.append(m_cssStyles[e.tagName()]);
        }

        // match class selector
        QString classAttribute = e.attribute("class").simplified();
        if (!classAttribute.isEmpty()) {
            QStringList cssClasses = classAttribute.split(' ', QString::SkipEmptyParts);
            foreach(const QString &cssClass, cssClasses) {
                if (m_cssStyles.contains('.'+cssClass)) {
                    cssStyles.append(m_cssStyles['.'+cssClass]);
                }
            }
        }

        // match id selector
        QString idAttribute = e.attribute("id").simplified();
        if (!idAttribute.isEmpty()) {
            if (m_cssStyles.contains('#'+idAttribute)) {
                cssStyles.append(m_cssStyles['#'+idAttribute]);
            }
        }
    }

    return cssStyles;
}
