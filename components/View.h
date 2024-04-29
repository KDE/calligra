/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CALLIGRA_COMPONENTS_VIEW_H
#define CALLIGRA_COMPONENTS_VIEW_H

#include <QQuickPaintedItem>

namespace Calligra
{
namespace Components
{

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
    Q_PROPERTY(Calligra::Components::Document *document READ document WRITE setDocument NOTIFY documentChanged)
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
    explicit View(QQuickItem *parent = nullptr);
    /**
     * Destructor.
     */
    ~View() override;

    /**
     * Inherited from QQuickPaintedItem.
     */
    void paint(QPainter *painter) override;

    /**
     * Getter for property #document.
     */
    Document *document() const;
    /**
     * Setter for property #document.
     */
    void setDocument(Document *newValue);

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
    void linkClicked(const QUrl &url);

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
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private:
    class Private;
    Private *const d;
};

} // Namespace Components
} // Namespace Calligra

Q_DECLARE_METATYPE(Calligra::Components::View *)

#endif // CALLIGRA_COMPONENTS_VIEW_H
