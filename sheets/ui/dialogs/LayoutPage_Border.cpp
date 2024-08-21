/* This file is part of the KDE project
             SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
             SPDX-FileCopyrightText: 2004-2022 Tomas Mecir <mecirt@gmail.com>
             SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
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

// TODO - this needs to be done better, it sort of works, but is barely usable, if at all. Multicell borders are also mostly wrong.

#include "LayoutPage_Border.h"
#include "LayoutWidgets.h"
#include "core/Style.h"

#include <KoIcon.h>

#include <KColorButton>
#include <KComboBox>
#include <KLocalizedString>

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>

using namespace Calligra::Sheets;

BorderButton::BorderButton(QWidget *parent, const char * /*_name*/)
    : QPushButton(parent)
{
    penStyle = Qt::NoPen;
    penWidth = 1;
    penColor = palette().text().color();
    setCheckable(true);
    setChecked(false);
    setChanged(false);
}
void BorderButton::mousePressEvent(QMouseEvent *)
{
    this->setChecked(!isChecked());
    Q_EMIT clicked(this);
}

void BorderButton::setUndefined()
{
    setPenStyle(Qt::SolidLine);
    setPenWidth(1);
    setColor(palette().midlight().color());
}

void BorderButton::unselect()
{
    setChecked(false);
    setPenWidth(1);
    setPenStyle(Qt::NoPen);
    setColor(palette().text().color());
    setChanged(true);
}

Border::Border(QWidget *parent, const char * /*_name*/, bool _oneCol, bool _oneRow)
    : QFrame(parent)
{
    setAutoFillBackground(true);

    oneCol = _oneCol;
    oneRow = _oneRow;
}

#define OFFSETX 5
#define OFFSETY 5
void Border::paintEvent(QPaintEvent *_ev)
{
    QFrame::paintEvent(_ev);
    QPen pen;
    QPainter painter;
    painter.begin(this);
    pen = QPen(palette().midlight(), 2, Qt::SolidLine).color();
    painter.setPen(pen);

    painter.drawLine(OFFSETX - 5, OFFSETY, OFFSETX, OFFSETY);
    painter.drawLine(OFFSETX, OFFSETY - 5, OFFSETX, OFFSETY);
    painter.drawLine(width() - OFFSETX, OFFSETY, width(), OFFSETY);
    painter.drawLine(width() - OFFSETX, OFFSETY - 5, width() - OFFSETX, OFFSETY);

    painter.drawLine(OFFSETX, height() - OFFSETY, OFFSETX, height());
    painter.drawLine(OFFSETX - 5, height() - OFFSETY, OFFSETX, height() - OFFSETY);

    painter.drawLine(width() - OFFSETX, height() - OFFSETY, width(), height() - OFFSETY);
    painter.drawLine(width() - OFFSETX, height() - OFFSETY, width() - OFFSETX, height());
    if (oneCol == false) {
        painter.drawLine(width() / 2, OFFSETY - 5, width() / 2, OFFSETY);
        painter.drawLine(width() / 2 - 5, OFFSETY, width() / 2 + 5, OFFSETY);
        painter.drawLine(width() / 2, height() - OFFSETY, width() / 2, height());
        painter.drawLine(width() / 2 - 5, height() - OFFSETY, width() / 2 + 5, height() - OFFSETY);
    }
    if (oneRow == false) {
        painter.drawLine(OFFSETX - 5, height() / 2, OFFSETX, height() / 2);
        painter.drawLine(OFFSETX, height() / 2 - 5, OFFSETX, height() / 2 + 5);
        painter.drawLine(width() - OFFSETX, height() / 2, width(), height() / 2);
        painter.drawLine(width() - OFFSETX, height() / 2 - 5, width() - OFFSETX, height() / 2 + 5);
    }
    painter.end();
    Q_EMIT redraw();
}

void Border::mousePressEvent(QMouseEvent *_ev)
{
    Q_EMIT choosearea(_ev);
}

PatternSelect::PatternSelect(QWidget *parent, const char *)
    : QFrame(parent)
{
    penStyle = Qt::NoPen;
    penWidth = 1;
    penColor = palette().text().color();
    selected = false;
    undefined = false;
}

void PatternSelect::setPattern(const QColor &_color, int _width, Qt::PenStyle _style)
{
    penStyle = _style;
    penColor = _color;
    penWidth = _width;
    repaint();
}

void PatternSelect::setUndefined()
{
    undefined = true;
}

