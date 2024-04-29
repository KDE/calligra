/* This file is part of the KDE project
 * Copyright ( C ) 2010 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOPAVIEWBASE_H
#define KOPAVIEWBASE_H

#include "KoPAViewMode.h"
#include "KoPageApp.h"

class KoPACanvasBase;
class KoViewConverter;
class KoPAPageBase;
class KoPADocument;
class KoZoomHandler;
class KoZoomController;

#include "kopageapp_export.h"

class KoPAViewProxyObject;

/**
 * Base class defining the View interface the KoPAViewMode class needs.
 */
class KOPAGEAPP_EXPORT KoPAViewBase
{
public:
    // proxy QObject
    KoPAViewProxyObject *proxyObject;

    explicit KoPAViewBase();
    virtual ~KoPAViewBase();

    /// @return the canvas for the application
    virtual KoPACanvasBase *kopaCanvas() const = 0;

    /// @return the document for the application
    virtual KoPADocument *kopaDocument() const = 0;

    /// XXX
    virtual KoViewConverter *viewConverter(KoPACanvasBase *canvas);
    virtual KoViewConverter *viewConverter() const;
    virtual KoZoomController *zoomController() const = 0;

    virtual KoZoomHandler *zoomHandler();
    virtual KoZoomHandler *zoomHandler() const;

    /// Set the active page and updates the UI
    virtual void doUpdateActivePage(KoPAPageBase *page) = 0;

    /// Set page shown in the canvas to @p page
    virtual void setActivePage(KoPAPageBase *page) = 0;

    /// @return Page that is shown in the canvas
    virtual KoPAPageBase *activePage() const = 0;

    /// XXX
    virtual void navigatePage(KoPageApp::PageNavigation pageNavigation) = 0;

    /**
     * @brief Enables/Disables the given actions
     *
     * The actions are of Type KoPAAction
     *
     * @param actions which should be enabled/disabled
     * @param enable new state of the actions
     */
    virtual void setActionEnabled(int actions, bool enable) = 0;

    /// XXX
    virtual void updatePageNavigationActions() = 0;

    /**
     * @brief Set the view mode
     *
     * @param mode the new view mode
     */
    virtual void setViewMode(KoPAViewMode *mode);

    /// @return the active viewMode
    virtual KoPAViewMode *viewMode() const;

    /// Insert a new page after the current one
    virtual void insertPage() = 0;

    /// Paste the page if everything is ok
    virtual void pagePaste() = 0;

    /// XXX
    virtual void editPaste() = 0;

    /// XXX
    virtual void setShowRulers(bool show) = 0;

private:
    class Private;
    Private *const d;
};

/**
 * QObject proxy class for handling signals and slots
 */
class KOPAGEAPP_EXPORT KoPAViewProxyObject : public QObject
{
    Q_OBJECT

public:
    explicit KoPAViewProxyObject(KoPAViewBase *parent);

    void emitActivePageChanged()
    {
        Q_EMIT activePageChanged();
    }

public Q_SLOTS:

    /// Set the active page and updates the UI
    void updateActivePage(KoPAPageBase *page)
    {
        m_view->viewMode()->updateActivePage(page);
    }

    /// Shows/hides the rulers
    void setShowRulers(bool show)
    {
        m_view->setShowRulers(show);
    }

    /// Insert a new page after the current one
    void insertPage()
    {
        m_view->insertPage();
    }

    void editPaste()
    {
        m_view->editPaste();
    }

Q_SIGNALS:

    /// Emitted every time the active page is changed
    void activePageChanged();

private:
    KoPAViewBase *m_view;
};

#endif // KOPAVIEWBASE_H
