/* This file is part of the KDE project
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * SPDX-FileCopyrightText: 2002 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
 * SPDX-FileCopyrightText: 2002 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2004 Laurent Montel <montel@kde.org>
 * SPDX-FileCopyrightText: 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2005 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2005, 2011 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2005-2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.com>
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCHART_STROKECONFIGWIDGET_H
#define KOCHART_STROKECONFIGWIDGET_H

#include <QWidget>

class KoUnit;
class KoShapeStrokeModel;
class KoMarker;
class KoCanvasBase;
class KoShapeStroke;

namespace KoChart
{

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
    Private *const d;
};

} // namespace KoChart

#endif // KOCHART_STROKECONFIGWIDGET_H
