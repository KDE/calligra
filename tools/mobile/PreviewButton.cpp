/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Sugnan Prabhu S <sugnan.prabhu@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QMouseEvent>

#include "PreviewButton.h"

PreviewButton::PreviewButton(const QIcon & icon, const QString & text, QWidget * parent) :
    QPushButton(icon,text,parent)
{
    installEventFilter(this);
    timer = new QTimer(this);
}

void PreviewButton::rightButtonClicked()
{
    QMenu *menu=new QMenu();
    QAction *actionNewSlide=new QAction("new",this);
    QAction *actionMoveSlide=new QAction("move",this);
    QAction *actionDeleteSlide=new QAction("delete",this);

    menu->addAction(actionNewSlide);
    menu->addAction(actionMoveSlide);
    menu->addAction(actionDeleteSlide);

    QObject::connect(actionNewSlide,SIGNAL(triggered()),this,SIGNAL(newSlide()));
    QObject::connect(actionMoveSlide,SIGNAL(triggered()),this,SIGNAL(moveSlide()));
    QObject::connect(actionDeleteSlide,SIGNAL(triggered()),this,SIGNAL(deleteSlide()));
    menu->exec(pos);

    disconnect(timer, SIGNAL(timeout()), this, SLOT(rightButtonClicked()));
    emit moveSlide();
}

bool PreviewButton::eventFilter(QObject *obj, QEvent *event)
{
       if (event->type() == QEvent::MouseButtonRelease) {
        disconnect(timer, SIGNAL(timeout()), this, SLOT(rightButtonClicked()));
       }

    if (event->type() == QEvent::MouseButtonPress) {
        timer->start(800);
        connect(timer, SIGNAL(timeout()), this, SLOT(rightButtonClicked()));
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
        pos.setX(mouseEvent->globalX());
        pos.setY(mouseEvent->globalY());
        return false;
    }
    else
        return QObject::eventFilter(obj, event);

}
void PreviewButton::setSlideNumber(int num)
{
    this->slideNumber=num;
}
int PreviewButton::getSlideNumber()
{
    return this->slideNumber;
}
