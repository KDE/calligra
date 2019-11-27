/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#ifndef CALLIGRA_COMPONENTS_VIEW_H
#define CALLIGRA_COMPONENTS_VIEW_H

#include <QQuickPaintedItem>

namespace Calligra {
namespace Components {

class Document;

/**
 * \brief The view provides a View object for viewing Documents.
 *
 * The View object provides a view on a document. 
 */

class View : public QQuickPaintedItem
{
    Q_OBJECT
    /**
     * \property document
     * \brief The document this object provides a view on.
     *
     * \default null
     * \get document() const
     * \set setDocument()
     * \notify documentChanged()
     */
    Q_PROPERTY(Calligra::Components::Document* document READ document WRITE setDocument NOTIFY documentChanged)
    /**
     * \property zoom
     * \brief The zoom level the view renders the document at.
     *
     * \note For more extensive control, use a ViewController object.
     *
     * \default -1.0 when #document is null or #document is not loaded. 1.0 otherwise.
     * \get zoom() const
     * \set setZoom()
     * \notify zoomChanged()
     */
    Q_PROPERTY(float zoom READ zoom WRITE setZoom NOTIFY zoomChanged)

public:
    /**
     * Constructor.
     *
     * \param parent The parent item.
     */
    explicit View(QQuickItem* parent = 0);
    /**
     * Destructor.
     */
    ~View() override;

    /**
     * Inherited from QQuickPaintedItem.
     */
    void paint(QPainter* painter) override;

    /**
     * Getter for property #document.
     */
    Document* document() const;
    /**
     * Setter for property #document.
     */
    void setDocument(Document* newValue);

    /**
     * Getter for property #zoom.
     */
    float zoom() const;
    /**
     * Setter for property #zoom.
     */
    void setZoom(float newValue);

Q_SIGNALS:
    /**
     * \brief Emitted when a link in the document is clicked.
     *
     * \param url The URL of the link that was clicked. For internal links, the protocol
     * `document://` will be used.
     */
    void linkClicked(const QUrl& url);

    /**
     * Notify signal for property #document.
     */
    void documentChanged();
    /**
     * Notify signal for property #zoom.
     */
    void zoomChanged();

protected:
    /**
     * Inherited from QQuickPaintedItem.
     */
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;

private:
    class Private;
    Private* const d;
};

} // Namespace Components
} // Namespace Calligra

Q_DECLARE_METATYPE(Calligra::Components::View*)

#endif // CALLIGRA_COMPONENTS_VIEW_H
