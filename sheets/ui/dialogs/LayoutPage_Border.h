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

#ifndef CALLIGRA_SHEETS_LAYOUT_PAGE_BORDER
#define CALLIGRA_SHEETS_LAYOUT_PAGE_BORDER

#include <QFrame>
#include <QPushButton>

class KComboBox;
class QCheckBox;
class KColorButton;

namespace Calligra
{
namespace Sheets
{

class Style;

class Border : public QFrame
{
    Q_OBJECT
public:
    Border(QWidget *parent, const char *_name, bool _oneCol, bool _oneRow);
Q_SIGNALS:
    void redraw();
    void choosearea(QMouseEvent *_ev);

protected:
    void paintEvent(QPaintEvent *_ev) override;
    void mousePressEvent(QMouseEvent *_ev) override;
    bool oneCol;
    bool oneRow;
};

class BorderButton : public QPushButton
{
    Q_OBJECT
public:
    BorderButton(QWidget *parent, const char *_name);
    void setPenStyle(Qt::PenStyle _pat)
    {
        penStyle = _pat;
    }
    Qt::PenStyle getPenStyle()
    {
        return penStyle;
    }
    void setColor(const QColor &_col)
    {
        penColor = _col;
    }
    const QColor &getColor()
    {
        return penColor;
    }
    void setPenWidth(int _w)
    {
        penWidth = _w;
    }
    int getPenWidth()
    {
        return penWidth;
    }
    bool isChanged()
    {
        return changed;
    }
    void setChanged(bool _changed)
    {
        changed = _changed;
    }
    void setUndefined();
    void unselect();
Q_SIGNALS:
    void clicked(BorderButton *);

protected:
    void mousePressEvent(QMouseEvent *_ev) override;
    Qt::PenStyle penStyle;
    QColor penColor;
    int penWidth;
    bool changed;
};

/**
 * \ingroup UI
 * Widget to select a background pattern.
 */
class PatternSelect : public QFrame
{
    Q_OBJECT
public:
    PatternSelect(QWidget *parent, const char *_name);

    void setPenStyle(Qt::PenStyle _pat)
    {
        penStyle = _pat;
        repaint();
    }
    Qt::PenStyle getPenStyle()
    {
        return penStyle;
    }
    void setColor(const QColor &_col)
    {
        penColor = _col;
        repaint();
    }
    const QColor &getColor()
    {
        return penColor;
    }
    void setPenWidth(int _w)
    {
        penWidth = _w;
        repaint();
    }
    int getPenWidth()
    {
        return penWidth;
    }

    void setPattern(const QColor &_color, int _width, Qt::PenStyle _style);
    void setUndefined();
    void setDefined()
    {
        undefined = false;
        repaint();
    }

    bool isDefined()
    {
        return !undefined;
    }

Q_SIGNALS:
    void clicked(PatternSelect *_this);

public Q_SLOTS:
    void slotUnselect();
    void slotSelect();

protected:
    void paintEvent(QPaintEvent *_ev) override;
    void mousePressEvent(QMouseEvent *_ev) override;

    Qt::PenStyle penStyle;
    QColor penColor;
    int penWidth;

    bool selected;
    bool undefined;
};

enum BorderType {
    BorderType_Top = 0,
    BorderType_Bottom,
    BorderType_Left,
    BorderType_Right,
    BorderType_Vertical,
    BorderType_Horizontal,
    BorderType_FallingDiagonal,
    BorderType_RisingDiagonal,
    BorderType_END
};

enum BorderShortcutType { BorderShortcutType_Remove = 0, BorderShortcutType_All, BorderShortcutType_Outline, BorderShortcutType_END };

/**
 * \ingroup UI
 * Dialog page to select the cell borders.
 */
class LayoutPageBorder : public QWidget
{
    Q_OBJECT
public:
    LayoutPageBorder(QWidget *parent);

    void apply(Style *style, bool partial);
    void loadFrom(const Style &style, bool partial);
    void invertState(BorderButton *_button);
    QPixmap paintFormatPixmap(Qt::PenStyle _style);

public Q_SLOTS:
    void changeState(BorderButton *_this);
    void preselect(BorderButton *_this);
    void draw();
    void slotSetColorButton(const QColor &_color);
    void slotUnselect2(PatternSelect *_select);
    void loadIcon(const QString &iconName, BorderButton *_button);
    void slotPressEvent(QMouseEvent *_ev);
    void slotChangeStyle(int);
    void slotChangeStyle(const QString &);
    void cutomize_chosen_slot();

protected:
    struct CellBorderFormat {
        int width;
        bool bStyle;
        QColor color;
        bool bColor;
        Qt::PenStyle style;
    };

    // The format of the selected area
    CellBorderFormat borders[BorderType_END];

    BorderButton *borderButtons[BorderType_END];
    BorderButton *shortcutButtons[BorderShortcutType_END];
#define NUM_BORDER_PATTERNS 10

    /* the patterns to choose from */
    PatternSelect *pattern[NUM_BORDER_PATTERNS];

    /* the pattern box that is the 'preview' of what is selected above. */
    PatternSelect *preview;
    KComboBox *size;
    KComboBox *style;
    KColorButton *color;
    QCheckBox *customize;
    QColor currentColor;
    Border *area;
    bool m_multicell;

private:
    /*some helper functions to space some tasks apart */
    void InitializeGrids();
    void InitializeBorderButtons();
    void InitializePatterns();
    void SetConnections();
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_LAYOUT_PAGE_BORDER
