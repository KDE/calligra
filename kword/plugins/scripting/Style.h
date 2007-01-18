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
    * The Style class defines style-options and formatting functionality
    * for content. Styles and content are strictly separated and could
    * be manipulated detached from each other. This follows the common
    * idiom of separation of presentation and content.
    */
    class Style : public QObject
    {
            Q_OBJECT

            Q_ENUMS(Alignment)
            //Q_ENUMS(BorderStyle)
            Q_ENUMS(ListStyle)

        public:
            Style(QObject* parent, KoParagraphStyle* style)
                : QObject(parent), m_style(style) {}
            virtual ~Style() {}
            KoParagraphStyle* style() const { return m_style; }

            enum Alignment {
                AlignLeft = Qt::AlignLeft,
                AlignRight = Qt::AlignRight,
                AlignHCenter = Qt::AlignHCenter,
                AlignJustify = Qt::AlignJustify,
                AlignTop = Qt::AlignTop,
                AlignBottom = Qt::AlignBottom,
                AlignVCenter = Qt::AlignVCenter,
                AlignCenter = Qt::AlignCenter
            };

            /*
            enum BorderStyle {
                BorderNone = KoParagraphStyle::BorderNone,
                BorderDotted = KoParagraphStyle::BorderDotted,
                BorderDashed = KoParagraphStyle::BorderDashed,
                BorderSolid = KoParagraphStyle::BorderSolid,
                BorderDouble = KoParagraphStyle::BorderDouble,
                BorderGroove = KoParagraphStyle::BorderGroove,
                BorderRidge = KoParagraphStyle::BorderRidge,
                BorderInset = KoParagraphStyle::BorderInset,
                BorderOutset = KoParagraphStyle::BorderOutset,
                BorderDashDotPattern = KoParagraphStyle::BorderDashDotPattern,
                BorderDashDotDotPattern = KoParagraphStyle::BorderDashDotDotPattern
            };
            */

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
                BoxItem = KoListStyle::BoxItem
            };

        public Q_SLOTS:

            /***** Name *****/

            /** Return the user-visible name the style has. */
            QString name() const {
                return m_style->name();
            }

            /** Set the user-visible name the style has. */
            void setName(const QString& name) {
                m_style->setName(name);
            }

            /***** Alignment *****/

            /** Return the alignment the style has. */
            int alignment() const {
                return m_style->alignment();
            }

            /** Set the alignment the style has. */
            void setAlignment(int alignment) {
                m_style->setAlignment( (Qt::Alignment) alignment);
            }

            /***** Padding *****/

            /** Return the distance between text and border. */
            QRectF padding() const {
                return QRectF(m_style->leftPadding(), m_style->topPadding(), m_style->rightPadding(), m_style->bottomPadding());
            }

            /** Set the distance between text and border. */
            void setPadding(const QRectF& r) {
                m_style->setLeftPadding(r.x());
                m_style->setTopPadding(r.y());
                m_style->setRightPadding(r.width());
                m_style->setBottomPadding(r.height());
            }

            /***** Margin *****/

            /** Return the margin between text and border. */
            QRectF margin() const {
                return QRectF(m_style->leftMargin(), m_style->topMargin(), m_style->rightMargin(), m_style->bottomMargin());
            }

            /** Set the margin between text and border. */
            void setMargin(const QRectF& r) {
                m_style->setLeftMargin(r.x());
                m_style->setTopMargin(r.y());
                m_style->setRightMargin(r.width());
                m_style->setBottomMargin(r.height());
            }

/*TODO simplify border options even more. Propably just deal with a QVariantMap using QMetaEnum's, e.g.
            QVariantMap border() {
                QVariantMap map;
                for(int i = KoParagraphStyle::HasLeftBorder; i <= KoParagraphStyle::BottomBorderColor; i++)
                    map.insert("", m_style->property(""));
                return map;
            }
*/
#if 0
            /***** Border *****/

            QRect borderStyle() {
                return QRect(m_style->leftBorderStyle(), m_style->topBorderStyle(), m_style->rightBorderStyle(), m_style->bottomBorderStyle());
            }

            void setBorderStyle(const QRect& rect) {
                m_style->setLeftBorderStyle( (KoParagraphStyle::BorderStyle) r.x() );
                m_style->setTopBorderStyle( (KoParagraphStyle::BorderStyle) r.y() );
                m_style->setRightBorderStyle( (KoParagraphStyle::BorderStyle) r.width() );
                m_style->setBottomBorderStyle( (KoParagraphStyle::BorderStyle) r.height() );
            }

            QRect borderSpacing() {
                return QRect(m_style->leftBorderSpacing(), m_style->topBorderSpacing(), m_style->rightBorderSpacing(), m_style->bottomBorderSpacing());
            }

            void setBorderSpacing(const QRect& rect) {
                m_style->setLeftBorderSpacing(r.x());
                m_style->setTopBorderSpacing(r.y());
                m_style->setRightBorderSpacing(r.width());
                m_style->setBottomBorderSpacing(r.height());
            }

            QRect borderWidth() {
                return QRect(m_style->leftBorderWidth(), m_style->topBorderWidth(), m_style->rightBorderWidth(), m_style->bottomBorderWidth());
            }

            void setBorderWidth(const QRect& rect) {
                m_style->setLeftBorderWidth(r.x());
                m_style->setTopBorderWidth(r.y());
                m_style->setRightBorderWidth(r.width());
                m_style->setBottomBorderWidth(r.height());
            }

            QRect innerBorderWidth() {
                return QRect(m_style->leftInnerBorderWidth(), m_style->topInnerBorderWidth(), m_style->rightInnerBorderWidth(), m_style->bottomInnerBorderWidth());
            }

            void setInnerBorderWidth(const QRect& rect) {
                m_style->setLeftInnerBorderWidth(r.x());
                m_style->setTopInnerBorderWidth(r.y());
                m_style->setRightInnerBorderWidth(r.width());
                m_style->setBottomInnerBorderWidth(r.height());
            }

            QRect borderColor() {
                return QRect(m_style->leftBorderColor(), m_style->topBorderColor(), m_style->rightBorderColor(), m_style->bottomBorderColor());
            }

            void setBorderColor(const QRect& rect) {
                m_style->setLeftBorderColor(r.x());
                m_style->setTopBorderColor(r.y());
                m_style->setRightBorderColor(r.width());
                m_style->setBottomBorderColor(r.height());
            }
#endif

            /***** List *****/

            /** Return the style of listitems. */
            int listStyle(int liststyle) const {
                return m_style->listStyle() ? m_style->listStyle()->style() : 0;
            }

            /** Set the style of listitems. */
            void setListStyle(int liststyle) {
                if( m_style->listStyle() )
                    m_style->listStyle()->setStyle( (KoListStyle::Style)liststyle );
                else {
                    KoListStyle s;
                    s.setStyle( (KoListStyle::Style) liststyle );
                    m_style->setListStyle(s);
                }
            }

        private:
            KoParagraphStyle* m_style;
    };

}

#endif
