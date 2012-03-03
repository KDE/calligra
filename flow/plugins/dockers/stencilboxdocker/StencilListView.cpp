/*
 * Copyright 2012  Yue Liu <yue.liu@mail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include "StencilListView.h"

ShapeItemDelegate::ShapeItemDelegate(QWidget *parent) {}

ShapeItemDelegate::~ShapeItemDelegate() {}

void ShapeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter,option,index);

    painter->save();

    QFont font = QApplication::font();
    //font.setPixelSize(font.weight()+);
    QFontMetrics fm(font);

    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    QString text = qvariant_cast<QString>(index.data(Qt::DisplayRole));

    QSize iconsize = icon.actualSize(option.decorationSize);

    QRect iconRect = option.rect;
    QRect textRect = option.rect;

    iconRect.setRight(iconsize.width()+16);
    iconRect.setTop(iconRect.top()+1);
    textRect.setTop(iconRect.bottom()+1);
    textRect.setBottom(textRect.top()+fm.height());

    painter->drawPixmap(QPoint(iconRect.left()+8,iconRect.top()+2),icon.pixmap(iconsize.width(),iconsize.height()));

    painter->setFont(font);
    painter->drawText(textRect,text);

    painter->restore();
}

QSize ShapeItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    QSize iconsize = icon.actualSize(option.decorationSize);
    QFont font = QApplication::font();
    QFontMetrics fm(font);

    return(QSize(iconsize.width()+16,iconsize.height()+fm.height()+2 ));
}

StencilListView::StencilListView(QObject *parent) :
    QListView(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setFrameShape(QFrame::NoFrame);
    setIconSize(QSize(48, 48));
    setItemDelegate(new ShapeItemDelegate(this));
    setMovement(QListView::Static);
    setSpacing(1);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setResizeMode(QListView::Adjust);

    //setSelectionMode(QAbstractItemView::NoSelection);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDropIndicatorShown(true);

    //connect(this, SIGNAL(pressed(QModelIndex)), this, SLOT(slotPressed(QModelIndex)));
    setEditTriggers(QAbstractItemView::AnyKeyPressed);
}


/*void ShapeListView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_startPos = event->pos();
    }
    QListView::mousePressEvent(event);
}

void ShapeListView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        int distance = (event->pos() - m_startPos).manhattanLength();
        if (distance >= QApplication::startDragDistance())
        {
            ;//performDrag();
        }
    }
    QListView::mouseMoveEvent(event);
}

void ShapeListView::performDrag()
{
    QListViewItem *item = currentItem();
    if (item) {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(item->text());

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(QPixmap(":/images/person.png"));
        if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
            delete item;
    }
}*/
