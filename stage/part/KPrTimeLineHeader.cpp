#include "KPrTimeLineHeader.h"

#include <QPainter>
#include <QScrollArea>
#include <QEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QDebug>
#include "KPrAnimationsTimeLineView.h"

const int headerHeigth = 20;

KPrTimeLineHeader::KPrTimeLineHeader(QWidget *parent)
    :QWidget(parent)
{
    m_mainView = qobject_cast<KPrAnimationsTimeLineView*>(parent);
    Q_ASSERT(m_mainView);
    setMinimumSize(minimumSizeHint());
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize KPrTimeLineHeader::minimumSizeHint() const
{
    return QSize(m_mainView->totalWidth()*0.25, headerHeigth);
}

void KPrTimeLineHeader::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    paintHeader(&painter, height());
    painter.setPen(QPen(palette().button().color().darker(), 0.5));
    painter.drawRect(0, 0, width(), height());
}

void KPrTimeLineHeader::paintHeader(QPainter *painter, const int RowHeight)
{
    int scroll = m_mainView->scrollArea()->horizontalScrollBar()->value();
    QFontMetrics fm(font());
    int minimumSize = fm.width(QString("W%1W").arg("seconds"));
    if (scroll < (m_mainView->totalWidth()-m_mainView->widthOfColumn(5)-minimumSize)) {
        //Seconds Header
        QRect rect(0,0,m_mainView->totalWidth()-m_mainView->widthOfColumn(5)-scroll, RowHeight);
        paintHeaderItem(painter, rect, QString("seconds"));
    } else if (scroll < (m_mainView->totalWidth()-m_mainView->widthOfColumn(5))-2) {
        QRect rect(0,0,m_mainView->totalWidth()-m_mainView->widthOfColumn(5)-scroll, RowHeight);
        paintHeaderItem(painter, rect, QString(""));
    }
    // Paint time scale header
    QRect rect(m_mainView->totalWidth()-m_mainView->widthOfColumn(5)-scroll,
               0, m_mainView->widthOfColumn(5), RowHeight);
    paintHeaderItem(painter, rect, QString());
    paintTimeScale(painter, rect);

}

void KPrTimeLineHeader::paintHeaderItem(QPainter *painter, const QRect &rect, const QString &text)
{
    int x = rect.center().x();
    QLinearGradient gradient(x, rect.top(), x, rect.bottom());
    QColor color = palette().button().color();
    gradient.setColorAt(0, color.lighter(125));
    gradient.setColorAt(1, color.darker(125));
    painter->fillRect(rect, gradient);
    m_mainView->paintItemBorder(painter, palette(), rect);
    painter->setPen(palette().buttonText().color());
    painter->drawText(rect, text, QTextOption(Qt::AlignCenter));
}

void KPrTimeLineHeader::paintTimeScale(QPainter *painter, const QRect &rect)
{
    const int Padding = 3;
    painter->setPen(palette().buttonText().color());
    painter->setFont(QFont("", 8));
    int totalWidth = m_mainView->widthOfColumn(5);
    int stepScale = m_mainView->stepsScale();
    int stepRatio = m_mainView->numberOfSteps()/m_mainView->stepsScale();
    int stepSize = totalWidth/stepRatio;
    for (int x = 0; x < totalWidth-Padding; x+=stepSize) {
        int z=x+rect.x()+Padding;
        if (z >0) {
            // Draw numbers
            qreal number = x/stepSize*stepScale;
            painter->drawText((z-19 > 1 ? z-19 : z-16), rect.y(), 38, rect.height(), Qt::AlignCenter, QString("%1").arg(number));
        }
    }
    // Draw substeps
    const int substeps = 5;
    stepSize = totalWidth / (stepRatio*substeps);
    for (qreal x = 0; x < totalWidth-Padding; x+=stepSize) {
        int z=x+rect.x()+Padding;
        if (z >0) {
            painter->drawLine(z, 1, z, 3);
            painter->drawLine(z, rect.height()-4, z, rect.height()-2);
        }
    }
}

bool KPrTimeLineHeader::eventFilter(QObject *target, QEvent *event)
{
    int ExtraWidth = 5;
    if (QScrollArea *scrollArea = m_mainView->scrollArea()) {
        if (target == scrollArea && event->type() == QEvent::Resize) {
            if (QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event)) {
                QSize size = resizeEvent->size();
                size.setHeight(sizeHint().height());
                int width = size.width() - (ExtraWidth +
                                            scrollArea->verticalScrollBar()->sizeHint().width());
                size.setWidth(width);
                setMinimumSize(QSize(m_mainView->totalWidth() - m_mainView->widthOfColumn(5)-m_mainView->widthOfColumn(4), size.height()));
                resize(size);
            }
        }
    }
    return QWidget::eventFilter(target, event);
}
