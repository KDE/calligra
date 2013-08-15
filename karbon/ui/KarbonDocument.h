/* This file is part of the KDE project
 * Copyright (C) 2001-2002 Lennart Kudling <kudling@kde.org>
 * Copyright (C) 2001-2005,2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2002,2004-2005 Laurent Montel <montel@kde.org>
 * Copyright (C) 2002 Benoit Vautrin <benoit.vautrin@free.fr>
 * Copyright (C) 2004-2005,2007 David Faure <faure@kde.org>
 * Copyright (C) 2004,2006 Peter Simonsson <psn@linux.se>
 * Copyright (C) 2004-2005 Fredrik Edemar <f_edemar@linux.se>
 * Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2006 Inge Wallin <inge@lysator.liu.se>
 * Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
 * Copyright (C) 2006 Casper Boemann <cbr@boemann.dk>
 * Copyright (C) 2006-2007 Thorsten Zachmann <t.zachmann@zagge.de>
 * Copyright (C) 2012 Yue Liu <yue.liu@mail.com>
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
 */

#ifndef KARBON_DOCUMENT_H
#define KARBON_DOCUMENT_H

#include <QString>
#include <QMap>
#include <QRectF>

#include <KoShapeBasedDocumentBase.h>

#include <KoDocument.h>
#include <KoUnit.h>
#include <KoShapeLoadingContext.h>
#include <KoGenStyle.h>
#include <KoXmlReader.h>

#include <karbonui_export.h>

class QRect;
class KarbonPart;
class KarbonCanvas;
class KoDataCenterBase;
class KoShape;
class KoShapeSavingContext;
class KoShapeLayer;
class KoImageCollection;
class KoStore;

#define KARBON_MIME_TYPE "application/vnd.oasis.opendocument.graphics"

/**
 * Keeps track of visual per document properties.
 * It loads initial settings and applies them to the document and its views.
 */
class KARBONUI_EXPORT KarbonDocument : public KoDocument, public KoShapeBasedDocumentBase
{
    Q_OBJECT
public:
    explicit KarbonDocument(KarbonPart *part);
    virtual ~KarbonDocument();

    /// reimplemented form KoDocument
    virtual void paintContent(QPainter& painter, const QRect& rect);
    /// reimplemented form KoDocument
    virtual bool loadXML(const KoXmlDocument& document, KoStore *store);
    /// reimplemented form KoDocument
    virtual bool loadOdf(KoOdfReadStore & odfStore);
    /// reimplemented form KoDocument
    virtual bool completeLoading(KoStore* store);
    /// reimplemented form KoDocument
    virtual bool saveOdf(SavingContext &documentContext);

    /// reimplemented from KoDocument
    virtual QByteArray nativeFormatMimeType() const { return KARBON_MIME_TYPE; }
    /// reimplemented from KoDocument
    virtual QByteArray nativeOasisMimeType() const { return KARBON_MIME_TYPE; }
    /// reimplemented from KoDocument
    virtual QStringList extraNativeMimeTypes() const
    {
        return QStringList() << "application/vnd.oasis.opendocument.graphics"
                             << "application/vnd.oasis.opendocument.graphics-template";
    }

    /// implemented from KoShapeController
    virtual void addShape(KoShape* shape);
    /// implemented from KoShapeController
    virtual void removeShape(KoShape* shape);

    /// Returns if status bar is shown
    bool showStatusBar() const;
    /// Shows/hides status bar
    void setShowStatusBar(bool b);
    /// update attached view(s) on the current doc settings
    /// at this time only the status bar is handled
    void reorganizeGUI();

    /// Returns maximum number of recent files
    uint maxRecentFiles() const;

    /// Sets page layout of the document
    virtual void setPageLayout(const KoPageLayout& layout);

    bool mergeNativeFormat(const QString & file);

    // merged from original KarbonDocument

