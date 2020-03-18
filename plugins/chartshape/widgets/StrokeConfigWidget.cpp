/* This file is part of the KDE project
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * Copyright (C) 2002 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
 * Copyright (C) 2002-2003 Rob Buis <buis@kde.org>
 * Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2005-2006, 2011 Inge Wallin <inge@lysator.liu.se>
 * Copyright (C) 2005-2008 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2006 C. Boemann <cbo@boemann.dk>
 * Copyright (C) 2006 Peter Simonsson <psn@linux.se>
 * Copyright (C) 2006 Laurent Montel <montel@kde.org>
 * Copyright (C) 2007,2011 Thorsten Zachmann <t.zachmann@zagge.de>
 * Copyright (C) 2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * Copyright (C) 2018 Dag Andersen <danders@get2net.dk>
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

// Own
#include "StrokeConfigWidget.h"

#include "ChartShape.h"
#include "PlotArea.h"

// Qt
#include <QMenu>
#include <QLabel>
#include <QToolButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QColorDialog>

// KF5
#include <klocalizedstring.h>
#include <KColorButton>

// Calligra
#include <KoIcon.h>
#include <KoUnit.h>
#include <KoLineStyleSelector.h>
#include <KoUnitDoubleSpinBox.h>
#include <KoMarkerSelector.h>
#include <KoColorPopupAction.h>
#include <KoMarker.h>
#include <KoShapeStroke.h>
#include <KoPathShape.h>
#include <KoMarkerCollection.h>
#include <KoPathShapeMarkerCommand.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoCanvasResourceManager.h>
#include <KoDocumentResourceManager.h>
#include <KoToolManager.h>
#include <KoSelection.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>
#include <KoShapeStrokeCommand.h>
#include <KoShapeStrokeModel.h>

using namespace KoChart;

class CapNJoinMenu : public QMenu
{
    Q_OBJECT
public:
    CapNJoinMenu(QWidget *parent = 0);
    QSize sizeHint() const override;
    
    KoUnitDoubleSpinBox *miterLimit;
    QButtonGroup        *capGroup;
    QButtonGroup        *joinGroup;
};

CapNJoinMenu::CapNJoinMenu(QWidget *parent)
: QMenu(parent)
{
    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->setMargin(2);

    // The cap group
    capGroup = new QButtonGroup(this);
    capGroup->setExclusive(true);

    QToolButton *button = 0;

    button = new QToolButton(this);
    button->setIcon(koIcon("stroke-cap-butt"));
    button->setCheckable(true);
    button->setToolTip(i18n("Butt cap"));
    capGroup->addButton(button, Qt::FlatCap);
    mainLayout->addWidget(button, 2, 0);

    button = new QToolButton(this);
    button->setIcon(koIcon("stroke-cap-round"));
    button->setCheckable(true);
    button->setToolTip(i18n("Round cap"));
    capGroup->addButton(button, Qt::RoundCap);
    mainLayout->addWidget(button, 2, 1);

    button = new QToolButton(this);
    button->setIcon(koIcon("stroke-cap-square"));
    button->setCheckable(true);
    button->setToolTip(i18n("Square cap"));
    capGroup->addButton(button, Qt::SquareCap);
    mainLayout->addWidget(button, 2, 2, Qt::AlignLeft);

    // The join group
    joinGroup = new QButtonGroup(this);
    joinGroup->setExclusive(true);

    button = new QToolButton(this);
    button->setIcon(koIcon("stroke-join-miter"));
    button->setCheckable(true);
    button->setToolTip(i18n("Miter join"));
    joinGroup->addButton(button, Qt::MiterJoin);
    mainLayout->addWidget(button, 3, 0);

    button = new QToolButton(this);
    button->setIcon(koIcon("stroke-join-round"));
    button->setCheckable(true);
    button->setToolTip(i18n("Round join"));
    joinGroup->addButton(button, Qt::RoundJoin);
    mainLayout->addWidget(button, 3, 1);

    button = new QToolButton(this);
    button->setIcon(koIcon("stroke-join-bevel"));
    button->setCheckable(true);
    button->setToolTip(i18n("Bevel join"));
    joinGroup->addButton(button, Qt::BevelJoin);
    mainLayout->addWidget(button, 3, 2, Qt::AlignLeft);

    // Miter limit
    // set min/max/step and value in points, then set actual unit
    miterLimit = new KoUnitDoubleSpinBox(this);
    miterLimit->setMinMaxStep(0.0, 1000.0, 0.5);
    miterLimit->setDecimals(2);
    miterLimit->setUnit(KoUnit(KoUnit::Point));
    miterLimit->setToolTip(i18n("Miter limit"));
    mainLayout->addWidget(miterLimit, 4, 0, 1, 3);

    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    setLayout(mainLayout);
}

QSize CapNJoinMenu::sizeHint() const
{
    return layout()->sizeHint();
}


class Q_DECL_HIDDEN StrokeConfigWidget::Private
{
public:
    Private()
        : chart(0)
        , plotArea(0)
    {
    }

    ChartShape *chart;
    PlotArea *plotArea;

    KoLineStyleSelector *lineStyle;
    KoUnitDoubleSpinBox *lineWidth;

    CapNJoinMenu *capNJoinMenu;
    KColorButton *colorButton;
    QColor color;
    QIcon defaultIcon;
    QIcon blackIcon;
    QWidget *spacer;
};

StrokeConfigWidget::StrokeConfigWidget(QWidget * parent)
    : QWidget(parent)
    , d(new Private())
{
    setObjectName("Stroke widget");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);

    QHBoxLayout *firstLineLayout = new QHBoxLayout();

    // Line style
    d->lineStyle = new KoLineStyleSelector(this);
    d->lineStyle->setMinimumWidth(50);
    firstLineLayout->addWidget(d->lineStyle);

    // Line width, NOTE: KChart does not handle points
    d->lineWidth = new KoUnitDoubleSpinBox(this);
    d->lineWidth->setMinMaxStep(0.0, 1000.0, 1.0);
    d->lineWidth->setDecimals(0);
    d->lineWidth->setUnit(KoUnit(KoUnit::Point));
    d->lineWidth->setToolTip(i18n("Set line width of actual selection"));
    firstLineLayout->addWidget(d->lineWidth);

    QToolButton *capNJoinButton = new QToolButton(this);
    capNJoinButton->setMinimumHeight(25);
    d->capNJoinMenu = new CapNJoinMenu(this);
    capNJoinButton->setMenu(d->capNJoinMenu);
    capNJoinButton->setText("...");
    capNJoinButton->setPopupMode(QToolButton::InstantPopup);

    firstLineLayout->addWidget(capNJoinButton);

    d->colorButton = new KColorButton(this);
    d->colorButton->setObjectName("colorButton");
    d->colorButton->setToolTip(i18n("Change the color of the line"));

    firstLineLayout->addWidget(d->colorButton);

    mainLayout->addLayout(firstLineLayout);

    // Spacer
    d->spacer = new QWidget();
    d->spacer->setObjectName("SpecialSpacer");
    mainLayout->addWidget(d->spacer);

    // set sensitive defaults
    d->lineStyle->setLineStyle(Qt::SolidLine);
    d->lineWidth->changeValue(1);

    // Make the signals visible on the outside of this widget.
    connect(d->lineStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(applyChanges()));
    connect(d->lineWidth, SIGNAL(valueChangedPt(qreal)), this, SLOT(applyChanges()));
    connect(d->capNJoinMenu->capGroup, SIGNAL(buttonClicked(int)), this, SLOT(applyChanges()));
    connect(d->capNJoinMenu->joinGroup, SIGNAL(buttonClicked(int)), this, SLOT(applyChanges()));
    connect(d->capNJoinMenu->miterLimit, SIGNAL(valueChangedPt(qreal)), this, SLOT(applyChanges()));
    connect(d->colorButton, &KColorButton::changed, this, &StrokeConfigWidget::colorButtonClicked);
}

StrokeConfigWidget::~StrokeConfigWidget()
{
    delete d;
}

void StrokeConfigWidget::open(ChartShape *chart)
{
    if (d->chart) {
        
    }
    if (chart) {
        d->chart = chart;
        d->plotArea = chart->plotArea();
    }
}

void StrokeConfigWidget::updateData()
{
    blockChildSignals(true);
    
    QPen stroke = d->plotArea->stockRangeLinePen();
    d->lineWidth->changeValue(stroke.widthF());
    QAbstractButton *button = d->capNJoinMenu->capGroup->button(stroke.capStyle());
    if (button) {
        button->setChecked(true);
    }
    button = d->capNJoinMenu->joinGroup->button(stroke.joinStyle());
    if (button) {
        button->setChecked(true);
    }
    d->capNJoinMenu->miterLimit->changeValue(stroke.miterLimit());
    d->capNJoinMenu->miterLimit->setEnabled(stroke.joinStyle() == Qt::MiterJoin);
    d->lineStyle->setLineStyle(stroke.style(), stroke.dashPattern());
    d->color = stroke.color();
    QString style = QString("QToolButton#colorButton { background-color: %1 }").arg(d->color.name());
    d->colorButton->setStyleSheet(style);
    
    blockChildSignals(false);
}


Qt::PenStyle StrokeConfigWidget::lineStyle() const
{
    return d->lineStyle->lineStyle();
}

QVector<qreal> StrokeConfigWidget::lineDashes() const
{
    return d->lineStyle->lineDashes();
}

qreal StrokeConfigWidget::lineWidth() const
{
    return d->lineWidth->value();
}

QColor StrokeConfigWidget::color() const
{
    return d->color;
}

qreal StrokeConfigWidget::miterLimit() const
{
    return d->capNJoinMenu->miterLimit->value();
}

Qt::PenCapStyle StrokeConfigWidget::capStyle() const
{
    return static_cast<Qt::PenCapStyle>(d->capNJoinMenu->capGroup->checkedId());
}

Qt::PenJoinStyle StrokeConfigWidget::joinStyle() const
{
    return static_cast<Qt::PenJoinStyle>(d->capNJoinMenu->joinGroup->checkedId());
}

void StrokeConfigWidget::blockChildSignals(bool block)
{
    d->lineWidth->blockSignals(block);
    d->capNJoinMenu->capGroup->blockSignals(block);
    d->capNJoinMenu->joinGroup->blockSignals(block);
    d->capNJoinMenu->miterLimit->blockSignals(block);
    d->lineStyle->blockSignals(block);
}

void StrokeConfigWidget::applyChanges()
{
    QPen pen;
    pen.setColor(color());
    pen.setWidth(lineWidth());
    pen.setCapStyle(static_cast<Qt::PenCapStyle>(d->capNJoinMenu->capGroup->checkedId()));
    pen.setJoinStyle(static_cast<Qt::PenJoinStyle>(d->capNJoinMenu->joinGroup->checkedId()));
    pen.setMiterLimit(miterLimit());
    pen.setStyle(lineStyle());
    pen.setDashPattern(lineDashes());

    d->plotArea->setStockRangeLinePen(pen);
    d->plotArea->plotAreaUpdate();
    d->chart->update();
}

void StrokeConfigWidget::colorButtonClicked(const QColor &color)
{
    d->color = color;
    applyChanges();
}

#include "StrokeConfigWidget.moc"