void PatternSelect::paintEvent(QPaintEvent *_ev)
{
    QFrame::paintEvent(_ev);

    QPainter painter(this);

    if (!undefined) {
        QPen pen(penColor, penWidth, penStyle);
        painter.setPen(pen);
        painter.drawLine(6, height() / 2, width() - 6, height() / 2);
    } else {
        painter.fillRect(2, 2, width() - 4, height() - 4, Qt::BDiagPattern);
    }
}

void PatternSelect::mousePressEvent(QMouseEvent *)
{
    slotSelect();

    Q_EMIT clicked(this);
}

void PatternSelect::slotUnselect()
{
    selected = false;

    setLineWidth(1);
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    repaint();
}

void PatternSelect::slotSelect()
{
    selected = true;

    setLineWidth(2);
    setFrameStyle(QFrame::Panel | QFrame::Plain);
    repaint();
}

LayoutPageBorder::LayoutPageBorder(QWidget *parent)
    : QWidget(parent)
    , m_multicell(false)
{
    InitializeGrids();
    InitializePatterns();
    SetConnections();

    preview->slotSelect();
    pattern[2]->slotSelect();

    style->setEnabled(false);
    size->setEnabled(false);
    preview->setPattern(Qt::black, 1, Qt::SolidLine);
    this->resize(400, 400);
}

