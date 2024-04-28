/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * SPDX-FileCopyrightText: 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * SPDX-FileCopyrightText: 2009 Jérémy Lugagne <jejewindsurf@hotmail.com>
 * SPDX-FileCopyrightText: 2009 Johann Hingue <yoan1703@hotmail.fr>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KPRPRESENTATIONDRAWWIDGET_H
#define KPRPRESENTATIONDRAWWIDGET_H

#include "KPrPresentationToolEventForwarder.h"

#include <QLabel>
#include <QVarLengthArray>

class QAction;

struct KPrPresentationDrawPath {
    QVector<QPointF> points;
    QColor color;
    int size;
};

Q_DECLARE_TYPEINFO(KPrPresentationDrawPath, Q_MOVABLE_TYPE);

class KPrPresentationDrawWidget : public KPrPresentationToolEventForwarder
{
    Q_OBJECT

public:
    explicit KPrPresentationDrawWidget(KoPACanvasBase *canvas);
    ~KPrPresentationDrawWidget() override;

    /** Draw on the Presentation */
    void paintEvent(QPaintEvent *event) override;

    /** Get all the mouse event needed to paint */
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

    /** Popup menu for colors and sizes */
    void contextMenuEvent(QContextMenuEvent *event) override;

public Q_SLOTS:
    void updateColor(QAction *);
    void updateColor(const QString &color);
    void updateSize(QAction *);
    void updateSize(int size);

private:
    QIcon buildIconColor(const QColor &);
    QIcon buildIconSize(int);
    QAction *buildActionColor(const QColor &, const QString &);
    QAction *buildActionSize(int);

    bool m_draw;
    int m_penSize;
    QColor m_penColor;
    QVector<KPrPresentationDrawPath> m_pointVectors;
};

#endif /* KPRPRESENTATIONDRAWWIDGET_H */
