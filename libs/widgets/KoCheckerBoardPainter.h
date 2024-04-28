/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCHECKERBOARDPAINTER_H
#define KOCHECKERBOARDPAINTER_H

#include "kowidgets_export.h"
#include <QColor>
#include <QPixmap>

class QPainter;

class KOWIDGETS_EXPORT KoCheckerBoardPainter
{
public:
    explicit KoCheckerBoardPainter(int checkerSize);
    void setCheckerColors(const QColor &lightColor, const QColor &darkColor);
    void setCheckerSize(int checkerSize);
    void paint(QPainter &painter, const QRectF &rect) const;

private:
    void createChecker();
    int m_checkerSize;
    QPixmap m_checker;
    QColor m_lightColor;
    QColor m_darkColor;
};

#endif // KOCHECKERBOARDPAINTER_H