void LayoutPageBorder::InitializeGrids()
{
    QGridLayout *grid = new QGridLayout(this);
    QGridLayout *grid2 = nullptr;
    QGroupBox *tmpQGroupBox = nullptr;

    /***********************/
    /* here is the data to initialize all the border buttons with */
    const char borderButtonNames[BorderType_END][20] = {"top", "bottom", "left", "right", "vertical", "fall", "go", "horizontal"};

    const char shortcutButtonNames[BorderShortcutType_END][20] = {"remove", "all", "outline"};

    QString borderButtonIconNames[BorderType_END] = {koIconName("format-border-set-top"),
                                                     koIconName("format-border-set-bottom"),
                                                     koIconName("format-border-set-left"),
                                                     koIconName("format-border-set-right"),
                                                     koIconName("format-border-set-internal-vertical"),
                                                     koIconName("format-border-set-internal-horizontal"),
                                                     koIconName("format-border-set-diagonal-tl-br"),
                                                     koIconName("format-border-set-diagonal-bl-tr")};

    QString shortcutButtonIconNames[BorderShortcutType_END] = {koIconName("format-border-set-none"), QString(), koIconName("format-border-set-external")};

    int borderButtonPositions[BorderType_END][2] = {{0, 2}, {4, 2}, {2, 0}, {2, 4}, {4, 4}, {4, 0}, {0, 0}, {0, 4}};

    int shortcutButtonPositions[BorderShortcutType_END][2] = {{0, 0}, {0, 1}, {0, 2}};
    /***********************/

    /* set up a layout box for most of the border setting buttons */
    tmpQGroupBox = new QGroupBox(this);
    tmpQGroupBox->setTitle(i18n("Border"));
    tmpQGroupBox->setAlignment(Qt::AlignLeft);
    grid2 = new QGridLayout(tmpQGroupBox);
    int fHeight = tmpQGroupBox->fontMetrics().height();
    grid2->addItem(new QSpacerItem(0, fHeight / 2), 0, 0); // groupbox title

    area = new Border(tmpQGroupBox, "area", !m_multicell, !m_multicell);
    grid2->addWidget(area, 2, 1, 3, 3);
    QPalette palette = area->palette();
    palette.setColor(area->backgroundRole(), this->palette().base().color());
    area->setPalette(palette);

    /* initialize the buttons that are in this box */
    for (int i = BorderType_Top; i < BorderType_END; i++) {
        borderButtons[i] = new BorderButton(tmpQGroupBox, borderButtonNames[i]);
        loadIcon(borderButtonIconNames[i], borderButtons[i]);
        grid2->addWidget(borderButtons[i], borderButtonPositions[i][0] + 1, borderButtonPositions[i][1]);
    }

    grid->addWidget(tmpQGroupBox, 0, 0, 3, 1);

    /* the remove, all, and outline border buttons are in a second box down
       below.*/

    tmpQGroupBox = new QGroupBox(this);
    tmpQGroupBox->setTitle(i18n("Preselect"));
    tmpQGroupBox->setAlignment(Qt::AlignLeft);

    grid2 = new QGridLayout(tmpQGroupBox);

    shortcutButtonIconNames[BorderShortcutType_All] = koIconName("format-border-set-internal");

    for (int i = BorderShortcutType_Remove; i < BorderShortcutType_END; i++) {
        shortcutButtons[i] = new BorderButton(tmpQGroupBox, shortcutButtonNames[i]);
        loadIcon(shortcutButtonIconNames[i], shortcutButtons[i]);
        grid2->addWidget(shortcutButtons[i], shortcutButtonPositions[i][0], shortcutButtonPositions[i][1]);
    }

    if (!m_multicell)
        shortcutButtons[BorderShortcutType_All]->setEnabled(false);

    grid->addWidget(tmpQGroupBox, 3, 0, 2, 1);

    /* now set up the group box with the pattern selector */
    tmpQGroupBox = new QGroupBox(this);
    tmpQGroupBox->setTitle(i18n("Pattern"));
    tmpQGroupBox->setAlignment(Qt::AlignLeft);

    grid2 = new QGridLayout(tmpQGroupBox);
    fHeight = tmpQGroupBox->fontMetrics().height();
    grid2->addItem(new QSpacerItem(0, fHeight / 2), 0, 0); // groupbox title

    char name[] = "PatternXX";
    Q_ASSERT(NUM_BORDER_PATTERNS < 100);

    for (int i = 0; i < NUM_BORDER_PATTERNS; i++) {
        name[7] = '0' + (i + 1) / 10;
        name[8] = '0' + (i + 1) % 10;
        pattern[i] = new PatternSelect(tmpQGroupBox, name);
        pattern[i]->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        grid2->addWidget(pattern[i], i % 5, i / 5);
        /* this puts them in the pattern:
           1  6
           2  7
           3  8
           4  9
           5  10
        */
    }

    color = new KColorButton(tmpQGroupBox);
    grid2->addWidget(color, 8, 1);

    QLabel *tmpQLabel = new QLabel(tmpQGroupBox);
    tmpQLabel->setText(i18n("Color:"));
    grid2->addWidget(tmpQLabel, 8, 0);

    /* tack on the 'customize' border pattern selector */
    customize = new QCheckBox(i18n("Customize"), tmpQGroupBox);
    grid2->addWidget(customize, 6, 0);
    connect(customize, &QAbstractButton::clicked, this, &LayoutPageBorder::cutomize_chosen_slot);

    size = new KComboBox(tmpQGroupBox);
    size->setEditable(true);
    grid2->addWidget(size, 7, 1);
    size->setValidator(new QIntValidator(size));
    QString tmp;
    for (int i = 0; i < 10; i++) {
        tmp = tmp.setNum(i);
        size->insertItem(i, tmp);
    }
    size->setCurrentIndex(1);

    style = new KComboBox(tmpQGroupBox);
    grid2->addWidget(style, 7, 0);
    style->setIconSize(QSize(100, 14));
    style->insertItem(0, paintFormatPixmap(Qt::DotLine), "");
    style->insertItem(1, paintFormatPixmap(Qt::DashLine), "");
    style->insertItem(2, paintFormatPixmap(Qt::DashDotLine), "");
    style->insertItem(3, paintFormatPixmap(Qt::DashDotDotLine), "");
    style->insertItem(4, paintFormatPixmap(Qt::SolidLine), "");
    palette = style->palette();
    palette.setColor(style->backgroundRole(), this->palette().window().color());
    style->setPalette(palette);

    grid->addWidget(tmpQGroupBox, 0, 1, 4, 1);

    /* Now the preview box is put together */
    tmpQGroupBox = new QGroupBox(this);
    tmpQGroupBox->setTitle(i18n("Preview"));
    tmpQGroupBox->setAlignment(Qt::AlignLeft);

    grid2 = new QGridLayout(tmpQGroupBox);
    fHeight = tmpQGroupBox->fontMetrics().height();
    grid2->addItem(new QSpacerItem(0, fHeight / 2), 0, 0); // groupbox title

    preview = new PatternSelect(tmpQGroupBox, "Pattern_preview");
    preview->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    grid2->addWidget(preview, 1, 0);

    grid->addWidget(tmpQGroupBox, 4, 1);
}

