/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CALLIGRA_COMPONENTS_DOCUMENT_H
#define CALLIGRA_COMPONENTS_DOCUMENT_H

#include <QObject>

#include "Global.h"

class KoDocument;
class KoZoomController;
class KoCanvasController;
class QGraphicsWidget;
class KoFindBase;

namespace Calligra
{
namespace Components
{

/**
 * \brief The Document object provides a loader for Calligra Documents.
 *
 */
class Document : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QObject *document READ document NOTIFY documentChanged)
    Q_PROPERTY(QObject *part READ part NOTIFY documentChanged)
    Q_PROPERTY(Calligra::Components::DocumentType::Type documentType READ documentType NOTIFY documentTypeChanged)
    Q_PROPERTY(Calligra::Components::DocumentStatus::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QSize documentSize READ documentSize NOTIFY documentSizeChanged)
    Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly NOTIFY readOnlyChanged)

    Q_PROPERTY(QWindow *windowView)
    Q_PROPERTY(KoView *view)

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
    /**
     * \property indexCount
     * \brief The number of "indexes" in the document, i.e. pages, slides, etc.
     *
     * \default 0
     * \get indexCount() const
     * \notify indexCountChanged()
     */
    Q_PROPERTY(int indexCount READ indexCount NOTIFY indexCountChanged)

    /**
     * \property textEditor
     * \brief The instance of KoTextEditor for the currently selected object in the document, or null
     *
     * \default null
     * \get textEditor() const
     * \notify textEditorChanged()
     */
    Q_PROPERTY(QObject *textEditor READ textEditor NOTIFY textEditorChanged)

public:
    explicit Document(QObject *parent = nullptr);
    ~Document() override;

    QUrl source() const;
    void setSource(const QUrl &value);

    bool readOnly() const;
    void setReadOnly(bool readOnly);

    DocumentType::Type documentType() const;
    DocumentStatus::Status status() const;
    QSize documentSize() const;
    QObject *document() const;
    virtual QObject *part() const;

    /**
     * Getter for property #currentIndex.
     */
    int currentIndex() const;
    /**
     * Setter for property #currentIndex.
     */
    void setCurrentIndex(int newValue);
    /**
     * Getter for property #indexCount.
     */
    int indexCount() const;

    /**
     * \internal
     * These methods are used internally by the components and not exposed
     * to QML.
     * @{
     */
    KoFindBase *finder() const;
    QGraphicsWidget *canvas() const;
    KoCanvasController *canvasController() const;
    KoZoomController *zoomController() const;
    KoDocument *koDocument() const;

    /**
     * \return The url of the link at point or an empty url if there is no link at point.
     */
    virtual QUrl urlAtPoint(const QPoint &point);

    QObject *textEditor();
    // Deselects any text selection present in the document, and deselects all shapes
    // This is highly useful, as it makes navigation prettier.
    Q_INVOKABLE void deselectEverything();

    /**
     * @}
     */
Q_SIGNALS:
    void sourceChanged();
    void statusChanged();
    void documentChanged();
    void readOnlyChanged();
    void documentSizeChanged();
    void documentTypeChanged();
    void textEditorChanged();

    /**
     * Notify signal for property #currentIndex.
     */
    void currentIndexChanged();
    /**
     * Notify signal for property #indexCount
     */
    void indexCountChanged();
    /**
     * \brief Emitted whenever the backend wants to update the view.
     */
    void requestViewUpdate();

private:
    class Private;
    Private *const d;
};

} // Namespace Components
} // Namespace Calligra

Q_DECLARE_METATYPE(Calligra::Components::Document *)

#endif // CALLIGRA_COMPONENTS_DOCUMENT_H
