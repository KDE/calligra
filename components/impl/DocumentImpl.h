/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CALLIGRA_COMPONENTS_DOCUMENTIMPL_H
#define CALLIGRA_COMPONENTS_DOCUMENTIMPL_H

#include "Global.h"
#include <QObject>

class KoDocument;
class QGraphicsWidget;
class KoFindBase;
class KoCanvasController;
class KoZoomController;
class KoDocument;
class KoCanvasBase;

namespace Calligra
{
namespace Components
{

/**
 * \brief Defines an interface for handling specific documents in Document.
 *
 */

class DocumentImpl : public QObject
{
    Q_OBJECT
public:
    explicit DocumentImpl(QObject *parent = nullptr);
    ~DocumentImpl() override;

    virtual bool load(const QUrl &url) = 0;
    virtual int currentIndex() = 0;
    virtual void setCurrentIndex(int newValue) = 0;
    virtual int indexCount() const = 0;
    virtual QUrl urlAtPoint(QPoint point) = 0;

    DocumentType::Type documentType() const;
    KoFindBase *finder() const;
    QGraphicsWidget *canvas() const;
    KoCanvasController *canvasController() const;
    KoZoomController *zoomController() const;
    QSize documentSize() const;
    KoDocument *koDocument() const;
    virtual QObject *part() const = 0;

    void setReadOnly(bool readOnly);

Q_SIGNALS:
    void documentSizeChanged();
    void currentIndexChanged();
    void requestViewUpdate();

protected:
    void setDocumentType(DocumentType::Type type);
    void setKoDocument(KoDocument *document);
    void setCanvas(QGraphicsWidget *newCanvas);
    void setFinder(KoFindBase *newFinder);

    void createAndSetCanvasController(KoCanvasBase *canvas);
    void createAndSetZoomController(KoCanvasBase *canvas);

protected Q_SLOTS:
    void setDocumentSize(const QSize &size);

private:
    class Private;
    Private *const d;
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_DOCUMENTIMPL_H