void LayoutPageBorder::InitializeBorderButtons()
{
    for (int i = BorderType_Top; i < BorderType_END; i++) {
        // no border?
        if ((!borders[i].bStyle) || (borders[i].style == Qt::NoPen)) {
            borderButtons[i]->setUndefined();
            borderButtons[i]->setChecked(false);
            continue;
        }

        borderButtons[i]->setPenStyle(borders[i].style);
        borderButtons[i]->setPenWidth(borders[i].width);
        borderButtons[i]->setColor(borders[i].color);
        borderButtons[i]->setChecked(true);
    }
}

void LayoutPageBorder::InitializePatterns()
{
    pattern[0]->setPattern(Qt::black, 1, Qt::DotLine);
    pattern[1]->setPattern(Qt::black, 1, Qt::DashLine);
    pattern[2]->setPattern(Qt::black, 1, Qt::SolidLine);
    pattern[3]->setPattern(Qt::black, 1, Qt::DashDotLine);
    pattern[4]->setPattern(Qt::black, 1, Qt::DashDotDotLine);
    pattern[5]->setPattern(Qt::black, 2, Qt::SolidLine);
    pattern[6]->setPattern(Qt::black, 3, Qt::SolidLine);
    pattern[7]->setPattern(Qt::black, 4, Qt::SolidLine);
    pattern[8]->setPattern(Qt::black, 5, Qt::SolidLine);
    pattern[9]->setPattern(Qt::black, 1, Qt::NoPen);

    slotSetColorButton(Qt::black);
}

void LayoutPageBorder::SetConnections()
{
    connect(color, &KColorButton::changed, this, &LayoutPageBorder::slotSetColorButton);

    for (int i = 0; i < NUM_BORDER_PATTERNS; i++) {
        connect(pattern[i], &PatternSelect::clicked, this, &LayoutPageBorder::slotUnselect2);
    }

    for (int i = BorderType_Top; i < BorderType_END; i++) {
        connect(borderButtons[i], QOverload<BorderButton *>::of(&BorderButton::clicked), this, &LayoutPageBorder::changeState);
    }

    for (int i = BorderShortcutType_Remove; i < BorderShortcutType_END; i++) {
        connect(shortcutButtons[i], QOverload<BorderButton *>::of(&BorderButton::clicked), this, &LayoutPageBorder::preselect);
    }

    connect(area, &Border::redraw, this, &LayoutPageBorder::draw);
    connect(area, &Border::choosearea, this, &LayoutPageBorder::slotPressEvent);

    connect(style, QOverload<int>::of(&KComboBox::activated), this, QOverload<int>::of(&LayoutPageBorder::slotChangeStyle));
    connect(size, &KComboBox::editTextChanged, this, QOverload<const QString &>::of(&LayoutPageBorder::slotChangeStyle));
    connect(size, QOverload<int>::of(&KComboBox::activated), this, QOverload<int>::of(&LayoutPageBorder::slotChangeStyle));
}

void LayoutPageBorder::cutomize_chosen_slot()
{
    if (customize->isChecked()) {
        style->setEnabled(true);
        size->setEnabled(true);
        slotUnselect2(preview);
    } else {
        style->setEnabled(false);
        size->setEnabled(false);
        pattern[2]->slotSelect();
        preview->setPattern(Qt::black, 1, Qt::SolidLine);
    }
}

void LayoutPageBorder::slotChangeStyle(const QString &)
{
    /* if they try putting text in the size box, then erase the line */
    slotChangeStyle(0);
}

void LayoutPageBorder::slotChangeStyle(int)
{
    int index = style->currentIndex();
    int penSize = size->currentText().toInt();
    if (!penSize) {
        preview->setPattern(preview->getColor(), penSize, Qt::NoPen);
    } else {
        switch (index) {
        case 0:
            preview->setPattern(preview->getColor(), penSize, Qt::DotLine);
            break;
        case 1:
            preview->setPattern(preview->getColor(), penSize, Qt::DashLine);
            break;
        case 2:
            preview->setPattern(preview->getColor(), penSize, Qt::DashDotLine);
            break;
        case 3:
            preview->setPattern(preview->getColor(), penSize, Qt::DashDotDotLine);
            break;
        case 4:
            preview->setPattern(preview->getColor(), penSize, Qt::SolidLine);
            break;
        default:
            debugSheets << "Error in combobox";
            break;
        }
    }
    slotUnselect2(preview);
}

QPixmap LayoutPageBorder::paintFormatPixmap(Qt::PenStyle _style)
{
    QPixmap pixmap(100, 14);
    pixmap.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&pixmap);
    painter.setPen(QPen(palette().text().color(), 5, _style));
    painter.drawLine(0, 7, 100, 7);
    painter.end();
    return pixmap;
}

