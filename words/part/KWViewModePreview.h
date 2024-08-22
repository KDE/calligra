/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2001 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWVIEWMODEPREVIEW_H
#define KWVIEWMODEPREVIEW_H

#include "KWViewMode.h"
#include "words_export.h"

#include <QString>

/**
 * A mode for previewing the overall document
 * Pages are organized in a grid (mostly useful with low zoom levels)
 */
class WORDS_EXPORT KWViewModePreview : public KWViewMode
{
    Q_OBJECT
public:
    /// constructor
    explicit KWViewModePreview();
    ~KWViewModePreview() override = default;

    QPointF documentToView(const QPointF &point, KoViewConverter *viewConverter) const override;
    QPointF viewToDocument(const QPointF &point, KoViewConverter *viewConverter) const override;
    QSizeF contentsSize() const override;

    /**
     * The preview can show several pages in a row for easy overview.
     * You can set the preferred pages per row here.
     * @param num the new number of pages per row
     */
    void setPagesPerRow(int num)
    {
        m_pagesPerRow = num;
    }
    /**
     * @return the number of pages we are showing per row
     */
    int pagesPerRow() const
    {
        return m_pagesPerRow;
    }

    /// return a string identification of this viewMode
    static const QString viewMode()
    {
        return "ModePreview";
    }
    /// return a string identification of this viewMode
    const QString type() const override
    {
        return KWViewModePreview::viewMode();
    }
    QVector<ViewMap> mapExposedRects(const QRectF &viewRect, KoViewConverter *viewConverter) const override;

    /// set the gap between the pages
    void setGap(int gap);

protected:
    void updatePageCache() override;

private:
    int m_pagesPerRow;
};

#endif
