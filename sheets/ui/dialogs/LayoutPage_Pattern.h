/* This file is part of the KDE project
             SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
             SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>
             SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1999-2002 Harri Porten <porten@kde.org>
             SPDX-FileCopyrightText: 2000-2001 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>
             SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Stephan Kulow <coolo@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_LAYOUT_PAGE_PATTERN
#define CALLIGRA_SHEETS_LAYOUT_PAGE_PATTERN

#include <QFrame>
#include <QWidget>

class QPushButton;
class KColorButton;

namespace Calligra
{
namespace Sheets
{

class Style;

class BrushSelect : public QFrame
{
    Q_OBJECT
public:
    BrushSelect(QWidget *parent, const char *_name);

    void setBrushStyle(Qt::BrushStyle _pat)
    {
        brushStyle = _pat;
        repaint();
    }
    Qt::BrushStyle getBrushStyle() const
    {
        return brushStyle;
    }
    QColor getBrushColor() const
    {
        return brushColor;
    }
    void setBrushColor(const QColor &_c)
    {
        brushColor = _c;
    }
    void setPattern(const QColor &_color, Qt::BrushStyle _style);

Q_SIGNALS:
    void clicked(BrushSelect *_this);

public Q_SLOTS:
    void slotUnselect();
    void slotSelect();

protected:
    void paintEvent(QPaintEvent *_ev) override;
    void mousePressEvent(QMouseEvent *_ev) override;

    Qt::BrushStyle brushStyle;
    QColor brushColor;
    bool selected;
};

#define BRUSH_COUNT 15

/**
 * \ingroup UI
 * Dialog page to set the cell background.
 */
class LayoutPagePattern : public QWidget
{
    Q_OBJECT
public:
    LayoutPagePattern(QWidget *parent);

    void apply(Style *style, bool partial);
    void loadFrom(const Style &style, bool partial);

public Q_SLOTS:
    void slotUnselectOthers(BrushSelect *_select);
    void slotSetColorButton(const QColor &_color);
    void slotSetBackgroundColor(const QColor &_color);
    void slotNotAnyColor();

protected:
    Qt::BrushStyle getBrushStyle(int idx);

    BrushSelect *selectedBrush;
    BrushSelect *brush[BRUSH_COUNT];
    BrushSelect *preview;
    KColorButton *color;
    QPushButton *notAnyColor;
    KColorButton *bgColorButton;

    QColor currentColor;
    QColor bgColor;

    bool m_notAnyColor;
    QColor m_bgColor; // original color
    QBrush m_brush; // original brush
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_LAYOUT_PAGE_PATTERN
