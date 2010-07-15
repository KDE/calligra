/* This file is part of the KDE project
 * Copyright (C) 2002-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 * Copyright (C) 2008 Sebastian Sauer <mail@dipe.org>
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

#include "KWPageManager.h"
#include "KWApplicationConfig.h"
#include "frames/KWFrameLayout.h"
#include "kword_export.h"

#include <KoDocument.h>
#include <KoShapeControllerBase.h>
#include <KoXmlReader.h>

#include <QObject>
#include <QPainter>
#include <QRect>

class KWView;
class KWPage;
class KWFrameSet;
class MagicCurtain;

class KoInlineTextObjectManager;

class KLocalizedString;
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
    explicit KWDocument(QWidget *parentWidget = 0, QObject* parent = 0, bool singleViewMode = false);
    ~KWDocument();

    // KoShapeControllerBase interface
    /// reimplemented from KoShapeControllerBase
    void addShape(KoShape *shape);
    /// reimplemented from KoShapeControllerBase
    void removeShape(KoShape *shape);


    // KoDocument interface
    /// reimplemented from KoDocument
    virtual void paintContent(QPainter&, const QRect&);
    /// reimplemented from KoDocument
    virtual bool loadXML(const KoXmlDocument &doc, KoStore *store);
    /// reimplemented from KoOdfDocument
    virtual bool loadOdf(KoOdfReadStore &odfStore);
    /// reimplemented from KoOdfDocument
    virtual bool saveOdf(SavingContext &documentContext);
    /// reimplemented from KoDocument
    KoView* createViewInstance(QWidget*);
    /// reimplemented from KoDocument
    virtual int pageCount() const {
        return pageManager()->pageCount();
    }

    // others
    /**
     * Return the pageManager used in this document.
     */
    const KWPageManager *pageManager() const {
        return &m_pageManager;
    }
    /**
     * Return the pageManager used in this document.
     */
    Q_SCRIPTABLE KWPageManager *pageManager() {
        return &m_pageManager;
    }

    /**
     * Insert a new page after another,
     * creating followup frames (but not headers/footers),
     * @param afterPageNum the page is inserted after the one specified here
     * If afterPageNum is 0, a page is inserted before page 1.
     * In all cases, the new page will have the number afterPageNum+1.
     * Use appendPage in WP mode, insertPage in DTP mode.
     * @param masterPageName the name of the master page to use for this new page.
     */
    KWPage insertPage(int afterPageNum, const QString &masterPageName = QString());
    /**
     * Append a new page, creating followup frames (but not headers/footers),
     * and return the page number.
     * @param masterPageName the name of the master page to use for this new page.
     */
    KWPage appendPage(const QString &masterPageName = QString());
    /**
     * remove a page from the document.
     * @param pageNumber the pageNumber that should be removed.
     */
    void removePage(int pageNumber);

    /// return the amount of framesets this document holds
    int frameSetCount() const {
        return m_frameSets.count();
    }
    /// return a list of all the framesets this document holds
    const QList<KWFrameSet*> &frameSets() const {
        return m_frameSets;
    }
    /// return a frameset, or null, by name. @see KWFrameSet::name()
    KWFrameSet *frameSetByName(const QString &name);
    /// return a suggestion for a copy frameset that does not collide with known ones.
    QString suggestFrameSetNameForCopy(const QString& base);
    /// return a suggestion for a new frameset name that does not collide with known ones.
    QString uniqueFrameSetName(const QString &suggestion);
    /// return the main text frameset of the document
    KWTextFrameSet *mainFrameSet() const;

    /// return the inlineTextObjectManager for this document.
    KoInlineTextObjectManager *inlineTextObjectManager() const;

    /// reimplemented from super
    QList<KoDocument::CustomDocumentWidgetItem> createCustomDocumentWidgets(QWidget *parent);

    KWApplicationConfig &config() {
        return m_config;
    }
    const KWApplicationConfig &config() const {
        return m_config;
    }

#ifndef NDEBUG
    /// Use a log of kDebug calls to print out the internal state of the document and its members
    void printDebug();
#endif

    void firePageSetupChanged();

    // reimplemented slot from KoDocument
    virtual void initEmpty();
    // reimplemented slot from KoDocument
    virtual QStringList extraNativeMimeTypes(ImportExportType importExportType) const;

    bool layoutFinishedAtleastOnce() const { return m_mainFramesetEverFinished; }

public slots:
    /// Relayout the pages
    void relayout();
    /// Register new frameset
    void addFrameSet(KWFrameSet *f);
    /**
     * Remove frameset from the document stopping it from being saved or displayed.
     * Note that the document is normally the one that deletes framesets when the
     * document is closed, after removing it the
     * caller will have the responsibility to delete it when its no longer of use.
     * @param fs the frameset that should be removed from the doc
     * \sa addFrameSet()
     */
    void removeFrameSet(KWFrameSet *fs);

signals:
    /// signal emitted when a page has been added
    void pageSetupChanged();

private slots:
    /// Frame maintenance on already registered framesets
    void addFrame(KWFrame *frame);
    void removeFrame(KWFrame *frame);
    void requestMoreSpace(KWTextFrameSet *fs);
    void removeFrameFromViews(KWFrame*);
    void updateHeaderFooter(KWTextFrameSet*);

    /// Called after the constructor figures out there is an install problem.
    void showErrorAndDie();
    void mainTextFrameSetLayoutDone();

private:
    friend class PageProcessingQueue;
    friend class KWDLoader;
    friend class KWOdfLoader;
    friend class KWPagePropertiesCommand;
    QString renameFrameSet(const QString &prefix , const QString &base);
    /// post process loading after either oasis or oldxml loading finished
    void endOfLoading();
    /**
     * Called before loading
     * It's important to clear out anything that might be in the document already,
     * for things like using DBUS to load multiple documents into the same KWDocument,
     * or "reload" when kword is embedded into konqueror.
     */
    void clear();

    void showStartUpWidget(KoMainWindow *parent, bool alwaysShow = false);
    /// emits pageSetupChanged

    void saveConfig();

private:
    QList<KWFrameSet*> m_frameSets;
    QString m_viewMode;

    KWPageManager m_pageManager;
    KWFrameLayout m_frameLayout;
    KWApplicationConfig m_config;

    MagicCurtain *m_magicCurtain; ///< all things we don't want to show are behind this one
    bool m_mainFramesetEverFinished;
};

#endif
