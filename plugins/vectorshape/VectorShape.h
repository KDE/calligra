/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2009-2011 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef VECTORSHAPE_H
#define VECTORSHAPE_H

// Qt
#include <QByteArray>
#include <QCache>
#include <QMutex>
#include <QObject>
#include <QRunnable>
#include <QSize>

// Calligra
#include <KoFrameShape.h>
#include <KoShape.h>

#define DEBUG_VECTORSHAPE 0

class QPainter;
class VectorShape;

#define VectorShape_SHAPEID "VectorShapeID"

class VectorShape : public QObject, public KoShape, public KoFrameShape
{
    Q_OBJECT
public:
    // Type of vector file. Add here when we get support for more.
    enum VectorType {
        VectorTypeNone, // Uninitialized
        VectorTypeWmf, // Windows MetaFile
        VectorTypeEmf, // Extended MetaFile
        VectorTypeSvm, // StarView Metafile
        VectorTypeSvg // Scalable Vector Graphics
        // ... more here later
    };

    VectorShape();
    ~VectorShape() override;

    // reimplemented methods.

    /// reimplemented from KoShape
    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;
    /// reimplemented from KoShape
    void saveOdf(KoShapeSavingContext &context) const override;
    /// reimplemented from KoShape
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;
    /// Load the real contents of the frame shape.  reimplemented  from KoFrameShape
    bool loadOdfFrameElement(const KoXmlElement &frameElement, KoShapeLoadingContext &context) override;
    /// reimplemented from KoShape
    void waitUntilReady(const KoViewConverter &converter, bool asynchronous = true) const override;

    // Methods specific to the vector shape.
    QByteArray compressedContents() const;
    VectorType vectorType() const;
    void setCompressedContents(const QByteArray &newContents, VectorType vectorType);

    static VectorShape::VectorType vectorType(const QByteArray &contents);

private Q_SLOTS:
    void renderFinished(const QSize &boundingSize, QImage *image);

private:
    static bool isWmf(const QByteArray &bytes);
    static bool isEmf(const QByteArray &bytes);
    static bool isSvm(const QByteArray &bytes);
    static bool isSvg(const QByteArray &bytes);

    // Member variables
    mutable VectorType m_type;
    mutable QByteArray m_contents;
    mutable bool m_isRendering;
    mutable QMutex m_mutex;
    QCache<int, QImage> m_cache;

    QImage *render(const KoViewConverter &converter, bool asynchronous, bool useCache) const;
};

class RenderThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    RenderThread(const QByteArray &contents, VectorShape::VectorType type, const QSizeF &size, const QSize &boundingSize, qreal zoomX, qreal zoomY);
    ~RenderThread() override;
    void run() override;
Q_SIGNALS:
    void finished(const QSize &boundingSize, QImage *image);

private:
    void draw(QPainter &painter);
    void drawNull(QPainter &painter) const;
    void drawWmf(QPainter &painter) const;
    void drawEmf(QPainter &painter) const;
    void drawSvm(QPainter &painter) const;
    void drawSvg(QPainter &painter) const;

private:
    const QByteArray m_contents;
    VectorShape::VectorType m_type;
    QSizeF m_size;
    QSize m_boundingSize;
    qreal m_zoomX, m_zoomY;
};

#endif
