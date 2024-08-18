// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "kowidgets_export.h"

#include <QWidget>

class KoCanvasBase;
class KoShape;

class KOWIDGETS_EXPORT KoShapePropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KoShapePropertyWidget(QWidget *widget = nullptr);
    ~KoShapePropertyWidget();

    void setCanvas(KoCanvasBase *canvas);

private:
    void selectionChanged();
    void addWidgetForShape(KoShape *shape);
    void shapePropertyChanged();
    void canvasResourceChanged(int key, const QVariant &variant);
    void addPlaceholder();

    class Private;
    std::unique_ptr<Private> d;
};
