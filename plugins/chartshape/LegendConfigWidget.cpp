/* This file is part of the KDE project

   Copyright 2007-2008 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2009      Inge Wallin    <inge@lysator.liu.se>
   Copyright 2018 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Own
#include "LegendConfigWidget.h"
#include "ui_LegendConfigWidget.h"

// KF5
#include <klocalizedstring.h>
#include <KFontChooser>

// KoChart
#include "Legend.h"
#include "FontEditorDialog.h"
#include "commands/ChartTypeCommand.h"
#include "ChartDebug.h"

using namespace KoChart;


class LegendConfigWidget::Private
{
public:
    Private(QWidget *parent);
    ~Private();

    Ui::LegendConfigWidget ui;

    FontEditorDialog legendFontEditorDialog;

};


LegendConfigWidget::Private::Private(QWidget *parent)
{
}

LegendConfigWidget::Private::~Private()
{
}


// ================================================================
//                     class LegendConfigWidget

LegendConfigWidget::LegendConfigWidget()
    : d(new Private(this))
{
    setObjectName("LegendConfigWidget");
    d->ui.setupUi(this);
    // FIXME: after 3.2 release: Implement fonts dialog
    d->ui.legendEditFontButton->hide();

    connect(d->ui.showLegend, SIGNAL(toggled(bool)),
            this,             SIGNAL(showLegendChanged(bool)));

    connect(d->ui.legendTitle, SIGNAL(textChanged(QString)),
            this, SIGNAL(legendTitleChanged(QString)));
    connect(d->ui.legendOrientation, SIGNAL(activated(int)),
            this, SLOT(setLegendOrientation(int)));
    connect(d->ui.legendPosition, SIGNAL(activated(int)),
            this, SLOT(setLegendPosition(int)));
    connect(d->ui.legendAlignment, SIGNAL(activated(int)),
            this, SLOT(setLegendAlignment(int)));

    createActions();
}

LegendConfigWidget::~LegendConfigWidget()
{
    delete d;
}

QAction * LegendConfigWidget::createAction()
{
    return 0;
}

int toIndex(Position pos)
{
    int index = 0;
    switch (pos) {
        case StartPosition: index = 2; break;
        case TopPosition: index = 1; break;
        case EndPosition: index = 0; break;
        case BottomPosition: index = 3; break;
        case TopStartPosition: index = 4; break;
        case TopEndPosition: index = 5; break;
        case BottomStartPosition: index = 6; break;
        case BottomEndPosition: index = 7; break;
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
        case Qt::AlignLeft: index = 0; break;
        case Qt::AlignCenter: index = 1; break;
        case Qt::AlignRight: index = 2; break;
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
    emit legendOrientationChanged((Qt::Orientation)boxEntryIndex);
}
/*
void LegendConfigWidget::setLegendShowTitle(bool show)
{
    if (show) {
        d->ui.legendTitle->setEnabled(true);
        emit legendTitleChanged(d->ui.legendTitle->text());
    } else {
        d->ui.legendTitle->setEnabled(false);
        emit legendTitleChanged("");
    }
}
*/
void LegendConfigWidget::setLegendAlignment(int index)
{
    Qt::Alignment align = Qt::AlignCenter;
    switch (index) {
        case 0: align = Qt::AlignLeft; break;
        case 1: align = Qt::AlignCenter; break;
        case 2: align = Qt::AlignRight; break;
        default:
            break;
    }
    emit legendAlignmentChanged(align);
}

void LegendConfigWidget::setLegendPosition(int index)
{
    Position pos;
    switch (index) {
        case 0: pos = EndPosition;
            break;
        case 1: pos = TopPosition;
            break;
        case 2: pos = BottomPosition;
            break;
        case 3: pos = StartPosition;
            break;
        case 4: pos = TopStartPosition;
            break;
        case 5: pos = TopEndPosition;
            break;
        case 6: pos = BottomStartPosition;
            break;
        case 7: pos = BottomEndPosition;
            break;
        default: // manual
            pos = CenterPosition;
            break;
    }
    emit legendPositionChanged(pos);
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
