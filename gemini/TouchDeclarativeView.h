/* This file is part of the KDE project
 * Copyright (C) Boudewijn Rempt <boud@valdyas.org>, (C) 2013
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef TOUCHDECLARATIVEVIEW_H
#define TOUCHDECLARATIVEVIEW_H

#include <QPointer>
#include <QQuickView>

/**
 * @brief The SketchDeclarativeView class overrides QGraphicsView's drawBackground
 */
class TouchDeclarativeView : public QQuickView
{
    Q_OBJECT

    Q_PROPERTY(bool drawCanvas READ drawCanvas WRITE setDrawCanvas NOTIFY drawCanvasChanged);
    Q_PROPERTY(QWidget *canvasWidget READ canvasWidget WRITE setCanvasWidget NOTIFY canvasWidgetChanged);

public:
    explicit TouchDeclarativeView(QWindow *parent = 0);
    TouchDeclarativeView(const QUrl &url, QWindow *parent = 0);
    virtual ~TouchDeclarativeView();

    QWidget *canvasWidget() const;
    void setCanvasWidget(QWidget *canvasWidget);

    bool drawCanvas() const;
    void setDrawCanvas(bool drawCanvas);

Q_SIGNALS:
    void canvasWidgetChanged();
    void drawCanvasChanged();

protected:
    //     void resizeEvent(QResizeEvent *event);
    //     virtual bool event(QEvent* event);
    //     virtual bool eventFilter(QObject* watched, QEvent* e);

    //     void drawBackground(QPainter *painter, const QRectF &rect);

private:
    bool m_drawCanvas;
    QPointer<QWidget> m_canvasWidget;
    bool m_GLInitialized;
    Q_SLOT void resetInitialized();
};

#endif // TOUCHDECLARATIVEVIEW_H
