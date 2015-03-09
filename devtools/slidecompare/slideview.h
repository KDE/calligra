/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef SLIDEVIEW_H
#define SLIDEVIEW_H

#include <QWidget>
#include <QProgressBar>
#include <QGraphicsView>
#include <QGraphicsScene>

class SlideLoader;
class GraphicsSlideItem;

class SlideView : public QWidget {
friend class GraphicsSlideItem;
private:
    Q_OBJECT
    class SlideGraphicsScene : public QGraphicsScene {
    public:
        void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    };
    QGraphicsView view;
    SlideGraphicsScene scene;
    QProgressBar progressBar;
    SlideLoader* loader;
    qreal zoomfactor;
    bool sendingChange;

    void layout();
    bool eventFilter(QObject *obj, QEvent *event);
    void toggleSlideZoom(const GraphicsSlideItem* slide);
private Q_SLOTS:
    void slotUpdateSlides();
    void slotViewChanged();
public:
    explicit SlideView(SlideLoader* loader, QWidget* parent = 0);
public Q_SLOTS:
    void setView(qreal zoomFactor, int h, int v);
Q_SIGNALS:
    void viewChanged(qreal zoomFactor, int h, int v);
};

#endif
