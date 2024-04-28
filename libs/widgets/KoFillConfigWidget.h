/* This file is part of the KDE project
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * SPDX-FileCopyrightText: 2012 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef FILLCONFIGWIDGET_H
#define FILLCONFIGWIDGET_H

#include "kowidgets_export.h"

#include <QSharedPointer>
#include <QWidget>

class KoCanvasBase;
class KoShapeBackground;
class KoShape;

/// A widget for configuring the fill of a shape
class KOWIDGETS_EXPORT KoFillConfigWidget : public QWidget
{
    Q_OBJECT
    enum StyleButton { None, Solid, Gradient, Pattern };

public:
    explicit KoFillConfigWidget(QWidget *parent);
    ~KoFillConfigWidget() override;

    void setCanvas(KoCanvasBase *canvas);

    KoCanvasBase *canvas();

    /// Returns the list of the selected shape
    /// If you need to use only one shape, call currentShape()
    virtual QList<KoShape *> currentShapes();

    /// Returns the first selected shape of the resource
    virtual KoShape *currentShape();

private Q_SLOTS:
    void styleButtonPressed(int buttonId);

    void noColorSelected();

    /// apply color changes to the selected shape
    void colorChanged();

    /// the gradient of the fill changed, apply the changes
    void gradientChanged(QSharedPointer<KoShapeBackground> background);

    /// the pattern of the fill changed, apply the changes
    void patternChanged(QSharedPointer<KoShapeBackground> background);

protected:
    virtual void shapeChanged();

private:
    /// update the widget with the KoShape background
    void updateWidget(KoShape *shape);

    class Private;
    Private *const d;
};

#endif // FILLCONFIGWIDGET_H
