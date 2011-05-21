#include "KPrSlidesManagerView.h"
#include "KPrViewModeSlidesSorter.h"

//Qt headers
#include <QScrollBar>
#include <QMenu>
#include <QtCore/qmath.h>
#include <QPaintEvent>
#include <QtGui>

//Kde libs
#include <klocale.h>
#include <KIconLoader>

//Calligra libs
#include <KoToolProxy.h>



KPrSlidesManagerView::KPrSlidesManagerView(KoToolProxy *toolProxy, QWidget *parent)
    : QListView(parent)
    , m_dragingFlag(false)
    , m_toolProxy(toolProxy)
{
    setViewMode(QListView::IconMode);
    setFlow(QListView::LeftToRight);
    setWrapping(TRUE);
    setResizeMode(QListView::Adjust);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
}

KPrSlidesManagerView::~KPrSlidesManagerView()
{
}

void KPrSlidesManagerView::paintEvent(QPaintEvent *event)
{
    event->accept();
    QListView::paintEvent(event);

    // Paint the line where the slide should go
    int lastItem = lastItemNumber();
    int currentItemNumber = lastItem;

    //20 is for the rigth margin
    int slidesNumber = qFloor((contentsRect().width() - 20)/itemSize().width());

    if (isDraging() && currentItemNumber >= 0) {

        QSize size(itemSize().width(), itemSize().height());

        int numberMod = currentItemNumber % slidesNumber;

        //put line after last slide.
        if (numberMod == 0 && currentItemNumber == model()->rowCount(QModelIndex())) {
            numberMod = slidesNumber;
        }
        int verticalValue = (currentItemNumber - numberMod) / slidesNumber * size.height() - verticalScrollBar()->value();

        QPoint point1(numberMod * size.width(), verticalValue);
        QPoint point2(numberMod * size.width(), verticalValue + size.height());
        QLineF line(point1, point2);

        QPainter painter(this->viewport());
        painter.drawLine(line);
    }
}

void KPrSlidesManagerView::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit slideDblClick();
}

void KPrSlidesManagerView::contextMenuEvent(QContextMenuEvent *event)
{
    emit requestContextMenu(event);
}

void KPrSlidesManagerView::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions);
    QAbstractItemView::startDrag(Qt::MoveAction);
}

void KPrSlidesManagerView::dropEvent(QDropEvent *ev)
{
    setDragingFlag(false);
    ev->setDropAction(Qt::IgnoreAction);
    ev->accept();

    clearSelection();

    int newIndex;
    QModelIndex itemNew = indexAt(ev->pos());
    newIndex = itemNew.row();
    model()->dropMimeData (ev->mimeData (), Qt::MoveAction, newIndex, -1, QModelIndex());
}

void KPrSlidesManagerView::dragMoveEvent(QDragMoveEvent *ev)
{
    ev->accept();
    setDragingFlag ();
    pageBefore(ev->pos());
    viewport()->update();
}

void KPrSlidesManagerView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-koffice-sliderssorter")) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

int KPrSlidesManagerView::pageBefore(QPoint point)
{
    QModelIndex item = indexAt(point);
    int pageBeforeNumber = -1;
    if (item.row()>=0) {
        //normal case
        pageBeforeNumber = item.row();
    } else {
        //after the last slide
        pageBeforeNumber = model()->rowCount(QModelIndex());
    }
    setLastItemNumber(pageBeforeNumber);
    return pageBeforeNumber;
}

void KPrSlidesManagerView::setLastItemNumber(int number)
{
    m_lastItemNumber = number;
}

int KPrSlidesManagerView::lastItemNumber()
{
    return m_lastItemNumber;
}

QRect KPrSlidesManagerView::itemSize() const
{
    return m_itemSize;
}

void KPrSlidesManagerView::setItemSize(QRect size)
{
    m_itemSize = size;
}

void KPrSlidesManagerView::setDragingFlag(bool flag)
{
    m_dragingFlag = flag;
}

bool KPrSlidesManagerView::isDraging()
{
    return m_dragingFlag;
}