    /**
     * Checks if specified layer can be raised.
     *
     * A layer can be raised if there is more than one layer and the specified layer
     * is not already at the top.
     *
     * @param layer the layer to check
     * @return true if layer can be raised, else false
     */
    bool canRaiseLayer(KoShapeLayer* layer);

    /**
     * Checks if specified layer can be lowered.
     *
     * A layer can be lowered if there is more than one layer and the specified layer
     * is not already at the bottom.
     *
     * @param layer the layer to check
     * @return true if layer can be lowered, else false
     */
    bool canLowerLayer(KoShapeLayer* layer);

    /**
     * Raises the layer.
     *
     * @param layer the layer to raise
     */
    void raiseLayer(KoShapeLayer* layer);

    /**
     * Lowers the layer.
     *
     * @param layer the layer to lower
     */
    void lowerLayer(KoShapeLayer* layer);

    /**
     * Returns the position of the specified layer.
     *
     * @param layer the layer to retrieve the position for
     * @return the layer position
     */
    int layerPos(KoShapeLayer* layer);

    /**
     * Inserts a new layer.
     *
     * The layer is appended at the end, on top of all other layers, and is activated.
     *
     * @param layer the layer to insert
     */
    void insertLayer(KoShapeLayer* layer);

    /**
     * Removes the layer.
     *
     * If there is no layer left, a new layer is created, inserted and activated.
     *
     * @param layer the layer to remove
     */
    void removeLayer(KoShapeLayer* layer);

    /**
     * Returns the list of layers.
     * The layer list provides a hierarchical view/access of the document data.
     * All the documents shapes are children of a shape container, where a layer
     * resembles a root container which can contain other containers in an
     * arbitrary nesting depth.
     */
    const QList<KoShapeLayer*> layers() const;

    /**
     * Returns the list of all shapes of the document.
     * This list provides a flat view/access to all the documents shapes.
     * For an hierarchical view/access one should retrieve the documents
     * layers with layers().
     */
    const QList<KoShape*> shapes() const;

    void saveOasis(KoShapeSavingContext & context) const;
    bool loadOasis(const KoXmlElement &element, KoShapeLoadingContext &context);
    void loadOdfStyles(KoShapeLoadingContext & context);
    void saveOdfStyles(KoShapeSavingContext & context);

    /**
    * Adds an object to the document.
    *
    * @param shape the object to append
    */
    void add(KoShape* shape);

    /**
    * Removes an object from the document.
    *
    * @param shape the object to append
    */
    void remove(KoShape* shape);

    /// Returns the united bounding rectangle of the documents content and the document page
    QRectF boundingRect() const;

    /// Returns the bounding rectangle of the documents content
    QRectF contentRect() const;

    /// Returns the documents page size
    QSizeF pageSize() const;

    /// Sets given page size to all attached views/canvases
    void setPageSize(const QSizeF &pageSize);

    /// Sets the documents page size
    //void setDocumentPageSize(QSizeF pageSize);

    /// Returns the documents image collection
    KoImageCollection * imageCollection();

    /// Returns the documents data centers
    QMap<QString, KoDataCenterBase*> dataCenterMap() const;

    /// Sets the data centers to be used by this document
    void useExternalDataCenterMap(QMap<QString, KoDataCenterBase*> dataCenters);

    void addToDataCenterMap(const QString &key, KoDataCenterBase* dataCenter);

public slots:
    void slotDocumentRestored();

signals:
    void shapeCountChanged();
    void applyCanvasConfiguration(KarbonCanvas *canvas);

protected:

    /// Loads settings like grid and guide lines from given xml document
    void loadOasisSettings(const KoXmlDocument & settingsDoc);
    /// Saves settings like grid and guide lines to store
    void saveOasisSettings(KoStore * store); 

    /// Reads settings from config file
    void initConfig();


private:
    class Private;
    Private * const d;
};

#endif // KARBON_DOCUMENT_H

