/* This file is part of the KDE project
 * Copyright (C) 2001 David Faure <faure@kde.org>
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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

#ifndef KWVIEWMODEPREVIEW_H
#define KWVIEWMODEPREVIEW_H

#include "KWViewMode.h"

#include <QString>

/**
 * A mode for previewing the overall document
 * Pages are organized in a grid (mostly useful with low zoom levels)
 */
class KWViewModePreview : public KWViewMode
{
public:
    /// constructor
    explicit KWViewModePreview();
    ~KWViewModePreview() {}

    virtual QPointF documentToView(const QPointF &point) const;
    virtual QPointF viewToDocument(const QPointF &point) const;
    virtual QSizeF contentsSize() const;

    /**
     * The preview can show several pages in a row for easy overview.
     * You can set the preferred pages per row here.
     * @param num the new number of pages per row
     */
    void setPagesPerRow(int num) {
        m_pagesPerRow = num;
    }
    /**
     * @return the number of pages we are showing per row
     */
    int pagesPerRow() const {
        return m_pagesPerRow;
    }

    /// return a string identification of this viewMode
    static const QString viewMode() {
        return "ModePreview";
    }
    /// return a string identification of this viewMode
    const QString type() const {
        return KWViewModePreview::viewMode();
    }
    QList<ViewMap> clipRectToDocument(const QRect &viewRect) const;

    /// set the gap between the pages
    void setGap(int gap);

protected:
    void updatePageCache();

private:
    int m_pagesPerRow;
};

#endif
