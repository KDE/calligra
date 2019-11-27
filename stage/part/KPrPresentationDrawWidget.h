/* This file is part of the KDE project
 * Copyright (C) 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * Copyright (C) 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * Copyright (C) 2009 Jérémy Lugagne <jejewindsurf@hotmail.com>
 * Copyright (C) 2009 Johann Hingue <yoan1703@hotmail.fr>
 * Copyright (C) 2009 Thorsten Zachmann <zachmann@kde.org>
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

#include "KPrPresentationToolEventForwarder.h"

#include <QLabel>
#include <QVarLengthArray>

class QAction;

struct KPrPresentationDrawPath
{
    QVector<QPointF> points;
    QColor color;
    int size;
};

Q_DECLARE_TYPEINFO(KPrPresentationDrawPath, Q_MOVABLE_TYPE);


class KPrPresentationDrawWidget : public KPrPresentationToolEventForwarder
{
    Q_OBJECT

public :
    explicit KPrPresentationDrawWidget(KoPACanvasBase *canvas);
    ~KPrPresentationDrawWidget() override;

    /** Draw on the Presentation */
    void paintEvent( QPaintEvent * event ) override;

    /** Get all the mouse event needed to paint */
    void mouseMoveEvent( QMouseEvent * e ) override;
    void mousePressEvent( QMouseEvent * e ) override;
    void mouseReleaseEvent( QMouseEvent * e ) override;

    /** Popup menu for colors and sizes */
    void contextMenuEvent( QContextMenuEvent * event ) override;

public Q_SLOTS:
    void updateColor( QAction * );
    void updateColor( const QString &color );
    void updateSize( QAction * );
    void updateSize( int size );

private :
    QIcon buildIconColor( const QColor& );
    QIcon buildIconSize( int );
    QAction* buildActionColor( const QColor&, const QString& );
    QAction* buildActionSize( int );

    bool m_draw;
    int m_penSize;
    QColor m_penColor;
    QVector<KPrPresentationDrawPath> m_pointVectors;
};

#endif /* KPRPRESENTATIONDRAWWIDGET_H */
