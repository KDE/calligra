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

#ifndef SCRIPTING_TEXTFORMAT_H
#define SCRIPTING_TEXTFORMAT_H

#include <QObject>
#include <QTextFormat>

namespace Scripting {

    /**
    *
    */
    class TextFormat : public QObject
    {
            Q_OBJECT
        public:
            TextFormat(QObject* parent, const QTextFormat& format)
                : QObject( parent ), m_format( format ) {}
            virtual ~TextFormat() {}
            const QTextFormat& format() const { return m_format; }

        public Q_SLOTS:

            QString backgroundColor() { return m_format.background().color().name(); }
            void setBackgroundColor(const QString& color) {
                QBrush brush = m_format.background();
                QColor c(color);
                if( c.isValid() ) {
                    brush.setColor(c);
                    m_format.setBackground(brush);
                }
            }

            QString foregroundColor() { return m_format.foreground().color().name(); }
            void setForegroundColor(const QString& color) {
                QBrush brush = m_format.foreground();
                QColor c(color);
                if( c.isValid() ) {
                    brush.setColor(c);
                    m_format.setForeground(brush);
                }
            }

            QString layoutDirection() const {
                switch( m_format.layoutDirection() ) {
                    case Qt::LeftToRight: return "LeftToRight";
                    case Qt::RightToLeft: return "RightToLeft";
                }
                return QString();
            }
            void setLayoutDirection(const QString& direction) {
                if(direction == "LeftToRight") m_format.setLayoutDirection(Qt::LeftToRight);
                else if(direction == "RightToLeft") m_format.setLayoutDirection(Qt::RightToLeft);
            }

            QVariantList properties() const {
                QVariantList list;
                QMap<int,QVariant>::Iterator it( m_format.properties().begin() ), end( m_format.properties().end() );
                for(; it != end; ++it) list.append(it.key());
                return list;
            }
            QVariant property(int propertyId) const { return m_format.property(propertyId); }
            void setProperty(int propertyId, const QVariant& value) { m_format.setProperty(propertyId, value); }

        private:
            QTextFormat m_format;
    };

}

#endif
