/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SHAPEPROPERTIESDOCKER_H
#define SHAPEPROPERTIESDOCKER_H

#include <KoCanvasObserverBase.h>
#include <KoDockFactoryBase.h>
#include <QDockWidget>

class KoShape;

/// The shape properties docker show the properties
/// of the currently selected shape
class ShapePropertiesDocker : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT
public:
    explicit ShapePropertiesDocker(QWidget *parent = nullptr);
    ~ShapePropertiesDocker() override;
    /// reimplemented
    QString observerName() const override
    {
        return QStringLiteral("ShapePropertiesDocker");
    }
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;

private Q_SLOTS:
    void selectionChanged();
    void addWidgetForShape(KoShape *shape);
    void shapePropertyChanged();
    virtual void canvasResourceChanged(int key, const QVariant &res);

private:
    class Private;
    Private *const d;
};

#endif // SHAPEPROPERTIESDOCKER_H
