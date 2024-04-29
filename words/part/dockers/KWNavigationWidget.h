/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Denis Kupluakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWNAVIGATIONWIDGET_H
#define KWNAVIGATIONWIDGET_H

#include <QHBoxLayout>
#include <QStandardItem>
#include <QTimer>
#include <QTreeView>
#include <QWidget>

// FIXME: Don't cross include
// #include "KWNavigationDocker.h"
#include "StatisticsPreferencesPopup.h"
#include <KWCanvas.h>
#include <KoTextDocumentLayout.h>

class KoCanvasResourceManager;
class KWDocument;
class StatisticsPreferencesPopup;

/** KWNavigationWidget shows document structure by parsing
 *  headers levels. You can click on the header to momentarily
 *  get to another part of the document.
 */

class KWNavigationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KWNavigationWidget(QWidget *parent = nullptr);
    ~KWNavigationWidget() override;

    friend class KWNavigationDocker;

    void setCanvas(KWCanvas *canvas);

    void unsetCanvas();

private Q_SLOTS:
    void navigationClicked(const QModelIndex &idx);

public Q_SLOTS:
    void updateData();

private:
    void initUi();
    void initLayout();

    void updateDataUi();

private:
    QTreeView *m_treeView;
    QStandardItemModel *m_model;

    KoCanvasResourceManager *m_resourceManager;
    KWDocument *m_document;
    KWCanvas *m_canvas;
    KoTextDocumentLayout *m_layout;
    QTimer *m_updateTimer;
};

#endif // KWNAVIGATIONWIDGET_H
