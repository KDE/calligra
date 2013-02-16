/* This file is part of the KDE project
 * Copyright (C) 2013 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#ifndef KPRPAGEBACKGROUNDWIDGET_H
#define KPRPAGEBACKGROUNDWIDGET_H

#include <QWidget>

class KoShapeBackground;
class KoShape;
class KoCanvasBase;

class KPrPageBackgroundWidget : public QWidget
{
    Q_OBJECT
    enum StyleButton {
        None,
        Solid,
        Gradient,
        Pattern
    };

public:
    KPrPageBackgroundWidget(QWidget* parent = 0);
    ~KPrPageBackgroundWidget();

    void setCanvas( KoCanvasBase *canvas );

private slots:
    void styleButtonPressed(int buttonId);

    void noColorSelected();

    /// apply color changes to the selected shape
    void colorChanged();

    /// the gradient of the fill changed, apply the changes
    void gradientChanged(KoShapeBackground *background);

    /// the pattern of the fill changed, apply the changes
    void patternChanged(KoShapeBackground *background);

private:
    /// update the widget
    void updateWidget();

    KoShapeBackground *applyFillGradientStops(const QGradientStops &stops);

    void blockChildSignals(bool block);

    class Private;
    Private * const d;
};

#endif /* KPRPAGEBACKGROUNDWIDGET_H */
