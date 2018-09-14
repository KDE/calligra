/* This file is part of the KDE project
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * Copyright (C) 2002 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
 * Copyright (C) 2002 Rob Buis <buis@kde.org>
 * Copyright (C) 2004 Laurent Montel <montel@kde.org>
 * Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * Copyright (C) 2005 Inge Wallin <inge@lysator.liu.se>
 * Copyright (C) 2005, 2011 Thomas Zander <zander@kde.org>
 * Copyright (C) 2005-2008 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2006 C. Boemann <cbo@boemann.dk>
 * Copyright (C) 2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * Copyright (C) 2011 Thorsten Zachmann <zachmann@kde.com>
 * Copyright (C) 2018 Dag Andersen <danders@get2net.dk>
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

#ifndef KOCHART_STROKECONFIGWIDGET_H
#define KOCHART_STROKECONFIGWIDGET_H

#include <QWidget>

class KoUnit;
class KoShapeStrokeModel;
class KoMarker;
class KoCanvasBase;
class KoShapeStroke;

namespace KoChart {

class ChartShape;

/// A widget for configuring the stroke of a pen
class StrokeConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StrokeConfigWidget(QWidget *parent);
    ~StrokeConfigWidget();

    void open(ChartShape *chart);
    void updateData();

    // Getters
    Qt::PenStyle lineStyle() const;
    QVector<qreal> lineDashes() const;
    qreal lineWidth() const;
    QColor color() const;
    qreal miterLimit() const;
    Qt::PenCapStyle capStyle() const;
    Qt::PenJoinStyle joinStyle() const;

private Q_SLOTS:
    /// apply line changes to the selected shape
    void applyChanges();

    void colorButtonClicked(const QColor &color);

private:
    void blockChildSignals(bool block);

private:
    class Private;
    Private * const d;
};

} // namespace KoChart

#endif // KOCHART_STROKECONFIGWIDGET_H
