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

#ifndef SCRIPTING_PAGE_H
#define SCRIPTING_PAGE_H

#include <QObject>
#include <KWPage.h>

#include "Module.h"

namespace Scripting {

    /**
    * The Page class represents a printed page in the document.
    */
    class Page : public QObject
    {
            Q_OBJECT
        public:
            Page( Module* module, KWPage* page )
                : QObject( module ), m_page( page ) {}
            virtual ~Page() {}

        public Q_SLOTS:

            /** Return the number of this page as it will be shown to the user. */
            int pageNumber() const { return m_page->pageNumber(); }

            /** Return the pageside of this page. The string could be one of the following;
                \li "Left" for left page used for even-numbered pages.
                \li "Right" for right page used for odd numbered pages.
                \li "Spread" for page spread which is one page that represents 2 pagenumbers.
            */
            QString pageSide() const {
                switch( m_page->pageSide() ) {
                    case KWPage::Left: return "Left";
                    case KWPage::Right: return "Right";
                    case KWPage::PageSpread: return "Spread";
                }
                return QString();
            }
            /** Set the pageside of this page. See the pageSide() method above for a
            list of valid arguments. */
            void setPageSide(const QString& ps) {
                if(ps == "Left") m_page->setPageSide(KWPage::Left);
                else if(ps == "Right") m_page->setPageSide(KWPage::Right);
                else if(ps == "Spread") m_page->setPageSide(KWPage::PageSpread);
            }

            /** Return the width of this page in pt. */
            double width() const { return m_page->width(); }
            /** Set the width of the page in pt. */
            void setWidth(double x) { m_page->setWidth(x); }

            /** Return the height of this page in pt. */
            double height() const { return m_page->height(); }
            /** Set the height of the page in pt. */
            void setHeight(double y) { m_page->setHeight(y); }

            /** Return the height of the margin at top in pt. */
            double topMargin() const { return m_page->topMargin(); }
            /** Set height of the top margin in pt. */
            void setTopMargin(double x) { m_page->setTopMargin(x); }

            /** Return the height of the margin at bottom in pt. */
            double bottomMargin() const { return m_page->bottomMargin(); }
            /** Set height of the bottom margin in pt. */
            void setBottomMargin(double y) { m_page->setBottomMargin(y); }

            /** Return the width of the margin at left in pt. */
            double leftMargin() const { return m_page->leftMargin(); }
            /** Set width of the left margin in pt. */
            void setLeftMargin(double l) { m_page->setLeftMargin(l); }

            /** Return the width of the margin at right in pt. */
            double rightMargin() const { return m_page->rightMargin(); }
            /** Set width of the right margin in pt. */
            void setRightMargin(double r) { m_page->setRightMargin(r); }

        private:
            KWPage* m_page;
    };

}

#endif
