/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRPRESENTATIONBLACKWIDGET_H
#define KPRPRESENTATIONBLACKWIDGET_H

#include "KPrPresentationToolEventForwarder.h"

#include <QSize>

class KPrPresentationBlackWidget : public KPrPresentationToolEventForwarder
{
    Q_OBJECT
public:
    explicit KPrPresentationBlackWidget(KoPACanvasBase *canvas);
    ~KPrPresentationBlackWidget() override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QSize m_size;
};

#endif /* KPRPRESENTATIONBLACKWIDGET_H */
