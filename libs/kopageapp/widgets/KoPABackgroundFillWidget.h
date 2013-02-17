/* This file is part of the KDE project
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * Copyright (C) 2013 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
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

#ifndef KOPABACKGROUNDFILLWIDGET_H
#define KOPABACKGROUNDFILLWIDGET_H

#include <KoFillConfigWidget.h>

#include "kopageapp_export.h"

class KoShapeLayer;
class KoPAView;

class KOPAGEAPP_EXPORT KoPABackgroundFillWidget : public KoFillConfigWidget
{
public:
    KoPABackgroundFillWidget(QWidget *parent);

    void setView(KoPAView *view);

private slots:
    void noColorSelected();

    /// apply color changes to the selected shape
    void colorChanged();

    /// the gradient of the fill changed, apply the changes
    void gradientChanged(KoShapeBackground *background);

    /// the pattern of the fill changed, apply the changes
    void patternChanged(KoShapeBackground *background);

    void shapeChanged();

private:
    /// update the widget
    void updateWidget(KoShape *shape);
};

#endif /* KOPABACKGROUNDFILLWIDGET_H */
