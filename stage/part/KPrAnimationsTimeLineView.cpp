#include "KPrAnimationsTimeLineView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QAbstractItemModel>
#include <QScrollArea>
#include <QPainter>
#include <QFontMetrics>
#include <QHeaderView>
#include <QDebug>
#include <QScrollBar>
#include "KPrAnimationsDataModel.h"
#include "KPrTimeLineHeader.h"
#include "KPrTimeLineView.h"
#include "animations/KPrAnimationStep.h"

const int Invalid = -1;
const int scaleLimit = 1000;
KPrAnimationsTimeLineView::KPrAnimationsTimeLineView(QWidget *parent)
    : QWidget(parent)
    , m_model(0)
    , m_selectedRow(Invalid)
    , m_selectedColumn(Invalid)
    , m_rowsHeigth(50)
    , m_stepsNumber(10)
    , m_scaleOversize(0)
    , m_nameWidth(0)
    , m_maxLength(0.0)
{

    QFontMetrics fm(font());
    m_orderColumnWidth = fm.width(QString("W99W"));
    m_shapeNameColumnWidth = fm.width(QString("WMMMW"));

    m_view = new KPrTimeLineView(this);
    m_header = new KPrTimeLineHeader(this);
    m_scrollArea = new QScrollArea;
    m_scrollArea->setBackgroundRole(QPalette::Light);
    m_scrollArea->setWidget(m_view);
    m_scrollArea->installEventFilter(m_view);
    m_scrollArea->installEventFilter(m_header);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_header);
    layout->addWidget(m_scrollArea);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    setLayout(layout);
    //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(m_view, SIGNAL(clicked(const QModelIndex&)), this, SIGNAL(clicked(const QModelIndex&)));
}

void KPrAnimationsTimeLineView::setModel(KPrAnimationsDataModel *model)
{
    m_model = model;
    updateColumnsWidth();
    connect(m_model, SIGNAL(dataReinitialized()), this, SLOT(updateColumnsWidth()));
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
    case 0:
        return m_orderColumnWidth;
    case 1:
        return m_shapeNameColumnWidth;
    case 2:
        return rowsHeigth()*3/2;
    case 3:
    case 4:
        return rowsHeigth()*5/4;
    case 5:
        return 2*(m_orderColumnWidth+m_shapeNameColumnWidth+
                          +rowsHeigth()*2/3+rowsHeigth()*10/4 + 10);
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

void KPrAnimationsTimeLineView::setCurrentIndex(const QModelIndex &index)
{
    setSelectedRow(index.row());
    setSelectedColumn(index.column());
    m_scrollArea->ensureVisible(widthOfColumn(index.row()),
                                rowsHeigth()*index.row());
}

int KPrAnimationsTimeLineView::rowsHeigth() const
{
    return m_rowsHeigth;
}

int KPrAnimationsTimeLineView::totalWidth() const
{
    int width = 0;
    for (int i = 0;i < 6;i++){
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
    if ((numberOfSteps() + 1) < 1000) {
        setNumberOfSteps(numberOfSteps()+1);
        m_header->update();
        m_view->update();
    }
}

void KPrAnimationsTimeLineView::adjustScale()
{
    m_maxLength = 10;
    for (int row = 0; row < m_model->rowCount(); ++ row){
        qreal length = m_model->data(m_model->index(row, 5)).toDouble() + m_model->data(m_model->index(row, 6)).toDouble();
        if (length > m_maxLength)
            m_maxLength = length;
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
    else if (numberOfSteps() < 1000)
        return 60;
    return 1;
}

qreal KPrAnimationsTimeLineView::maxLineLength() const
{
    return m_maxLength;
}

void KPrAnimationsTimeLineView::setMaxLineLength(qreal length)
{
    if (length > 0)
        m_maxLength = length;
}

QColor KPrAnimationsTimeLineView::colorforRow(int row)
{
    if (m_model) {
        KPrAnimationStep::Preset_Class type = static_cast<KPrAnimationStep::Preset_Class>(m_model->data(m_model->index(row, 7)).toInt());
        if (type == KPrAnimationStep::Entrance) {
            return Qt::darkGreen;
        } else if (type == KPrAnimationStep::Emphasis) {
            return Qt::blue;
        } else if (type == KPrAnimationStep::Custom) {
            return Qt::gray;
        } else if (type == KPrAnimationStep::Exit) {
            return Qt::red;
        }
    }
    return Qt::gray;
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
        int size = m_model->data(m_model->index(row, 1)).toString().length();
        qreal length = m_model->data(m_model->index(row, 5)).toDouble() + m_model->data(m_model->index(row, 6)).toDouble();
        if (size > m_nameWidth)
            m_nameWidth = size;
        if (length > m_maxLength)
            m_maxLength = length;
    }
    QFontMetrics fm(font());
    m_shapeNameColumnWidth = fm.width(QString("%1W").arg(QString(m_nameWidth, 'M')));
    m_view->setMinimumSize(m_view->minimumSizeHint());
}
