/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * SPDX-FileCopyrightText: 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * SPDX-FileCopyrightText: 2009 Jérémy Lugagne <jejewindsurf@hotmail.com>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KPRPRESENTATIONHIGHLIGHTWIDGET_H
#define KPRPRESENTATIONHIGHLIGHTWIDGET_H

#include "KPrPresentationToolEventForwarder.h"

class KPrPresentationHighlightWidget : public KPrPresentationToolEventForwarder
{
    Q_OBJECT
public:
    explicit KPrPresentationHighlightWidget(KoPACanvasBase *canvas);
    ~KPrPresentationHighlightWidget() override;

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QSize m_size;
    QPoint m_center;
    bool m_blackBackgroundVisibility;
};

#endif /* KPRPRESENTATIONHIGHLIGHTWIDGET_H */
