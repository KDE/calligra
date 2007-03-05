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

#ifndef SCRIPTING_PAGELAYOUT_H
#define SCRIPTING_PAGELAYOUT_H

#include <QObject>
#include <KWPage.h>
#include <KoPageLayout.h>

namespace Scripting {

    /**
    * The layout of a \a Page object that represents a printed page in
    * the document.
    */
    class PageLayout : public QObject
    {
            Q_OBJECT
        public:
            PageLayout( QObject* parent, const KoPageLayout& pagelayout )
                : QObject( parent ), m_pagelayout( pagelayout ) {}
            virtual ~PageLayout() {}
            const KoPageLayout& pageLayout() const { return m_pagelayout; }

        public Q_SLOTS:

            /** Page format */
            //KoFormat format;

            /** Return page orientation */
            QString orientation() {
                switch( m_pagelayout.orientation ) {
                    case KoPageFormat::Portrait: return "Portrait";
                    case KoPageFormat::Landscape: return "Landscape";
                }
                return QString();
            }
            /** Set page orientation */
            void setOrientation(const QString& orientation) {
                if(orientation == "Portrait") m_pagelayout.orientation = KoPageFormat::Portrait;
                else if(orientation == "Landscape") m_pagelayout.orientation = KoPageFormat::Landscape;
            }

            /** Return page width in pt */
            double width() { return m_pagelayout.width; }
            /** Set page width in pt */
            void setWidth(double width) { m_pagelayout.width = width; }

            /** Return page height in pt */
            double height() { return m_pagelayout.height; }
            /** Set page height in pt */
            void setHeight(double height) { m_pagelayout.height = height; }

            /** Return left margin in pt */
            double left() { return m_pagelayout.left; }
            /** Set left margin in pt */
            void setLeft(double left) { m_pagelayout.left = left; }

            /** Return right margin in pt */
            double right() { return m_pagelayout.right; }
            /** Set right margin in pt */
            void setRight(double right) { m_pagelayout.right = right; }

            /** Return top margin in pt */
            double top() { return m_pagelayout.top; }
            /** Set top margin in pt */
            void setTop(double top) { m_pagelayout.top = top; }

            /** Return bottom margin in pt */
            double bottom() { return m_pagelayout.bottom; }
            /** Set bottom margin in pt */
            void setBottom(double bottom) { m_pagelayout.bottom = bottom; }

            /** Return margin on page edge in pt */
            double pageEdge() { return m_pagelayout.pageEdge; }
            /** Set margin on page edge in pt */
            void setPageEdge(double edge) { m_pagelayout.pageEdge = edge; }

            /** Return margin on page-binding edge in pt */
            double bindingSide() { return m_pagelayout.bindingSide; }
            /** Set margin on page-binding edge in pt */
            void setBindingSide(double side) { m_pagelayout.bindingSide = side; }

        private:
            KoPageLayout m_pagelayout;
    };

}

#endif
