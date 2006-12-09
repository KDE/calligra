/* This file is part of the KOffice project
 * Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; version 2.

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
#include <koffice_export.h>

class KWPage;

namespace Scripting {

    /**
    * The Page class represents a printed page of the document.
    */
    class Page : public QObject
    {
            Q_OBJECT
        public:
            explicit Page( QObject* parent, KWPage* page );
            ~Page();

        public Q_SLOTS:

            /** Return the number of this page as it will be shown to the user. */
            int pageNumber() const;

            /** Return the pageside of this page. The string could be one of the following;
                \li "Left" for left page used for even-numbered pages.
                \li "Right" for right page used for odd numbered pages.
                \li "Spread" for page spread which is one page that represents 2 pagenumbers.
            */
            QString pageSide() const;
            /** Set the pageside of this page. See the pageSide() method above for a
            list of valid arguments. */
            void setPageSide(const QString& ps);

            /** Return the width of this page in pt. */
            double width() const;
            /** Set the width of the page in pt. */
            void setWidth(double x);

            /** Return the height of this page in pt. */
            double height() const;
            /** Set the height of the page in pt. */
            void setHeight(double y);

            /** Return the height of the margin at top in pt. */
            double topMargin() const;
            /** Set height of the top margin in pt. */
            void setTopMargin(double x);

            /** Return the height of the margin at bottom in pt. */
            double bottomMargin() const;
            /** Set height of the bottom margin in pt. */
            void setBottomMargin(double y);

            /** Return the width of the margin at left in pt. */
            double leftMargin() const;
            /** Set width of the left margin in pt. */
            void setLeftMargin(double l);

            /** Return the width of the margin at right in pt. */
            double rightMargin() const;
            /** Set width of the right margin in pt. */
            void setRightMargin(double r);

        private:
            KWPage* m_page;
    };

}

#endif
