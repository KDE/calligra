/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-FileCopyrightText: 2013 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef LINKAREA_H
#define LINKAREA_H

#include <QQuickItem>

#include "Document.h"

namespace Calligra
{
namespace Components
{
class LinkArea : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(Calligra::Components::Document *document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(float controllerZoom READ controllerZoom WRITE setControllerZoom NOTIFY controllerZoomChanged)

public:
    explicit LinkArea(QQuickItem *parent = nullptr);
    ~LinkArea() override;

    Calligra::Components::Document *document() const;
    void setDocument(Calligra::Components::Document *newDocument);

    float controllerZoom() const;
    void setControllerZoom(float newZoom);

Q_SIGNALS:
    void clicked();
    void doubleClicked();
    void linkClicked(QUrl linkTarget);

    void documentChanged();
    void controllerZoomChanged();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    class Private;
    Private *d;
};
} // Namespace Components
} // Namespace Calligra
#endif // LINKAREA_H