void LayoutPageBorder::loadIcon(const QString &iconName, BorderButton *_button)
{
    _button->setIcon(QIcon::fromTheme(iconName));
}

void LayoutPageBorder::loadFrom(const Style &style, bool partial)
{
    m_multicell = partial;

    // TODO - support the partial styles!!!

    // We assume, that all other objects have the same values
    for (int i = 0; i < BorderType_END; ++i) {
        borders[i].bStyle = true;
        borders[i].bColor = true;
    }

    QPen pen = style.leftBorderPen();
    borders[BorderType_Left].style = pen.style();
    borders[BorderType_Left].width = pen.width();
    borders[BorderType_Left].color = pen.color();
    if (!style.hasAttribute(Style::LeftPen))
        borders[BorderType_Left].bStyle = false;
    borders[BorderType_Vertical].style = pen.style();
    borders[BorderType_Vertical].width = pen.width();
    borders[BorderType_Vertical].color = pen.color();
    if (!style.hasAttribute(Style::LeftPen))
        borders[BorderType_Vertical].bStyle = false;

    pen = style.topBorderPen();
    borders[BorderType_Top].style = pen.style();
    borders[BorderType_Top].width = pen.width();
    borders[BorderType_Top].color = pen.color();
    if (!style.hasAttribute(Style::TopPen))
        borders[BorderType_Top].bStyle = false;
    borders[BorderType_Horizontal].style = pen.style();
    borders[BorderType_Horizontal].width = pen.width();
    borders[BorderType_Horizontal].color = pen.color();
    if (!style.hasAttribute(Style::TopPen))
        borders[BorderType_Horizontal].bStyle = false;

    pen = style.rightBorderPen();
    borders[BorderType_Right].style = pen.style();
    borders[BorderType_Right].width = pen.width();
    borders[BorderType_Right].color = pen.color();
    if (!style.hasAttribute(Style::RightPen))
        borders[BorderType_Right].bStyle = false;

    pen = style.bottomBorderPen();
    borders[BorderType_Bottom].style = pen.style();
    borders[BorderType_Bottom].width = pen.width();
    borders[BorderType_Bottom].color = pen.color();
    if (!style.hasAttribute(Style::BottomPen))
        borders[BorderType_Bottom].bStyle = false;

    pen = style.fallDiagonalPen();
    borders[BorderType_FallingDiagonal].style = pen.style();
    borders[BorderType_FallingDiagonal].width = pen.width();
    borders[BorderType_FallingDiagonal].color = pen.color();
    if (!style.hasAttribute(Style::FallDiagonalPen))
        borders[BorderType_FallingDiagonal].bStyle = false;

    pen = style.goUpDiagonalPen();
    borders[BorderType_RisingDiagonal].style = pen.style();
    borders[BorderType_RisingDiagonal].width = pen.width();
    borders[BorderType_RisingDiagonal].color = pen.color();
    if (!style.hasAttribute(Style::GoUpDiagonalPen))
        borders[BorderType_RisingDiagonal].bStyle = false;

    InitializeBorderButtons();
}

void LayoutPageBorder::apply(Style *style, bool partial)
{
    BorderButton *btn;
    QPen curPen;

    // TODO - Horizontal/Vertical stuff

    btn = borderButtons[BorderType_Left];
    curPen = QPen(btn->getColor(), btn->getPenWidth(), btn->getPenStyle());
    if (shouldApplyLayoutChange(!btn->isChecked(), btn->isChanged(), partial))
        style->setLeftBorderPen(curPen);

    btn = borderButtons[BorderType_Right];
    curPen = QPen(btn->getColor(), btn->getPenWidth(), btn->getPenStyle());
    if (shouldApplyLayoutChange(!btn->isChecked(), btn->isChanged(), partial))
        style->setRightBorderPen(curPen);

    btn = borderButtons[BorderType_Top];
    curPen = QPen(btn->getColor(), btn->getPenWidth(), btn->getPenStyle());
    if (shouldApplyLayoutChange(!btn->isChecked(), btn->isChanged(), partial))
        style->setTopBorderPen(curPen);

    btn = borderButtons[BorderType_Bottom];
    curPen = QPen(btn->getColor(), btn->getPenWidth(), btn->getPenStyle());
    if (shouldApplyLayoutChange(!btn->isChecked(), btn->isChanged(), partial))
        style->setBottomBorderPen(curPen);

    btn = borderButtons[BorderType_FallingDiagonal];
    curPen = QPen(btn->getColor(), btn->getPenWidth(), btn->getPenStyle());
    if (shouldApplyLayoutChange(!btn->isChecked(), btn->isChanged(), partial))
        style->setFallDiagonalPen(curPen);

    btn = borderButtons[BorderType_RisingDiagonal];
    curPen = QPen(btn->getColor(), btn->getPenWidth(), btn->getPenStyle());
    if (shouldApplyLayoutChange(!btn->isChecked(), btn->isChanged(), partial))
        style->setGoUpDiagonalPen(curPen);
}

