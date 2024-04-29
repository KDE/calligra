/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef SLIDEVIEW_H
#define SLIDEVIEW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QProgressBar>
#include <QWidget>

class SlideLoader;
class GraphicsSlideItem;

class SlideView : public QWidget
{
    friend class GraphicsSlideItem;

private:
    Q_OBJECT
    class SlideGraphicsScene : public QGraphicsScene
    {
    public:
        void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    };
    QGraphicsView view;
    SlideGraphicsScene scene;
    QProgressBar progressBar;
    SlideLoader *loader;
    qreal zoomfactor;
    bool sendingChange;

    void layout();
    bool eventFilter(QObject *obj, QEvent *event);
    void toggleSlideZoom(const GraphicsSlideItem *slide);
private Q_SLOTS:
    void slotUpdateSlides();
    void slotViewChanged();

public:
    explicit SlideView(SlideLoader *loader, QWidget *parent = nullptr);
public Q_SLOTS:
    void setView(qreal zoomFactor, int h, int v);
Q_SIGNALS:
    void viewChanged(qreal zoomFactor, int h, int v);
};

#endif
