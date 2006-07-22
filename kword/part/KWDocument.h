/* This file is part of the KDE project
 * Copyright (C) 2002-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
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

#ifndef KWDOCUMENT_H
#define KWDOCUMENT_H

#include "KWPageSettings.h"
#include "KWPageManager.h"
#include "frame/KWFrameLayout.h"

#include <KoDocument.h>
#include <KoShapeControllerBase.h>
#include <KoZoomMode.h>

#include <QObject>
#include <QPainter>
#include <QRect>

class KWView;
class KWPageManager;
class KWPage;
class KWFrameSet;

class KoOasisStyles;

class KLocalizedString;
class QDomDocument;
class QIODevice;

/**
 * The class that represents a KWord document containing content and settings.
 */
class KWORD_EXPORT KWDocument : public KoDocument, public KoShapeControllerBase
{
    Q_OBJECT
public:
    /**
     * Constructor, normally called by the KWFactory::createPartObject()
     */
    KWDocument( QWidget *parentWidget = 0, QObject* parent = 0, bool singleViewMode = false );
    ~KWDocument();

    /// return the grid width
    double gridX() const { return m_gridX; }
    /// return the grid height
    double gridY() const { return m_gridY; }
    /**
     * Set the size grid to a new value
     * @param x the width of a grid unit
     * @param y the height of a grid unit
     * @see snapToGrid()
     * @see gridX()
     * @see gridY()
     */
    void setGrid(double x, double y) { m_gridX = x; m_gridY = y; }

    /**
     * return if snap to grid is enabled.
     * @return if snap to grid is enabled.
     * @see setGrid()
     */
    bool snapToGrid() { return m_snapToGrid; }
    /**
     * Set the snap to grid, forcing objects being moved/scaled to the grid.
     * @param on when true, all moving and scaling will be on the grid.
     * @see setGrid()
     */
    void setSnapToGrid(bool on) { m_snapToGrid = on; }

    // KoShapeControllerBase interface
    /// reimplemented from KoShapeControllerBase
    void addShape (KoShape *shape);
    /// reimplemented from KoShapeControllerBase
    void removeShape (KoShape *shape);


    // KoDocument interface
    /// reimplemented from KoDocument
    void paintContent(QPainter&, const QRect&, bool, double, double);
    /// reimplemented from KoDocument
    bool loadXML(QIODevice*, const QDomDocument&);
    /// reimplemented from KoDocument
    bool loadOasis(const QDomDocument&, KoOasisStyles&, const QDomDocument&, KoStore*);
    /// reimplemented from KoDocument
    bool saveOasis(KoStore*, KoXmlWriter*);
    /// reimplemented from KoDocument
    KoView* createViewInstance(QWidget*, const char*);


    // others
    /**
     * Return the pageManager used in this document.
     */
    const KWPageManager *pageManager() const { return &m_pageManager; }
    /**
     * Return the settings for page-layouts used in this document.
     */
    const KWPageSettings *pageSettings() const { return &m_pageSettings; }

    /**
     * Insert a new page after another,
     * creating followup frames (but not headers/footers),
     * @param afterPageNum the page is inserted after the one specified here
     * If afterPageNum is 0, a page is inserted before page 1.
     * In all cases, the new page will have the number afterPageNum+1.
     * Use appendPage in WP mode, insertPage in DTP mode.
     */
    KWPage* insertPage( int afterPageNum );
    /**
     * Append a new page, creating followup frames (but not headers/footers),
     * and return the page number.
     */
    KWPage* appendPage();

    /**
     * Remove frameset from the document stopping it from being saved or displayed.
     * Note that the document is normally the one that deletes framesets, the
     * caller will have the responsibility to delete it when its no longer of use.
     * @param fs the frameset that should be removed from the doc
     */
    void removeFrameSet( KWFrameSet *fs );

    /**
     * Set the zoom level for new views and for next startups of kword.
     * Documents shown at 100% will show the page on screen at the same amount of
     * metric units as the user set them to be. In other words; the paper and the
     * screen versions should be exactly the same.
     * @param percent the new zoom level to be persisted between sessions.
     */
    void setZoom(int percent) { m_zoom = percent; }
    /**
     * Return the percentage of zoom.
     * @return the percentage of zoom.
     * @see setZoom(int)
     */
    int zoom() const { return m_zoom; }
    /**
     * Set the mode of zooming for new views and to be persisted between sessions.
     * @param mode the new mode
     */
    void setZoomMode(KoZoomMode::Mode mode) { m_zoomMode = mode; }
    /**
     * Return the zoomMode to be used for new views.
     */
    KoZoomMode::Mode zoomMode() const { return m_zoomMode; }

    /**
     * returns the amount of pages in the document.
     * @see startPage() @see lastPage()
     */
    int pageCount() const;
    /**
     * returns the page number of the first page in this document, this is the page number
     * that will be shown on prints and used in the TOC and user-variables.
     * @see pageCount() @see lastPage()
     */
    int startPage() const;
    /**
     * Returns the last page number in this document.
     * With a higher startPage and a constante pagecount this number will also get higher.
     */
    int lastPage() const;

    KWFrame *frameForShape(KoShape *shape) const;

    int frameSetCount() const { return m_frameSets.count(); }
    const QList<KWFrameSet*> &frameSets() const { return m_frameSets; }
    KWFrameSet *frameSetByName( const QString & name );
    QString suggestFrameSetNameForCopy( const QString& base );
    QString uniqueFrameSetName( const QString& suggestion );

#ifndef NDEBUG
    void printDebug();
#endif

public slots:
    /// Register new frameset
    void addFrameSet( KWFrameSet *f );

signals:
    /// signal emitted when a page has been added
    void pageAdded(KWPage *page);
    /// signal emitted when a page has been removed
    void pageRemoved(KWPage *page);

    /// signal emitted when a frameSet has been added
    void frameSetAdded(KWFrameSet*);
    /// signal emitted when a frameSet has been removed
    void frameSetRemoved(KWFrameSet*);

private slots:
    /// Frame maintainance on already registed framesets
    void addFrame( KWFrame *frame );
    void removeFrame( KWFrame *frame );

private:
    friend class PageProcessingQueue;
    friend class KWDLoader;
    QString renameFrameSet( const QString& prefix , const QString& base );
    /// post process loading after either oasis or oldxml loading finished
    void endOfLoading();
    /** Called before loading
     * It's important to clear out anything that might be in the document already,
     * for things like using DCOP to load multiple documents into the same KWDocument,
     * or "reload" when kword is embedded into konqueror.
     */
    void clear();

private:
    bool m_snapToGrid, m_hasTOC;
    double m_gridX, m_gridY;
    double m_defaultColumnSpacing;
    double m_tabStop;   ///< pt distance for auto-tabstops

    int m_zoom; /// < zoom level in percent
    KoZoomMode::Mode m_zoomMode;
    QList<KWFrameSet*> m_frameSets;
    QString m_viewMode;

    KWPageManager m_pageManager;
    KWPageSettings m_pageSettings;
    KWFrameLayout m_frameLayout;
    QMap<KoShape*, KWFrame*> m_frameMap;
};

class PageProcessingQueue : public QObject {
    Q_OBJECT
public:
    PageProcessingQueue(KWDocument *parent);
    void addPage(KWPage *page);

private slots:
    void process();

private:
    QList<KWPage *> m_pages;
    bool m_triggered;
    KWDocument *m_document;
};

#endif
