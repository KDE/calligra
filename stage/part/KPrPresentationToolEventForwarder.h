/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRPRESENTATIONTOOLEVENTFORWARDER_H
#define KPRPRESENTATIONTOOLEVENTFORWARDER_H

#include <QWidget>

class KoPACanvasBase;

class KPrPresentationToolEventForwarder : public QWidget
{
    Q_OBJECT

public:
    explicit KPrPresentationToolEventForwarder(KoPACanvasBase *canvas);
    ~KPrPresentationToolEventForwarder() override;

    void receiveMousePressEvent(QMouseEvent *event);
    void receiveMouseMoveEvent(QMouseEvent *event);
    void receiveMouseReleaseEvent(QMouseEvent *event);
};

#endif
