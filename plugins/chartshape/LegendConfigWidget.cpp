/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2007-2008 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "LegendConfigWidget.h"
#include "ui_LegendConfigWidget.h"

// KF5
#include <KFontChooser>
#include <KLocalizedString>

// KoChart
#include "ChartDebug.h"
#include "FontEditorDialog.h"
#include "Legend.h"
#include "commands/ChartTypeCommand.h"

using namespace KoChart;

class LegendConfigWidget::Private
{
public:
    Ui::LegendConfigWidget ui;

    FontEditorDialog legendFontEditorDialog;
};

// ================================================================
//                     class LegendConfigWidget

LegendConfigWidget::LegendConfigWidget()
    : d(new Private())
{
    setObjectName("LegendConfigWidget");
    d->ui.setupUi(this);
    // FIXME: after 3.2 release: Implement fonts dialog
    d->ui.legendEditFontButton->hide();

    connect(d->ui.showLegend, &QAbstractButton::toggled, this, &LegendConfigWidget::showLegendChanged);

    connect(d->ui.legendTitle, &QLineEdit::textChanged, this, &LegendConfigWidget::legendTitleChanged);
    connect(d->ui.legendOrientation, &QComboBox::activated, this, &LegendConfigWidget::setLegendOrientation);
    connect(d->ui.legendPosition, &QComboBox::activated, this, &LegendConfigWidget::setLegendPosition);
    connect(d->ui.legendAlignment, &QComboBox::activated, this, &LegendConfigWidget::setLegendAlignment);

    createActions();
}

LegendConfigWidget::~LegendConfigWidget()
{
    delete d;
}

QAction *LegendConfigWidget::createAction()
{
    return nullptr;
}

int toIndex(Position pos)
{
    int index = 0;
    switch (pos) {
    case StartPosition:
        index = 2;
        break;
    case TopPosition:
        index = 1;
        break;
    case EndPosition:
        index = 0;
        break;
    case BottomPosition:
        index = 3;
        break;
    case TopStartPosition:
        index = 4;
        break;
    case TopEndPosition:
        index = 5;
        break;
    case BottomStartPosition:
        index = 6;
        break;
    case BottomEndPosition:
        index = 7;
        break;
    case CenterPosition:
    case FloatingPosition:
        index = 8;
        break;
    }
    return index;
}

int toIndex(Qt::Alignment align)
{
    int index = 0;
    switch (align) {
    case Qt::AlignLeft:
        index = 0;
        break;
    case Qt::AlignCenter:
        index = 1;
        break;
    case Qt::AlignRight:
        index = 2;
        break;
    default:
        break;
    }
    return index;
}

void LegendConfigWidget::updateData()
{
    if (!chart) {
        return;
    }
    if (chart->legend()) {
        blockSignals(true);
        d->ui.showLegend->setChecked(chart->legend()->isVisible());

        d->ui.legendTitle->setText(chart->legend()->title());

        d->ui.legendOrientation->setCurrentIndex(chart->legend()->expansion());
        d->ui.legendPosition->setCurrentIndex(toIndex(chart->legend()->legendPosition()));
        d->ui.legendAlignment->setCurrentIndex(toIndex(chart->legend()->alignment()));
        blockSignals(false);
    }
}

void LegendConfigWidget::setLegendOrientation(int boxEntryIndex)
{
    Q_EMIT legendOrientationChanged((Qt::Orientation)boxEntryIndex);
}
/*
void LegendConfigWidget::setLegendShowTitle(bool show)
{
    if (show) {
        d->ui.legendTitle->setEnabled(true);
        Q_EMIT legendTitleChanged(d->ui.legendTitle->text());
    } else {
        d->ui.legendTitle->setEnabled(false);
        Q_EMIT legendTitleChanged("");
    }
}
*/
void LegendConfigWidget::setLegendAlignment(int index)
{
    Qt::Alignment align = Qt::AlignCenter;
    switch (index) {
    case 0:
        align = Qt::AlignLeft;
        break;
    case 1:
        align = Qt::AlignCenter;
        break;
    case 2:
        align = Qt::AlignRight;
        break;
    default:
        break;
    }
    Q_EMIT legendAlignmentChanged(align);
}

void LegendConfigWidget::setLegendPosition(int index)
{
    Position pos;
    switch (index) {
    case 0:
        pos = EndPosition;
        break;
    case 1:
        pos = TopPosition;
        break;
    case 2:
        pos = BottomPosition;
        break;
    case 3:
        pos = StartPosition;
        break;
    case 4:
        pos = TopStartPosition;
        break;
    case 5:
        pos = TopEndPosition;
        break;
    case 6:
        pos = BottomStartPosition;
        break;
    case 7:
        pos = BottomEndPosition;
        break;
    default: // manual
        pos = CenterPosition;
        break;
    }
    Q_EMIT legendPositionChanged(pos);
}

void LegendConfigWidget::createActions()
{
}

void LegendConfigWidget::ui_legendEditFontButtonClicked()
{
    QFont font = chart->legend()->font();
    d->legendFontEditorDialog.fontChooser->setFont(font);
    d->legendFontEditorDialog.show();
}
