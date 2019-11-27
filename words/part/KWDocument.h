/* This file is part of the KDE project
 * Copyright (C) 2002-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 * Copyright (C) 2008 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
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
#include "words_export.h"

#include <KoDocument.h>
#include <KoShapeManager.h>
#include <KoShapeBasedDocumentBase.h>
#include <KoXmlReader.h>

#include <QObject>
#include <QPainter>
#include <QRect>
#include <QPointer>

class KWView;
class KWPage;
class KWFrameSet;
class KWFrame;
class KoInlineTextObjectManager;
class KoTextRangeManager;
class KoShapeConfigFactoryBase;
class KoUpdater;
class KoShapeAnchor;
class KoShapeController;
class KoPart;
class KoAnnotationLayoutManager;
class KoDocumentInfoDlg;

#define WORDS_MIME_TYPE "application/vnd.oasis.opendocument.text"

/**
 * The class that represents a Words document containing content and settings.
 */
class WORDS_EXPORT KWDocument : public KoDocument, public KoShapeBasedDocumentBase
{
    Q_OBJECT
public:
    /**
     * Constructor, normally called by the KWFactory::createPartObject()
     */
    explicit KWDocument(KoPart *part);
    ~KWDocument() override;

    // KoShapeBasedDocumentBase interface
    /// reimplemented from KoShapeBasedDocumentBase
    void addShape(KoShape *shape) override;
    /// reimplemented from KoShapeBasedDocumentBase
    void removeShape(KoShape *shape) override;
    // reimplemented from KoShapeBasedDocumentBase
    void shapesRemoved(const QList<KoShape*> &shapes, KUndo2Command *command) override;

    // KoDocument interface
    /// reimplemented from KoDocument
    QPixmap generatePreview(const QSize& size) override;
    /// reimplemented from KoDocument
    void paintContent(QPainter&, const QRect&) override;
    /// reimplemented from KoDocument
    bool loadXML(const KoXmlDocument &doc, KoStore *store) override;
    /// reimplemented from KoDocumentBase
    bool loadOdf(KoOdfReadStore &odfStore) override;
    /// reimplemented from KoDocumentBase
    bool saveOdf(SavingContext &documentContext) override;
    /// reimplemented from KoDocument
    int pageCount() const override {
        return pageManager()->pageCount();
    }
    /// reimplemented from KoDocument
    QByteArray nativeFormatMimeType() const override { return WORDS_MIME_TYPE; }
    /// reimplemented from KoDocument
    QByteArray nativeOasisMimeType() const override { return WORDS_MIME_TYPE; }
    /// reimplemented from KoDocument
    QStringList extraNativeMimeTypes() const override
    {
        return QStringList() << "application/vnd.oasis.opendocument.text-master"
                             << "application/vnd.oasis.opendocument.text-template";
    }


    bool isMasterDocument() const;
    void setIsMasterDocument(bool isMasterDocument);

    // others
    KoAnnotationLayoutManager *annotationLayoutManager() const {
        return m_annotationManager;
    }
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
     * Return the frameLayout used in this document.
     */
    Q_SCRIPTABLE KWFrameLayout *frameLayout() {
        return &m_frameLayout;
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

    /// return the textRangeManager for this document.
    KoTextRangeManager *textRangeManager() const;

    KWApplicationConfig &config() {
        return m_config;
    }
    const KWApplicationConfig &config() const {
        return m_config;
    }

    /// This emits the pageSetupChanged signal which will call KWViewMode::updatePageCache.
    void firePageSetupChanged();

    // reimplemented slot from KoDocument
    void initEmpty() override;

    bool layoutFinishedAtleastOnce() const { return m_mainFramesetEverFinished; }

    /// request a relayout of auto-generated frames on all pages of this argument style.
    void updatePagesForStyle(const KWPageStyle &style);

    /// find the textshape closest to the given shape or return 0
    KoShape *findTargetTextShape(KoShape *shape) const;

    KoShapeAnchor *anchorOfShape(KoShape *shape) const;

    KWFrame *frameOfShape(KoShape *shape) const;

    /// returns the document's shapeController. This controller should only be used for deleting shapes.
    //TODO: refactor the shapeController so it can be completely per document maybe? Then it can be added to the resourceManager
    KoShapeController *shapeController() const { return m_shapeController; }

    KoDocumentInfoDlg* createDocumentInfoDialog(QWidget *parent, KoDocumentInfo *docInfo) const override;

public Q_SLOTS:
    /**
     * Relayout the pages or frames within the framesets.
     * @param framesets The framesets that should be relayouted. If no framesets are
     * provided (empty list) then all framesets and therefore all pages are relayouted.
     */
    void relayout(QList<KWFrameSet*> framesets = QList<KWFrameSet*>());
    /**
     * Register a frameset.
     * @param frameset The frameset that should be registered. Future operations like
     * for example @a relayout() operate on all registered framesets.
     */
    void addFrameSet(KWFrameSet *frameset);
    /**
     * Remove frameset from the document stopping it from being saved or displayed.
     * Note that the document is normally the one that deletes framesets when the
     * document is closed, after removing it the
     * caller will have the responsibility to delete it when its no longer of use.
     * @param fs the frameset that should be removed from the doc
     * \sa addFrameSet()
     */
    void removeFrameSet(KWFrameSet *fs);

Q_SIGNALS:
    /// signal emitted when a page has been added
    void pageSetupChanged();

    /// emitted whenever a shape is added.
    void shapeAdded(KoShape *, KoShapeManager::Repaint);

    /// emitted whenever a shape is removed
    void shapeRemoved(KoShape *);

    /// emitted wheneve a resources needs to be set on the canvasResourceManager
    void resourceChanged(int key, const QVariant &value);

private Q_SLOTS:
    /// Shape maintenance on already registered framesets
    void addSequencedShape(KoShape *shape);
    void removeSequencedShape(KoShape *shape);
    /// Called after the constructor figures out there is an install problem.
    void mainTextFrameSetLayoutDone();

    void layoutProgressChanged(int percent);
    void layoutFinished();

protected:
    /// reimplemented from KoDocument
    void setupOpenFileSubProgress() override;

private:
    friend class KWDLoader;
    friend class KWOdfLoader;
    friend class KWPagePropertiesCommand;
    QString renameFrameSet(const QString &prefix , const QString &base);
    /**
     * post process loading after either oasis or oldxml loading finished
     */
    void endOfLoading();
    /**
     * Called before loading
     * It's important to clear out anything that might be in the document already,
     * for things like using DBUS to load multiple documents into the same KWDocument,
     * or "reload" when words is embedded into konqueror.
     */
    void clear();

    /**
     * emits pageSetupChanged
     */
    void saveConfig();

private:
    bool m_isMasterDocument;
    QList<KWFrameSet*> m_frameSets;
    KWPageManager m_pageManager;
    KWFrameLayout m_frameLayout;
    KWApplicationConfig m_config;
    bool m_mainFramesetEverFinished;
    QList<KoShapeConfigFactoryBase *> m_panelFactories;
    QPointer<KoUpdater> m_layoutProgressUpdater;
    KoShapeController *m_shapeController;
    QList<KoShape*> m_loadedAnnotationShapes;
    KoAnnotationLayoutManager *m_annotationManager;
};

#endif
