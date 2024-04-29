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

#include "LayoutPage_Pattern.h"
#include "LayoutWidgets.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPainter>
#include <QPushButton>

#include <KColorButton>
#include <KLocalizedString>

using namespace Calligra::Sheets;

BrushSelect::BrushSelect(QWidget *parent, const char *)
    : QFrame(parent)
{
    brushStyle = Qt::NoBrush;
    brushColor = Qt::red;
    selected = false;
}

void BrushSelect::setPattern(const QColor &_color, Qt::BrushStyle _style)
{
    brushStyle = _style;
    brushColor = _color;
    repaint();
}

void BrushSelect::paintEvent(QPaintEvent *_ev)
{
    QFrame::paintEvent(_ev);

    QPainter painter;
    QBrush brush(brushColor, brushStyle);
    painter.begin(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawRect(2, 2, width() - 4, height() - 4);
    painter.end();
}

void BrushSelect::mousePressEvent(QMouseEvent *)
{
    slotSelect();

    Q_EMIT clicked(this);
}

void BrushSelect::slotUnselect()
{
    selected = false;

    setLineWidth(1);
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    repaint();
}

void BrushSelect::slotSelect()
{
    selected = true;

    setLineWidth(2);
    setFrameStyle(QFrame::Panel | QFrame::Plain);
    repaint();
}

LayoutPagePattern::LayoutPagePattern(QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *grid = new QGridLayout(this);

    QGroupBox *tmpQGroupBox;
    tmpQGroupBox = new QGroupBox(this);
    tmpQGroupBox->setTitle(i18n("Pattern"));
    tmpQGroupBox->setAlignment(Qt::AlignLeft);

    QGridLayout *grid2 = new QGridLayout(tmpQGroupBox);
    int fHeight = tmpQGroupBox->fontMetrics().height();
    grid2->addItem(new QSpacerItem(0, fHeight / 2), 0, 0); // groupbox title

    int brushRows = 0;
    for (int idx = 0; idx < BRUSH_COUNT; ++idx) {
        brush[idx] = new BrushSelect(tmpQGroupBox, (QString("Frame_") + QString::number(idx + 1)).toLatin1().data());
        brush[idx]->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        brushRows = int(idx / 3) + 1;
        grid2->addWidget(brush[idx], brushRows, idx % 3);
        brush[idx]->setPattern(Qt::red, getBrushStyle(idx));
        connect(brush[idx], &BrushSelect::clicked, this, &LayoutPagePattern::slotUnselectOthers);
    }

    QGridLayout *grid3 = new QGridLayout();
    color = new KColorButton(tmpQGroupBox);
    grid3->addWidget(color, 0, 1);

    QLabel *tmpQLabel = new QLabel(tmpQGroupBox);
    tmpQLabel->setText(i18n("Color:"));
    grid3->addWidget(tmpQLabel, 0, 0);

    grid2->addItem(grid3, brushRows + 1, 0, 1, 3);

    grid3 = new QGridLayout();

    tmpQLabel = new QLabel(tmpQGroupBox);
    grid3->addWidget(tmpQLabel, 0, 0);
    tmpQLabel->setText(i18n("Background color:"));

    bgColorButton = new KColorButton(tmpQGroupBox);
    grid3->addWidget(bgColorButton, 0, 1);

    connect(bgColorButton, &KColorButton::changed, this, &LayoutPagePattern::slotSetBackgroundColor);

    notAnyColor = new QPushButton(i18n("No Color"), tmpQGroupBox);
    grid3->addWidget(notAnyColor, 0, 2);
    connect(notAnyColor, &QAbstractButton::clicked, this, &LayoutPagePattern::slotNotAnyColor);
    m_notAnyColor = true;

    grid2->addItem(grid3, brushRows + 2, 0, 1, 3);

    grid->addWidget(tmpQGroupBox, 0, 0, 4, 1);

    tmpQGroupBox = new QGroupBox(this);
    tmpQGroupBox->setTitle(i18n("Preview"));
    tmpQGroupBox->setAlignment(Qt::AlignLeft);

    grid2 = new QGridLayout(tmpQGroupBox);
    fHeight = tmpQGroupBox->fontMetrics().height();
    grid2->addItem(new QSpacerItem(0, fHeight / 2), 0, 0); // groupbox title

    preview = new BrushSelect(tmpQGroupBox, "Current");
    preview->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    grid2->addWidget(preview, 1, 0);
    grid->addWidget(tmpQGroupBox, 4, 0);

    selectedBrush = nullptr;

    connect(color, &KColorButton::changed, this, &LayoutPagePattern::slotSetColorButton);
    this->resize(400, 400);
}

Qt::BrushStyle LayoutPagePattern::getBrushStyle(int idx)
{
    switch (idx) {
    case 0:
        return Qt::VerPattern;
    case 1:
        return Qt::HorPattern;
    case 2:
        return Qt::Dense1Pattern;
    case 3:
        return Qt::Dense2Pattern;
    case 4:
        return Qt::Dense3Pattern;
    case 5:
        return Qt::Dense4Pattern;
    case 6:
        return Qt::Dense5Pattern;
    case 7:
        return Qt::Dense6Pattern;
    case 8:
        return Qt::Dense7Pattern;
    case 9:
        return Qt::CrossPattern;
    case 10:
        return Qt::BDiagPattern;
    case 11:
        return Qt::FDiagPattern;
    case 12:
        return Qt::DiagCrossPattern;
    case 13:
        return Qt::SolidPattern;
    case 14:
        return Qt::NoBrush;
    default:
        return Qt::NoBrush;
    }
}

void LayoutPagePattern::slotNotAnyColor()
{
    m_notAnyColor = true;
    bgColorButton->setColor(palette().base().color());
    QPalette palette = preview->palette();
    palette.setColor(preview->backgroundRole(), this->palette().base().color());
    preview->setPalette(palette);
}

void LayoutPagePattern::slotSetBackgroundColor(const QColor &_color)
{
    bgColor = _color;
    QPalette palette = preview->palette();
    palette.setColor(preview->backgroundRole(), bgColor);
    preview->setPalette(palette);
    m_notAnyColor = false;
}

void LayoutPagePattern::slotSetColorButton(const QColor &_color)
{
    currentColor = _color;

    for (int idx = 0; idx < BRUSH_COUNT; ++idx)
        brush[idx]->setBrushColor(currentColor);
    preview->setBrushColor(currentColor);
}

void LayoutPagePattern::slotUnselectOthers(BrushSelect *_p)
{
    selectedBrush = _p;

    for (int idx = 0; idx < BRUSH_COUNT; ++idx)
        if (brush[idx] != _p)
            brush[idx]->slotUnselect();

    preview->setBrushStyle(selectedBrush->getBrushStyle());
}

void LayoutPagePattern::apply(Style *style, bool partial)
{
    Style defaultStyle;

    QBrush brush = defaultStyle.backgroundBrush();
    if (selectedBrush)
        brush = QBrush(selectedBrush->getBrushColor(), selectedBrush->getBrushStyle());
    if (shouldApplyLayoutChange(brush != defaultStyle.backgroundBrush(), brush != m_brush, partial))
        style->setBackgroundBrush(brush);

    QColor c = bgColor;
    if (m_notAnyColor)
        c = defaultStyle.backgroundColor();
    if (shouldApplyLayoutChange(c != defaultStyle.backgroundColor(), c != m_bgColor, partial))
        style->setBackgroundColor(c);
}

void LayoutPagePattern::loadFrom(const Style &style, bool /*partial*/)
{
    m_bgColor = style.backgroundColor();
    if (!style.hasAttribute(Style::BackgroundColor))
        m_notAnyColor = true;
    else
        slotSetBackgroundColor(m_bgColor);
    bgColorButton->setColor(bgColor);

    m_brush = style.backgroundBrush();
    preview->setPattern(m_brush.color(), m_brush.style());
    preview->slotSelect();
    for (int idx = 0; idx < BRUSH_COUNT; ++idx)
        if (m_brush.style() == getBrushStyle(idx))
            brush[idx]->slotSelect();
    color->setColor(m_brush.color());
    slotSetColorButton(m_brush.color());
}
