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
    * The TextFormat class represents a format object that is used
    * for things like the background- or the foreground-color.
    */
    class TextFormat : public QObject
    {
            Q_OBJECT
        public:
            explicit TextFormat(QObject* parent) : QObject(parent) {}
            virtual ~TextFormat() {}
            virtual QTextFormat* format() const = 0;

        public Q_SLOTS:

            /** Return the background-color. */
            virtual QString backgroundColor() = 0;
            /** Set the background-color. */
            virtual void setBackgroundColor(const QString& color) = 0;

            /** Return the foreground-color. */
            virtual QString foregroundColor() = 0;
            /** Set the foreground-color. */
            virtual void setForegroundColor(const QString& color) = 0;

            /** Return the layout-direction. */
            virtual QString layoutDirection() const = 0;
            /** Set the layout-direction. */
            virtual void setLayoutDirection(const QString& direction) = 0;

            /** Return a list of propertyId items. */
            virtual QVariantList properties() const = 0;
            /** Return the value of the property defined with the \p propertyId identifier. */
            virtual QVariant property(int propertyId) const = 0;
            /** Set the value of the property defined with the \p propertyId identifier. */
            virtual void setProperty(int propertyId, const QVariant& value) = 0;

    };

    /**
    * \internal template class that inherits TextFormat to provide an
    * implementation for QTextFormat and classes inheriting from it.
    */
    template<class T>
    class TextFormatImp : public TextFormat
    {
        public:
            TextFormatImp(QObject* parent, const T& format)
                : TextFormat( parent ), m_format( format ) {}
            virtual ~TextFormatImp() {}
            virtual QTextFormat* format() const {
                QTextFormat* f = dynamic_cast<QTextFormat*>( &m_format );
                Q_ASSERT(f);
                return f;
            }

        private:

            QString backgroundColor() {
                return m_format.background().color().name();
            }
            void setBackgroundColor(const QString& color) {
                QBrush brush = m_format.background();
                QColor c(color);
                if( c.isValid() ) {
                    brush.setColor(c);
                    m_format.setBackground(brush);
                }
            }

            QString foregroundColor() {
                return m_format.foreground().color().name();
            }
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
            QVariant property(int propertyId) const {
                return m_format.property(propertyId);
            }
            void setProperty(int propertyId, const QVariant& value) {
                m_format.setProperty(propertyId, value);
            }

        private:
            T m_format;
    };
}

#endif
