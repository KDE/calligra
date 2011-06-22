/* This file is part of the KDE project
 * Copyright (C) 2011 Steven Kakoczky <steven.kakoczky@gmail.com>
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

#include "KoAnnotationSideBar.h"

KoAnnotationSideBar::KoAnnotationSideBar(QWidget *parent) :
    QWidget(parent)
{
    annotations = new QList<KoBalloon*>();
}

void KoAnnotationSideBar::addAnnotation(QString content, int position)
{
    KoBalloon *curr, *newBalloon;
    int i;
    for(i = 0; i < annotations->size(); ++i)
    {
        curr = annotations->at(i);
        if(curr->y() < position) // TODO: check for relative position in line for same y values
        {
            break; // insert here. if never reached, insert at end
        }
    }
    newBalloon = new KoBalloon(content, position, this);
    newBalloon->resize(this->width(), newBalloon->sizeHint().height());
    newBalloon->move(0, position);
    annotations->insert(i, newBalloon);
    reposition(i);
    newBalloon->setVisible(true);
    newBalloon->setFocus();
    this->repaint();
}

void KoAnnotationSideBar::paintEvent(QPaintEvent *event)
{
    int i;
    KoBalloon *curr;
    QPainter painter(this);
    painter.setBackgroundMode(Qt::OpaqueMode);
    painter.setBackground(QBrush(Qt::gray));

    //setPositions();

    for(i = 0; i < annotations->size(); ++i)
    {
        curr = annotations->at(i);
        curr->paintEvent(event);
        QPoint anchorConnection((curr->pos()).x() - 30, curr->y());// 30 pixels to the left of current balloon
// TODO: change distance based on magnification from viewer
        painter.drawLine(anchorConnection, curr->pos());
    }
}

/*
 * fix collisions with adjacent balloons
 */
void KoAnnotationSideBar::reposition(int index)
{
    if(index < 0 || index >= annotations->size()) return; //just to be safe
    if(annotations->empty()) return;

    KoBalloon *curr, *adj;
    int tempTop, tempBottom, distance, currIndex;
    curr = annotations->at(index);
    // check if it collides with a lower balloon, if it does, move it up and fix any upper collisions
    if(index < annotations->size() - 1)
    {
        currIndex = index + 1;
        adj = annotations->at(currIndex--);
        tempTop = adj->pos().y();
        tempBottom = curr->pos().y() + curr->height();
        while(tempTop < tempBottom)
        {
            distance = tempBottom - tempTop;
            tempTop = curr->pos().y() - distance;
            curr->move(curr->pos().x(), tempTop);
            if(currIndex < 0) break;
            adj = curr;
            curr = annotations->at(currIndex--);
            tempBottom = curr->pos().y() + curr->height();
        }
        // fix if balloons were moved past 0
        tempBottom = 0;
        currIndex++;
        while(tempTop < tempBottom)
        {
            curr->move(curr->pos().x(), tempBottom);
            tempBottom += curr->height();
            if(currIndex >= annotations->size()) break;
            curr = annotations->at(++currIndex);
            tempTop = curr->pos().y();
        }

    }
}// END reposition

/* void KoAnnotationSideBar::setPositions()
{
    int i, newY;
    KoBalloon *curr;
    for(i = 0; i < annotations->size(); ++i)
    {
        curr = annotations->at(i);
        newY = curr->pos().y();
        /* if(lower collision)
               newY = collision->pos().y() - curr->height();
           if(newY < 0) newY = 0;
           if(upper collision)
               newY = collision->pos().y() + collision->height();


    }
} */
