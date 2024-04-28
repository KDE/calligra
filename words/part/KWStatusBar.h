/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Sebastian Sauer <mail@dipe.org>
 * SPDX-FileCopyrightText: 2008-2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWSTATUSBAR_H
#define KWSTATUSBAR_H

#include <QMap>
#include <QObject>
#include <QPointer>

class QPoint;
class QAction;
class QLabel;
class QStatusBar;

class KSqueezedTextLabel;

class KoCanvasController;
class KoCanvasControllerProxyObject;

class KWView;
class KWStatusBarEditItem;
class KWStatusBarButtonItem;

/**
 * The KWStatusBar class implements an extended statusbar for Words.
 */
class KWStatusBar : public QObject
{
    Q_OBJECT
public:
    /**
     * Destructor.
     */
    ~KWStatusBar() override;

    static void addViewControls(QStatusBar *statusBar, KWView *view);

public Q_SLOTS:
    void setText(const QString &text);

private Q_SLOTS:
    void setModified(bool modified);
    void updatePageCount();
    void gotoPage(int pagenumber = -1);
    void updatePageStyle();
    void showPageStyle();
    void updatePageSize();
    void updateCursorPosition();
    void gotoLine();
    void updateMousePosition(const QPoint &);
    void canvasResourceChanged(int, const QVariant &);
    void updateCurrentTool(KoCanvasController *);
    void createZoomWidget();
    void showPage(bool visible);
    void showPageStyle(bool visible);
    void showPageSize(bool visible);
    void showLineColumn(bool visible);
    void showModified(bool visible);
    void showMouse(bool visible);
    void showZoom(bool visible);
    void removeView(QObject *);

private:
    /**
     * Constructor.
     *
     * \param statusBar The parent statusbar this statusbar is child
     * of. We will embed our own widgets into this statusbar.
     * \param view The Words view instance the statusbar belongs to. Each
     * KWStatusBar instance belongs to exactly one view.
     */
    KWStatusBar(QStatusBar *statusBar, KWView *view);

    void setCurrentView(KWView *view);

    QStatusBar *m_statusbar;
    QPointer<KWView> m_currentView;
    QMap<KWView *, QWidget *> m_zoomWidgets;
    QPointer<KoCanvasControllerProxyObject> m_controller;
    int m_currentPageNumber;
    QAction *m_zoomAction;

    QLabel *m_modifiedLabel;
    KWStatusBarEditItem *m_pageLabel;
    KWStatusBarButtonItem *m_pageStyleLabel;
    QLabel *m_pageSizeLabel;
    KWStatusBarEditItem *m_lineLabel;
    QLabel *m_mousePosLabel;
    KSqueezedTextLabel *m_statusLabel;
    QList<KWView *> m_views;
};

#endif
