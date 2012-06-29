/* This file is part of the KDE project
 * Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#include "KPrAnimationsTimeLineView.h"

//Stage Headers
#include "KPrAnimationsDataModel.h"
#include "KPrTimeLineHeader.h"
#include "KPrTimeLineView.h"
#include "animations/KPrShapeAnimation.h"

//KDE HEADERS


//QT HEADERS
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAbstractItemModel>
#include <QScrollArea>
#include <QPainter>
#include <QFontMetrics>
#include <QDebug>
#include <QScrollBar>


//default value for invalid columns and rows index
const int INVALID = -1;

//Max value for time scale
const int SCALE_LIMIT = 1000;

KPrAnimationsTimeLineView::KPrAnimationsTimeLineView(QWidget *parent)
    : QWidget(parent)
    , m_model(0)
    , m_selectedRow(INVALID)
    , m_selectedColumn(INVALID)
    , m_rowsHeigth(50)
    , m_stepsNumber(10)
    , m_scaleOversize(0)
    , m_maxLength(0.0)
{
    //Setup GUI
    m_view = new KPrTimeLineView(this);
    m_header = new KPrTimeLineHeader(this);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    m_scrollArea = new QScrollArea;
    m_scrollArea->setBackgroundRole(QPalette::Light);
    m_scrollArea->setWidget(m_view);
    m_scrollArea->installEventFilter(m_view);
    m_scrollArea->installEventFilter(m_header);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_header);
    layout->addWidget(m_scrollArea);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);

    //Connect Signals
    connect(m_view, SIGNAL(clicked(const QModelIndex&)), this, SIGNAL(clicked(const QModelIndex&)));
    connect(m_view, SIGNAL(timeValuesChanged(QModelIndex)), this, SIGNAL(timeValuesChanged(QModelIndex)));
}

void KPrAnimationsTimeLineView::setModel(KPrAnimationsDataModel *model)
{
    m_model = model;
    updateColumnsWidth();
    connect(m_model, SIGNAL(layoutChanged()), this, SLOT(updateColumnsWidth()));
    connect(m_model, SIGNAL(layoutChanged()), this, SLOT(update()));
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(update()));
    //It works only if one item could be selected each time
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(timeValuesChanged(QModelIndex)));
    adjustScale();
    m_header->update();
    m_view->update();
}

void KPrAnimationsTimeLineView::resizeEvent(QResizeEvent *event)
{
    if (m_scrollArea->horizontalScrollBar()) {
        connect(m_scrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)), m_header, SLOT(repaint()));
    }
    QWidget::resizeEvent(event);
}

KPrAnimationsDataModel *KPrAnimationsTimeLineView::model()
{
    return m_model;
}

int KPrAnimationsTimeLineView::widthOfColumn(int column) const
{
    switch (column) {
    case ShapeThumbnail:
        return rowsHeigth()*3/2;
    case AnimationIcon:
        return rowsHeigth()*5/4;
    case StartTime:
        return 2*(rowsHeigth()*2/3+rowsHeigth()*10/4 + 10);
    default:
        return 0;
    }
    return 0;
}

void KPrAnimationsTimeLineView::setSelectedRow(int row)
{
    m_selectedRow = row;
    m_view->update();
}

void KPrAnimationsTimeLineView::setSelectedColumn(int column)
{
    m_selectedColumn = column;
}

QModelIndex KPrAnimationsTimeLineView::currentIndex()
{
    return m_model->index(m_selectedRow, m_selectedColumn, QModelIndex());
}

void KPrAnimationsTimeLineView::setCurrentIndex(const QModelIndex &index)
{
    setSelectedRow(index.row());
    setSelectedColumn(index.column());
    m_scrollArea->ensureVisible(widthOfColumn(index.row()),
                                rowsHeigth() * index.row());
}

int KPrAnimationsTimeLineView::rowsHeigth() const
{
    return m_rowsHeigth;
}

int KPrAnimationsTimeLineView::totalWidth() const
{
    int width = 0;
    for (int i = 0; i < Duration; i++){
        width = width + widthOfColumn(i);
    }
    return width;
}

void KPrAnimationsTimeLineView::paintItemBorder(QPainter *painter, const QPalette &palette, const QRect &rect)
{
    painter->setPen(QPen(palette.button().color().darker(), 0.33));
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());
    painter->drawLine(rect.bottomRight(), rect.topRight());
}

QScrollArea *KPrAnimationsTimeLineView::scrollArea() const
{
    return m_scrollArea;
}

int KPrAnimationsTimeLineView::numberOfSteps() const
{
    return m_stepsNumber;
}

void KPrAnimationsTimeLineView::setNumberOfSteps(int steps)
{
    m_stepsNumber = steps;
}

void KPrAnimationsTimeLineView::incrementScale()
{
    if ((numberOfSteps() + 1) < SCALE_LIMIT) {
        setNumberOfSteps(numberOfSteps() + 1);
        m_header->update();
        m_view->update();
    }
}

void KPrAnimationsTimeLineView::adjustScale()
{
    m_maxLength = 10;
    for (int row = 0; row < m_model->rowCount(); ++ row){
        int startOffSet = calculateStartOffset(row);
        qreal length = m_model->data(m_model->index(row, StartTime)).toDouble() +
                m_model->data(m_model->index(row, Duration)).toDouble() + startOffSet;
        if (length > m_maxLength) {
            m_maxLength = length;
        }
    }
    const int spacing = 2;
    if ((m_maxLength - spacing * stepsScale()) < (numberOfSteps())) {
        setNumberOfSteps(m_maxLength + spacing * stepsScale());
        m_header->update();
    }
}

int KPrAnimationsTimeLineView::stepsScale()
{
    if (numberOfSteps() < 25)
        return 1;
    else if (numberOfSteps() < 60)
        return 2;
    else if (numberOfSteps() < 120)
        return 5;
    else if (numberOfSteps() < 250)
        return 10;
    else if (numberOfSteps() < 500)
        return 20;
    else if (numberOfSteps() < SCALE_LIMIT)
        return 60;
    return 1;
}

qreal KPrAnimationsTimeLineView::maxLineLength() const
{
    return m_maxLength;
}

void KPrAnimationsTimeLineView::setMaxLineLength(qreal length)
{
    if (length > 0) {
        m_maxLength = length;
    }
}

QColor KPrAnimationsTimeLineView::colorforRow(int row)
{
    if (m_model) {
        KPrShapeAnimation::Preset_Class type =
                static_cast<KPrShapeAnimation::Preset_Class>(m_model->data(m_model->index(row, AnimationClass)).toInt());
        if (type == KPrShapeAnimation::Entrance) {
            return Qt::darkGreen;
        } else if (type == KPrShapeAnimation::Emphasis) {
            return Qt::blue;
        } else if (type == KPrShapeAnimation::Custom) {
            return Qt::gray;
        } else if (type == KPrShapeAnimation::Exit) {
            return Qt::red;
        }
    }
    return Qt::gray;
}

double KPrAnimationsTimeLineView::calculateStartOffset(int row)
{
    //calculate real start
    KPrShapeAnimation::Node_Type triggerEvent = static_cast<KPrShapeAnimation::Node_Type>(
               m_model->data(m_model->index(row, TriggerEvent)).toInt());
    if (triggerEvent == KPrShapeAnimation::After_Previous) {
        return m_model->rootItemEnd();
    }
    return 0;
}

int KPrAnimationsTimeLineView::rowCount() const
{
    if (m_model) {
        return m_model->rowCount(QModelIndex());
    }
    return 0;
}

void KPrAnimationsTimeLineView::update()
{
    m_view->update();
    m_header->update();
    QWidget::update();
}

void KPrAnimationsTimeLineView::updateColumnsWidth()
{
    for (int row = 0; row < m_model->rowCount(); ++ row){
        qreal length = m_model->data(m_model->index(row, StartTime)).toDouble() +
                m_model->data(m_model->index(row, Duration)).toDouble();
        if (length > m_maxLength) {
            m_maxLength = length;
        }
    }
    m_view->setMinimumSize(m_view->minimumSizeHint());
}
