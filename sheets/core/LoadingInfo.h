/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPLOADINGINFO_H
#define KSPLOADINGINFO_H

#include "engine/calligra_sheets_limits.h"

#include <QMap>
#include <QPoint>
#include <QPointF>

namespace Calligra
{
namespace Sheets
{
class SheetBase;

/// Temporary information used only during loading
class LoadingInfo
{
public:
    enum FileFormat {
        OpenDocument,
        NativeFormat,
        Gnumeric,
        Unknown
    };

    LoadingInfo()
        : m_fileFormat(Unknown)
        , m_initialActiveSheet(nullptr)
        , m_loadTemplate(false)
    {
    }
    ~LoadingInfo() = default;

    FileFormat fileFormat() const
    {
        return m_fileFormat;
    }
    void setFileFormat(FileFormat format)
    {
        m_fileFormat = format;
    }

    SheetBase *initialActiveSheet() const
    {
        return m_initialActiveSheet;
    }
    void setInitialActiveSheet(SheetBase *sheet)
    {
        m_initialActiveSheet = sheet;
    }

    /**
     * @return the cursor positions
     */
    const QMap<SheetBase *, QPoint> &cursorPositions() const
    {
        return m_cursorPositions;
    }

    /**
     * Stores the cursor position @p point for @p sheet .
     */
    void setCursorPosition(SheetBase *sheet, const QPoint &point)
    {
        Q_ASSERT(1 <= point.x() && point.x() <= KS_colMax);
        Q_ASSERT(1 <= point.y() && point.y() <= KS_rowMax);
        m_cursorPositions.insert(sheet, point);
    }

    /**
     * @return scrolling offsets
     */
    const QMap<SheetBase *, QPointF> &scrollingOffsets() const
    {
        return m_scrollingOffsets;
    }

    /**
     * Stores the scrolling offset @p point for @p sheet .
     */
    void setScrollingOffset(SheetBase *sheet, const QPointF &point)
    {
        m_scrollingOffsets.insert(sheet, point);
    }

    void setLoadTemplate(bool _b)
    {
        m_loadTemplate = _b;
    }
    bool loadTemplate() const
    {
        return m_loadTemplate;
    }

private:
    FileFormat m_fileFormat;
    SheetBase *m_initialActiveSheet;
    QMap<SheetBase *, QPoint> m_cursorPositions;
    QMap<SheetBase *, QPointF> m_scrollingOffsets;
    bool m_loadTemplate;
};

} // namespace Sheets
} // namespace Calligra

#endif /* KPRLOADINGINFO_H */
