/* This file is part of the KDE project
 * Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
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
#ifndef KPRPRESENTATIONDRAWWIDGET_H
#define KPRPRESENTATIONDRAWWIDGET_H

#include <QWidget>
#include <QtGui/QLabel>
#include <QVarLengthArray>

#include "KPrViewModePresentation.h"
/*dans le destructeur tu pourras faire un set sur le booléen à False, et dans le constructeur un Set à true*/

class KPrPresentationDrawWidget : public QWidget {

public :
    KPrPresentationDrawWidget(KoPACanvas * canvas);
    ~KPrPresentationDrawWidget();

    void paintEvent(QPaintEvent * event);
    void mouseMoveEvent( QMouseEvent* e );
    void mousePressEvent( QMouseEvent* e );

private :
    QSize m_size;
    QList<QVector<QPointF> > m_pointVectors;
};

#endif /* KPRPRESENTATIONDRAWWIDGET_H */ 
