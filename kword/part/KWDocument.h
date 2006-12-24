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
#include "KWMultiViewCommandHistory.h"
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
class KoStyleManager;
class KoShapeAddRemoveData;

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

    // KoShapeControllerBase interface
    /// reimplemented from KoShapeControllerBase
    void addShape (KoShape *shape, KoShapeAddRemoveData * addRemoveData);
    /// reimplemented from KoShapeControllerBase
    void removeShape (KoShape *shape, KoShapeAddRemoveData * addRemoveData);


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
    KoView* createViewInstance(QWidget*);


    // others
    /**
     * Return the pageManager used in this document.
     */
    const KWPageManager *pageManager() const { return &m_pageManager; }
    /**
     * Return the settings for page-layouts used in this document.
     */
    const KWPageSettings& pageSettings() const { return m_pageSettings; }
    /**
     * Return (a copy of) the settings for page-layouts used in this document.
     */
    KWPageSettings& pageSettings() { return m_pageSettings; }
    /**
     * Set new pageSettings for this document, triggering a layout change.
     */
    void setPageSettings(const KWPageSettings &newPageSettings);

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

    void removePage(int pageNumber);

    /**
     * Remove frameset from the document stopping it from being saved or displayed.
     * Note that the document is normally the one that deletes framesets when the
     * document is closed, after removing it the
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

    void setStartPage(int pageNumber);

    void setDefaultPageLayout(const KoPageLayout &layout);

    /// return the amount of framesets this document holds
    int frameSetCount() const { return m_frameSets.count(); }
    /// return a list of all the framesets this document holds
    const QList<KWFrameSet*> &frameSets() const { return m_frameSets; }
    /// return a frameset, or null, by name. @see KWFrameSet::name()
    KWFrameSet *frameSetByName( const QString & name );
    /// return a suggestion for a copy frameset that does not collide with known ones.
    QString suggestFrameSetNameForCopy( const QString& base );
    /// return a suggestion for a new frameset name that does not collide with known ones.
    QString uniqueFrameSetName( const QString& suggestion );

    /**
     * Return the style manager for this document.
     * The text (paragraph/character) styles are managed per document and are all
     * stored in this construct.
     */
    KoStyleManager *styleManager() { return m_styleManager; }
    /**
     * Return the style manager for this document.
     * The text (paragraph/character) styles are managed per document and are all
     * stored in this construct.
     */
    const KoStyleManager *styleManager() const { return m_styleManager; }

#ifndef NDEBUG
    /// Use a log of kDebug calls to print out the internal state of the document and its members
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
    /// Frame maintenance on already registered framesets
    void addFrame( KWFrame *frame );
    void requestMoreSpace(KWTextFrameSet *fs);

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
    bool m_hasTOC;
    double m_defaultColumnSpacing;
    double m_tabStop;   ///< pt distance for auto-tabstops

    int m_zoom; /// < zoom level in percent
    KoZoomMode::Mode m_zoomMode;
    QList<KWFrameSet*> m_frameSets;
    QString m_viewMode;

    KWPageManager m_pageManager;
    KWPageSettings m_pageSettings;
    KWFrameLayout m_frameLayout;

    KoStyleManager *m_styleManager;
    KWMultiViewCommandHistory m_commandHistory;
};

/// \internal
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