void LayoutPageBorder::slotSetColorButton(const QColor &_color)
{
    currentColor = _color;

    for (int i = 0; i < NUM_BORDER_PATTERNS; ++i) {
        pattern[i]->setColor(currentColor);
    }
    preview->setColor(currentColor);
}

void LayoutPageBorder::slotUnselect2(PatternSelect *_p)
{
    for (int i = 0; i < NUM_BORDER_PATTERNS; ++i) {
        if (pattern[i] != _p) {
            pattern[i]->slotUnselect();
        }
    }
    preview->setPattern(_p->getColor(), _p->getPenWidth(), _p->getPenStyle());
}

void LayoutPageBorder::preselect(BorderButton *_p)
{
    BorderButton *top = borderButtons[BorderType_Top];
    BorderButton *bottom = borderButtons[BorderType_Bottom];
    BorderButton *left = borderButtons[BorderType_Left];
    BorderButton *right = borderButtons[BorderType_Right];
    BorderButton *vertical = borderButtons[BorderType_Vertical];
    BorderButton *horizontal = borderButtons[BorderType_Horizontal];
    BorderButton *remove = shortcutButtons[BorderShortcutType_Remove];
    BorderButton *outline = shortcutButtons[BorderShortcutType_Outline];
    BorderButton *all = shortcutButtons[BorderShortcutType_All];

    _p->setChecked(false);
    if (_p == remove) {
        for (int i = BorderType_Top; i < BorderType_END; i++) {
            if (borderButtons[i]->isChecked()) {
                borderButtons[i]->unselect();
            }
        }
    }
    if (_p == outline) {
        top->setChecked(true);
        top->setPenWidth(preview->getPenWidth());
        top->setPenStyle(preview->getPenStyle());
        top->setColor(currentColor);
        top->setChanged(true);
        bottom->setChecked(true);
        bottom->setPenWidth(preview->getPenWidth());
        bottom->setPenStyle(preview->getPenStyle());
        bottom->setColor(currentColor);
        bottom->setChanged(true);
        left->setChecked(true);
        left->setPenWidth(preview->getPenWidth());
        left->setPenStyle(preview->getPenStyle());
        left->setColor(currentColor);
        left->setChanged(true);
        right->setChecked(true);
        right->setPenWidth(preview->getPenWidth());
        right->setPenStyle(preview->getPenStyle());
        right->setColor(currentColor);
        right->setChanged(true);
    }
    if (_p == all) {
        if (m_multicell) {
            horizontal->setChecked(true);
            horizontal->setPenWidth(preview->getPenWidth());
            horizontal->setPenStyle(preview->getPenStyle());
            horizontal->setColor(currentColor);
            horizontal->setChanged(true);

            vertical->setChecked(true);
            vertical->setPenWidth(preview->getPenWidth());
            vertical->setPenStyle(preview->getPenStyle());
            vertical->setColor(currentColor);
            vertical->setChanged(true);
        }
    }
    area->repaint();
}

void LayoutPageBorder::changeState(BorderButton *_p)
{
    _p->setChanged(true);

    if (_p->isChecked()) {
        _p->setPenWidth(preview->getPenWidth());
        _p->setPenStyle(preview->getPenStyle());
        _p->setColor(currentColor);
    } else {
        _p->setPenWidth(1);
        _p->setPenStyle(Qt::NoPen);
        _p->setColor(palette().text().color());
    }

    area->repaint();
}

