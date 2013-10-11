/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef CALLIGRA_COMPONENTS_VIEW_H
#define CALLIGRA_COMPONENTS_VIEW_H

#include <QtQuick/QQuickPaintedItem>

namespace Calligra {
namespace Components {

class Document;

/**
 * \brief The view provides a View object for viewing Documents.
 *
 */

class View : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(Calligra::Components::Document* document READ document WRITE setDocument NOTIFY documentChanged)

public:
    View(QQuickItem* parent = 0);
    virtual ~View();

    virtual void paint(QPainter* painter);

    Document* document() const;
    void setDocument(Document* newDocument);

Q_SIGNALS:
    void linkClicked(const QUrl& url);
    void documentChanged();

protected:
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

private:
    class Private;
    Private* const d;
};

} // Namespace Components
} // Namespace Calligra

Q_DECLARE_METATYPE(Calligra::Components::View*)

#endif // CALLIGRA_COMPONENTS_VIEW_H
