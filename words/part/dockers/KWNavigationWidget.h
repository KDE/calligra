/* This file is part of the KDE project
 * Copyright (C) 2014 Denis Kupluakov <dener.kup@gmail.com>
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

#ifndef KWNAVIGATIONWIDGET_H
#define KWNAVIGATIONWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QStandardItem>
#include <QTreeView>
#include <QTimer>

// FIXME: Don't cross include
//#include "KWNavigationDocker.h"
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
    explicit KWNavigationWidget(QWidget *parent = 0);
    ~KWNavigationWidget() override;

    friend class KWNavigationDocker;

    void setCanvas(KWCanvas* canvas);

    void unsetCanvas();

private Q_SLOTS:
    void navigationClicked(QModelIndex idx);

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