void LayoutPageBorder::draw()
{
    BorderButton *top = borderButtons[BorderType_Top];
    BorderButton *bottom = borderButtons[BorderType_Bottom];
    BorderButton *left = borderButtons[BorderType_Left];
    BorderButton *right = borderButtons[BorderType_Right];
    BorderButton *risingDiagonal = borderButtons[BorderType_RisingDiagonal];
    BorderButton *fallingDiagonal = borderButtons[BorderType_FallingDiagonal];
    BorderButton *vertical = borderButtons[BorderType_Vertical];
    BorderButton *horizontal = borderButtons[BorderType_Horizontal];
    QPen pen;
    QPainter painter;
    painter.begin(area);

    if ((bottom->getPenStyle()) != Qt::NoPen) {
        pen = QPen(bottom->getColor(), bottom->getPenWidth(), bottom->getPenStyle());
        painter.setPen(pen);
        painter.drawLine(OFFSETX, area->height() - OFFSETY, area->width() - OFFSETX, area->height() - OFFSETY);
    }
    if ((top->getPenStyle()) != Qt::NoPen) {
        pen = QPen(top->getColor(), top->getPenWidth(), top->getPenStyle());
        painter.setPen(pen);
        painter.drawLine(OFFSETX, OFFSETY, area->width() - OFFSETX, OFFSETY);
    }
    if ((left->getPenStyle()) != Qt::NoPen) {
        pen = QPen(left->getColor(), left->getPenWidth(), left->getPenStyle());
        painter.setPen(pen);
        painter.drawLine(OFFSETX, OFFSETY, OFFSETX, area->height() - OFFSETY);
    }
    if ((right->getPenStyle()) != Qt::NoPen) {
        pen = QPen(right->getColor(), right->getPenWidth(), right->getPenStyle());
        painter.setPen(pen);
        painter.drawLine(area->width() - OFFSETX, OFFSETY, area->width() - OFFSETX, area->height() - OFFSETY);
    }
    if ((fallingDiagonal->getPenStyle()) != Qt::NoPen) {
        pen = QPen(fallingDiagonal->getColor(), fallingDiagonal->getPenWidth(), fallingDiagonal->getPenStyle());
        painter.setPen(pen);
        painter.drawLine(OFFSETX, OFFSETY, area->width() - OFFSETX, area->height() - OFFSETY);
        if (m_multicell) {
            painter.drawLine(area->width() / 2, OFFSETY, area->width() - OFFSETX, area->height() / 2);
            painter.drawLine(OFFSETX, area->height() / 2, area->width() / 2, area->height() - OFFSETY);
        }
    }
    if ((risingDiagonal->getPenStyle()) != Qt::NoPen) {
        pen = QPen(risingDiagonal->getColor(), risingDiagonal->getPenWidth(), risingDiagonal->getPenStyle());
        painter.setPen(pen);
        painter.drawLine(OFFSETX, area->height() - OFFSETY, area->width() - OFFSETX, OFFSETY);
        if (m_multicell) {
            painter.drawLine(area->width() / 2, OFFSETY, OFFSETX, area->height() / 2);
            painter.drawLine(area->width() / 2, area->height() - OFFSETY, area->width() - OFFSETX, area->height() / 2);
        }
    }
    if ((vertical->getPenStyle()) != Qt::NoPen) {
        pen = QPen(vertical->getColor(), vertical->getPenWidth(), vertical->getPenStyle());
        painter.setPen(pen);
        painter.drawLine(area->width() / 2, 5, area->width() / 2, area->height() - 5);
    }
    if ((horizontal->getPenStyle()) != Qt::NoPen) {
        pen = QPen(horizontal->getColor(), horizontal->getPenWidth(), horizontal->getPenStyle());
        painter.setPen(pen);
        painter.drawLine(OFFSETX, area->height() / 2, area->width() - OFFSETX, area->height() / 2);
    }
    painter.end();
}

void LayoutPageBorder::invertState(BorderButton *_p)
{
    if (_p->isChecked()) {
        _p->unselect();
    } else {
        _p->setChecked(true);
        _p->setPenWidth(preview->getPenWidth());
        _p->setPenStyle(preview->getPenStyle());
        _p->setColor(currentColor);
        _p->setChanged(true);
    }
}

