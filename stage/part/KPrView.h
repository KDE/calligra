/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2009-2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRVIEW_H
#define KPRVIEW_H

#include "stage_export.h"
#include <KoPAView.h>
#include <QObject>

class KPrDocument;
class KPrPart;

#ifdef WITH_QTDBUS
class KPrViewAdaptor;
#endif

class KPrViewModeNotes;
class KPrViewModeSlidesSorter;
class KPrViewModePresentation;
class KActionMenu;
class KoPAPageBase;

class STAGE_EXPORT KPrView : public KoPAView
{
    Q_OBJECT
    friend class KPrConfigureSlideShowDialog;

public:
    explicit KPrView(KPrPart *part, KPrDocument *document, QWidget *parent = nullptr);
    ~KPrView() override;

    using KoPAViewBase::viewConverter;
    KoViewConverter *viewConverter(KoPACanvasBase *canvas) override;

    /**
     * Get the document object the view was initialised with
     */
    KPrDocument *kprDocument() const;

#ifdef WITH_QTDBUS
    /**
     * Get the view's dbus adaptor
     */
    virtual KPrViewAdaptor *dbusObject() const;
#endif

    /**
     * Get the presentation view mode
     */
    Q_INVOKABLE KPrViewModePresentation *presentationMode() const;

    /**
     * Get the slides sorter view mode
     */
    KPrViewModeSlidesSorter *slidesSorter() const;

    /**
     * Find whether the presentation view mode is active
     */
    bool isPresentationRunning() const;

    /**
     * Load zoom configuration
     */
    void initZoomConfig();

    /**
     * Restore zoom configuration
     */
    void restoreZoomConfig();

    /**
     * Save zoom value
     */
    void saveZoomConfig(KoZoomMode::Mode zoomMode, int zoom);

    /**
     * Setter of the zoom
     *
     * @param zoomMode mode
     * @param zoom zoom percent
     */
    void setZoom(KoZoomMode::Mode zoomMode, int zoom);

    /**
     * Return the last zoom stored
     *
     * @return the last zoom stored
     */
    int zoom();

    /**
     * Return the last zoom mode stored
     *
     * @return the last zoom mode stored
     */
    KoZoomMode::Mode zoomMode();

public Q_SLOTS:
    /**
     * Activate the presentation view mode
     */
    void startPresentation();

    /**
     * Activate the presentation view mode from the first slide
     */
    void startPresentationFromBeginning();

    /**
     * Stop the presentation and activate the previously active view mode.
     */
    void stopPresentation();

    void showNormal();

    /** Change current view mode using a given index */
    void changeViewByIndex(int index);

    void replaceActivePage(KoPAPageBase *page, KoPAPageBase *newActivePage);

    /// called if the zoom changed
    void zoomChanged(KoZoomMode::Mode mode, qreal zoom);

protected:
    void initGUI();
    void initActions();
    bool event(QEvent *event) override;

protected Q_SLOTS:
    void createAnimation();
    void showNotes();
    void showSlidesSorter();
    void editCustomSlideShows();
    void configureSlideShow();
    void configurePresenterView();
    void exportToHtml();
    void insertPictures();
    void drawOnPresentation();
    void highlightPresentation();
    void blackPresentation();
    void showStatusBar(bool toggled);

private:
    KPrPart *m_part;
    QAction *m_actionStopPresentation;
    QAction *m_actionCreateAnimation;
    QAction *m_actionViewModeNormal;
    QAction *m_actionViewModeNotes;
    QAction *m_actionViewModeSlidesSorter;
    QAction *m_actionEditCustomSlideShows;
    QAction *m_actionExportHtml;
    QAction *m_actionInsertPictures;
    QAction *m_actionDrawOnPresentation;
    QAction *m_actionHighlightPresentation;
    QAction *m_actionBlackPresentation;

    KPrViewModePresentation *m_presentationMode;
    KoPAViewMode *m_normalMode;
    KPrViewModeNotes *m_notesMode;
    KPrViewModeSlidesSorter *m_slidesSorterMode;

#ifdef WITH_QTDBUS
    KPrViewAdaptor *m_dbus;
#endif

    int m_zoom;
    KoZoomMode::Mode m_zoomMode;

    KoPrintJob *createPdfPrintJob() override;
};

#endif /* KPRVIEW_H */
