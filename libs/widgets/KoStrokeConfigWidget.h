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
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef STROKECONFIGWIDGET_H
#define STROKECONFIGWIDGET_H

#include "kowidgets_export.h"

#include <KoMarkerData.h>
#include <QWidget>

class KoUnit;
class KoShapeStrokeModel;
class KoMarker;
class KoCanvasBase;
class KoShapeStroke;

/// A widget for configuring the stroke of a shape
class KOWIDGETS_EXPORT KoStrokeConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KoStrokeConfigWidget(QWidget *parent);
    ~KoStrokeConfigWidget() override;

    // Getters
    Qt::PenStyle lineStyle() const;
    QVector<qreal> lineDashes() const;
    qreal lineWidth() const;
    QColor color() const;
    qreal miterLimit() const;
    KoMarker *startMarker() const;
    KoMarker *endMarker() const;
    Qt::PenCapStyle capStyle() const;
    Qt::PenJoinStyle joinStyle() const;

    /**
     * Creates KoShapeStroke object filled with the options
     * configured by the widget. The caller is in charge of
     * deletion of the returned object
     */
    KoShapeStroke *createShapeStroke() const;

    void setCanvas(KoCanvasBase *canvas);
    void setActive(bool active);

private Q_SLOTS:
    void updateControls(KoShapeStrokeModel *stroke, KoMarker *startMarker, KoMarker *endMarker);

    void updateMarkers(const QList<KoMarker *> &markers);

    /// start marker has changed
    void startMarkerChanged();
    /// end marker has changed
    void endMarkerChanged();

    void canvasResourceChanged(int key, const QVariant &value);

    /// selection has changed
    void selectionChanged();

    /// apply line changes to the selected shape
    void applyChanges();

private:
    void setUnit(const KoUnit &unit);

    /// apply marker changes to the selected shape
    void applyMarkerChanges(KoMarkerData::MarkerPosition position);

    void blockChildSignals(bool block);

private:
    class Private;
    Private *const d;
};

#endif // SHADOWCONFIGWIDGET_H