void LayoutPageBorder::slotPressEvent(QMouseEvent *_ev)
{
    BorderButton *top = borderButtons[BorderType_Top];
    BorderButton *bottom = borderButtons[BorderType_Bottom];
    BorderButton *left = borderButtons[BorderType_Left];
    BorderButton *right = borderButtons[BorderType_Right];
    BorderButton *vertical = borderButtons[BorderType_Vertical];
    BorderButton *horizontal = borderButtons[BorderType_Horizontal];

    QRect rect(OFFSETX, OFFSETY - 8, area->width() - OFFSETX, OFFSETY + 8);
    if (rect.contains(QPoint(_ev->x(), _ev->y()))) {
        if (((top->getPenWidth() != preview->getPenWidth()) || (top->getColor() != currentColor) || (top->getPenStyle() != preview->getPenStyle()))
            && top->isChecked()) {
            top->setPenWidth(preview->getPenWidth());
            top->setPenStyle(preview->getPenStyle());
            top->setColor(currentColor);
            top->setChanged(true);
        } else
            invertState(top);
    }
    rect.setCoords(OFFSETX, area->height() - OFFSETY - 8, area->width() - OFFSETX, area->height() - OFFSETY + 8);
    if (rect.contains(QPoint(_ev->x(), _ev->y()))) {
        if (((bottom->getPenWidth() != preview->getPenWidth()) || (bottom->getColor() != currentColor) || (bottom->getPenStyle() != preview->getPenStyle()))
            && bottom->isChecked()) {
            bottom->setPenWidth(preview->getPenWidth());
            bottom->setPenStyle(preview->getPenStyle());
            bottom->setColor(currentColor);
            bottom->setChanged(true);
        } else
            invertState(bottom);
    }

    rect.setCoords(OFFSETX - 8, OFFSETY, OFFSETX + 8, area->height() - OFFSETY);
    if (rect.contains(QPoint(_ev->x(), _ev->y()))) {
        if (((left->getPenWidth() != preview->getPenWidth()) || (left->getColor() != currentColor) || (left->getPenStyle() != preview->getPenStyle()))
            && left->isChecked()) {
            left->setPenWidth(preview->getPenWidth());
            left->setPenStyle(preview->getPenStyle());
            left->setColor(currentColor);
            left->setChanged(true);
        } else
            invertState(left);
    }
    rect.setCoords(area->width() - OFFSETX - 8, OFFSETY, area->width() - OFFSETX + 8, area->height() - OFFSETY);
    if (rect.contains(QPoint(_ev->x(), _ev->y()))) {
        if (((right->getPenWidth() != preview->getPenWidth()) || (right->getColor() != currentColor) || (right->getPenStyle() != preview->getPenStyle()))
            && right->isChecked()) {
            right->setPenWidth(preview->getPenWidth());
            right->setPenStyle(preview->getPenStyle());
            right->setColor(currentColor);
            right->setChanged(true);
        } else
            invertState(right);
    }

    // don't work because I don't know how create a rectangle
    // for diagonal
    /*rect.setCoords(OFFSETX,OFFSETY,XLEN-OFFSETX,YHEI-OFFSETY);
    if (rect.contains(QPoint(_ev->x(),_ev->y())))
            {
             invertState(fallDiagonal);
            }
    rect.setCoords(OFFSETX,YHEI-OFFSETY,XLEN-OFFSETX,OFFSETY);
    if (rect.contains(QPoint(_ev->x(),_ev->y())))
            {
             invertState(goUpDiagonal);
            } */

    if (m_multicell) {
        rect.setCoords(area->width() / 2 - 8, OFFSETY, area->width() / 2 + 8, area->height() - OFFSETY);

        if (rect.contains(QPoint(_ev->x(), _ev->y()))) {
            if (((vertical->getPenWidth() != preview->getPenWidth()) || (vertical->getColor() != currentColor)
                 || (vertical->getPenStyle() != preview->getPenStyle()))
                && vertical->isChecked()) {
                vertical->setPenWidth(preview->getPenWidth());
                vertical->setPenStyle(preview->getPenStyle());
                vertical->setColor(currentColor);
                vertical->setChanged(true);
            } else
                invertState(vertical);
        }

        rect.setCoords(OFFSETX, area->height() / 2 - 8, area->width() - OFFSETX, area->height() / 2 + 8);
        if (rect.contains(QPoint(_ev->x(), _ev->y()))) {
            if (((horizontal->getPenWidth() != preview->getPenWidth()) || (horizontal->getColor() != currentColor)
                 || (horizontal->getPenStyle() != preview->getPenStyle()))
                && horizontal->isChecked()) {
                horizontal->setPenWidth(preview->getPenWidth());
                horizontal->setPenStyle(preview->getPenStyle());
                horizontal->setColor(currentColor);
                horizontal->setChanged(true);
            } else
                invertState(horizontal);
        }
    }

    area->repaint();
}
