/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 * Copyright (C) 2013 Dan Leinir Turthra Jensen <admin@leinir.dk>
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
 *
 */

#ifndef LINKAREA_H
#define LINKAREA_H

#include <QQuickItem>

#include "Document.h"

namespace Calligra {
namespace Components {
class LinkArea : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY( Calligra::Components::Document* document READ document WRITE setDocument NOTIFY documentChanged )
    Q_PROPERTY( float controllerZoom READ controllerZoom WRITE setControllerZoom NOTIFY controllerZoomChanged )

public:
    explicit LinkArea(QQuickItem* parent = 0);
    ~LinkArea() override;

    Calligra::Components::Document* document() const;
    void setDocument( Calligra::Components::Document* newDocument );

    float controllerZoom() const;
    void setControllerZoom(float newZoom);

Q_SIGNALS:
    void clicked();
    void doubleClicked();
    void linkClicked(QUrl linkTarget);

    void documentChanged();
    void controllerZoomChanged();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    class Private;
    Private* d;
};
} // Namespace Components
} // Namespace Calligra
#endif // LINKAREA_H
