/* This file is part of the KOffice project
 * Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>
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

#ifndef SCRIPTING_STYLE_H
#define SCRIPTING_STYLE_H

#include <QObject>
//#include <QMetaObject>
//#include <QMetaEnum>
#include <KoParagraphStyle.h>
#include <KoListStyle.h>

namespace Scripting {

    /**
    *
    */
    class Style : public QObject
    {
            Q_OBJECT

            //Q_ENUMS(KoParagraphStyle::Properties)
            //Q_ENUMS(KoParagraphStyle::BorderStyle)
            Q_ENUMS(ListStyle)

        public:
            Style(QObject* parent, KoParagraphStyle* style)
                : QObject( parent ), m_style( style ) {}
            virtual ~Style() {}
            KoParagraphStyle* style() { return m_style; }

            enum ListStyle {
                NoItem = KoListStyle::NoItem,
                SquareItem = KoListStyle::SquareItem,
                DiscItem = KoListStyle::DiscItem,
                CircleItem = KoListStyle::CircleItem,
                DecimalItem = KoListStyle::DecimalItem,
                AlphaLowerItem = KoListStyle::AlphaLowerItem,
                AlphaUpperItem = KoListStyle::UpperAlphaItem,
                RomanLowerItem = KoListStyle::RomanLowerItem,
                RomanUpperItem = KoListStyle::UpperRomanItem,
            };

        public Q_SLOTS:

            const QString& name() const { return m_style->name(); }
            void setName(const QString& name) { m_style->setName(name); }

            void setListStyle(int liststyle) {
                if( m_style->listStyle() )
                    m_style->listStyle()->setStyle( (KoListStyle::Style)liststyle );
                else {
                    KoListStyle s;
                    s.setStyle( (KoListStyle::Style)liststyle );
                    m_style->setListStyle(s);
                }
            }

        private:
            KoParagraphStyle* m_style;
    };

}

#endif
