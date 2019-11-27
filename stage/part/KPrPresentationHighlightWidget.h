/* This file is part of the KDE project
 * Copyright (C) 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * Copyright (C) 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * Copyright (C) 2009 Jérémy Lugagne <jejewindsurf@hotmail.com>
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
    void mouseMoveEvent( QMouseEvent * event ) override;
    void paintEvent( QPaintEvent * event ) override;

private:
    QSize m_size;
    QPoint m_center;
    bool m_blackBackgroundVisibility;
};

#endif /* KPRPRESENTATIONHIGHLIGHTWIDGET_H */

