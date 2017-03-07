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
#include "KPrTimeLineHeader.h"
#include "KPrTimeLineView.h"
#include "KPrShapeAnimations.h"
#include "animations/KPrShapeAnimation.h"
#include "tools/animationtool/KPrAnimationGroupProxyModel.h"

//QT HEADERS
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAbstractTableModel>
#include <QScrollArea>
#include <QPainter>
#include <QFontMetrics>
#include <QScrollBar>


//default value for invalid columns and rows index
const int INVALID = -1;

//Max value for time scale
const int SCALE_LIMIT = 1000;
const int START_COLUMN = (int)KPrShapeAnimations::ShapeThumbnail;
const int END_COLUMN = (int)KPrShapeAnimations::StartTime;

KPrAnimationsTimeLineView::KPrAnimationsTimeLineView(QWidget *parent)
    : QWidget(parent)
    , m_model(0)
    , m_shapeModel(0)
    , m_selectedRow(INVALID)
    , m_selectedColumn(INVALID)
    , m_rowsHeight(50)
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
    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SIGNAL(clicked(QModelIndex)));
    connect(m_view, SIGNAL(timeValuesChanged(QModelIndex)), this, SIGNAL(timeValuesChanged(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(requestContextMenu(QPoint)));
}

void KPrAnimationsTimeLineView::setModel(KPrAnimationGroupProxyModel *model)
{
    m_model = model;
    m_shapeModel = dynamic_cast<KPrShapeAnimations *>(model->sourceModel());
    Q_ASSERT(m_shapeModel);
    updateColumnsWidth();
    connect(m_shapeModel, SIGNAL(layoutChanged()), this, SLOT(updateColumnsWidth()));
    connect(m_shapeModel, SIGNAL(layoutChanged()), this, SLOT(resetData()));
    connect(m_shapeModel, SIGNAL(layoutChanged()), this, SIGNAL(layoutChanged()));
    connect(m_shapeModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(update()));
    //It works only if one item could be selected each time
    connect(m_shapeModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(notifyTimeValuesChanged(QModelIndex)));
    connect(m_shapeModel, SIGNAL(timeScaleModified()), this, SLOT(adjustScale()));
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

KPrAnimationGroupProxyModel *KPrAnimationsTimeLineView::model()
{
    return m_model;
}

KPrShapeAnimations *KPrAnimationsTimeLineView::animationsModel()
{
    return m_shapeModel;
}

int KPrAnimationsTimeLineView::widthOfColumn(int column) const
{
    switch (column) {
    case KPrShapeAnimations::ShapeThumbnail:
        return rowsHeight() * 3 / 2;
    case KPrShapeAnimations::AnimationIcon:
        return rowsHeight() * 5 / 4;
    case KPrShapeAnimations::StartTime:
        return 2 * (rowsHeight() * 2 / 3 + rowsHeight() * 10 / 4 + 10);
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
                                rowsHeight() * index.row());
}

int KPrAnimationsTimeLineView::rowsHeight() const
{
    return m_rowsHeight;
}

int KPrAnimationsTimeLineView::totalWidth() const
{
    int width = 0;
    for (int i = 0; i < KPrShapeAnimations::Duration; i++){
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

void KPrAnimationsTimeLineView::incrementScale(int step)
{
    if ((numberOfSteps() + step) < SCALE_LIMIT) {
        setNumberOfSteps(numberOfSteps() + step);
        m_header->update();
        m_view->update();
    }
}

void KPrAnimationsTimeLineView::changeStartLimit(const int row)
{
    // If user wants a after_previous animation start before previous animation switch to with_previous
    QModelIndex index = m_model->index(row, 0);
    if (index.isValid()) {
        QModelIndex sourceIndex = m_model->mapToSource(index);
        m_shapeModel->recalculateStart(sourceIndex);
    }
}

void KPrAnimationsTimeLineView::adjustScale()
{
    m_maxLength = 10;
    for (int row = 0; row < m_model->rowCount(); ++ row){
        int startOffSet = calculateStartOffset(row);
        qreal length = m_model->data(m_model->index(row, KPrShapeAnimations::StartTime)).toInt() +
                m_model->data(m_model->index(row,KPrShapeAnimations:: Duration)).toInt() + startOffSet;
        length = length / 1000;
        if (length > m_maxLength) {
            m_maxLength = length;
        }
    }
    const int spacing = 2;
    // Increment Scale if maxLength is out of range
    if ((m_maxLength + spacing * stepsScale()) > (numberOfSteps())) {
        incrementScale(m_maxLength + spacing * stepsScale() - numberOfSteps());
        m_header->update();
    }
    // Decrement scale if maxLength is too short
    if ((m_maxLength - spacing * stepsScale()) < (numberOfSteps())) {
        incrementScale(m_maxLength + spacing * stepsScale() - numberOfSteps());
        m_header->update();
    }
}

void KPrAnimationsTimeLineView::notifyTimeValuesChanged(const QModelIndex &index)
{
    QModelIndex newIndex = m_model->mapFromSource(index);
    emit timeValuesChanged(newIndex);
}

void KPrAnimationsTimeLineView::requestContextMenu(QPoint pos)
{
    emit customContextMenuRequested(m_view->mapToParent(pos));
}

int KPrAnimationsTimeLineView::stepsScale()
{
    // Set step size depending on the scale length
    int stepsNumber = numberOfSteps();
    if (stepsNumber < 15)
        return 1;
    else if (stepsNumber < 50)
        return 2;
    else if (stepsNumber < 100)
        return 5;
    else if (stepsNumber < 200)
        return 10;
    else if (stepsNumber < 300)
        return 20;
    else if (stepsNumber < 500)
        return 25;
    else
        return 60;
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

QColor KPrAnimationsTimeLineView::barColor(int row)
{
    Q_ASSERT(m_model);
    KPrShapeAnimation::PresetClass type =
            static_cast<KPrShapeAnimation::PresetClass>(m_model->data(m_model->index(row, KPrShapeAnimations::AnimationClass)).toInt());
    switch (type) {
        case KPrShapeAnimation::Entrance: return Qt::darkGreen;
        case KPrShapeAnimation::Emphasis: return Qt::blue;
        case KPrShapeAnimation::Custom: return Qt::gray;
        case KPrShapeAnimation::Exit: return Qt::red;
        default: return Qt::gray;
    }
}

int KPrAnimationsTimeLineView::calculateStartOffset(int row) const
{
    //calculate real start
    KPrShapeAnimation::NodeType triggerEvent = static_cast<KPrShapeAnimation::NodeType>(
               m_model->data(m_model->index(row, KPrShapeAnimations::NodeType)).toInt());
    if (row <= 0) {
        return 0;
    }
    if (triggerEvent == KPrShapeAnimation::AfterPrevious) {
        QModelIndex sourceIndex = m_model->mapToSource(m_model->index(row - 1, KPrShapeAnimations::NodeType));
        return m_shapeModel->animationEnd(sourceIndex);
    }
    if (triggerEvent == KPrShapeAnimation::WithPrevious) {
        QModelIndex sourceIndex = m_model->mapToSource(m_model->index(row - 1, KPrShapeAnimations::NodeType));
        return m_shapeModel->animationStart(sourceIndex);
    }
    return 0;
}

int KPrAnimationsTimeLineView::rowCount() const
{
    if (m_model) {
        return m_model->rowCount();
    }
    return 0;
}

QSize KPrAnimationsTimeLineView::sizeHint() const
{
    return QSize(m_view->sizeHint().width(), m_view->sizeHint().height() + m_header->sizeHint().height());
}

int KPrAnimationsTimeLineView::startColumn() const
{
    return START_COLUMN;
}

int KPrAnimationsTimeLineView::endColumn() const
{
    return END_COLUMN;
}

void KPrAnimationsTimeLineView::update()
{
    m_view->update();
    m_view->updateGeometry();
    this->updateGeometry();
    m_header->update();
    QWidget::update();
}

void KPrAnimationsTimeLineView::updateColumnsWidth()
{
    for (int row = 0; row < m_model->rowCount(); ++ row){
        qreal length = m_model->data(m_model->index(row, KPrShapeAnimations::StartTime)).toDouble() +
                m_model->data(m_model->index(row, KPrShapeAnimations::Duration)).toDouble();
        if (length > m_maxLength) {
            m_maxLength = length;
        }
    }
    m_view->setMinimumSize(m_view->minimumSizeHint());
}

void KPrAnimationsTimeLineView::resetData()
{
    m_selectedRow = INVALID;
    m_selectedColumn = INVALID;
    update();
}
