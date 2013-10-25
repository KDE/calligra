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


#ifndef CALLIGRA_COMPONENTS_DOCUMENT_H
#define CALLIGRA_COMPONENTS_DOCUMENT_H

#include <QtCore/QObject>

#include "Global.h"

class KoDocument;
class KoZoomController;
class KoCanvasController;
class QGraphicsWidget;
class KoFindBase;

namespace Calligra {
namespace Components {

/**
 * \brief The Document object provides a loader for Calligra Documents.
 *
 */
class Document : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Calligra::Components::DocumentType::Type documentType READ documentType NOTIFY documentTypeChanged)
    Q_PROPERTY(Calligra::Components::DocumentStatus::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QSize documentSize READ documentSize NOTIFY documentSizeChanged)

    /**
     * \property currentIndex
     * \brief The current visible 'index', i.e. page, sheet or slide.
     *
     * Due to the abstraction of the difference between the three document types
     * we need some way to handle the "current visible item" based on an arbitrary
     * number.
     *
     * \default -1 if #source is not set or failed to load. 0 otherwise.
     * \get currentIndex() const
     * \set setcurrentIndex()
     * \notify currentIndexChanged()
     *
     * \todo This should probably be a property of View, but since DocumentImpl currently
     * creates the canvas it is currently pretty much a document property.
     */
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

public:
    Document(QObject* parent = 0);
    ~Document();

    /**
     * @{
     *
     * \property source
     */
    QUrl source() const;
    void setSource(const QUrl& value);
    /**
     * @}
     */

    DocumentType::Type documentType() const;
    DocumentStatus::Status status() const;
    QSize documentSize() const;

    /**
     * Getter for property #currentIndex.
     */
    int currentIndex() const;
    /**
     * Setter for property #currentIndex.
     */
    void setCurrentIndex(int newValue);

    /**
     * \internal
     * These methods are used internally by the components and not exposed
     * to QML.
     * @{
     */
    KoFindBase* finder() const;
    QGraphicsWidget* canvas() const;
    KoCanvasController* canvasController() const;
    KoZoomController* zoomController() const;
    KoDocument* koDocument() const;

    /**
     * @}
     */
Q_SIGNALS:
    void sourceChanged();
    void statusChanged();
    void documentSizeChanged();
    void documentTypeChanged();

    /**
     * Notify signal for property #currentIndex.
     */
    void currentIndexChanged();

private:
    class Private;
    Private* const d;
};

} // Namespace Components
} // Namespace Calligra

Q_DECLARE_METATYPE(Calligra::Components::Document*)

#endif // CALLIGRA_COMPONENTS_DOCUMENT_H
