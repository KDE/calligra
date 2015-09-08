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

#ifndef CAUOUTLINERWIDGET_H
#define CAUOUTLINERWIDGET_H

#include <KWCanvas.h>
#include <KoTextDocumentLayout.h>

#include <QWidget>
#include <QTreeView>
#include <QPushButton>
#include <QTimer>

/**
 * This widget shows the current structure of book
 * with additional information in columns and provide
 * ability to rearrange sections, change their metadata
 * and etc.
 */
class CAuOutlinerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CAuOutlinerWidget(QWidget *parent = 0);
    virtual ~CAuOutlinerWidget();

    friend class CAuOutlinerDocker;

    void setCanvas(KWCanvas* canvas);
    void unsetCanvas();

private Q_SLOTS:
    void updateData();
    void sectionClicked(QModelIndex idx);
    void updateSelection();
    void sectionEditClicked();

private:
    KWCanvas *m_canvas;
    KoTextDocumentLayout *m_layout;
    QTextDocument *m_document;

    QTimer *m_updateTimer;

    QTreeView *m_sectionTree;
    QPushButton *m_editButton;

    void initUi();
    void initLayout();
};

#endif //CAUOUTLINERWIDGET_H
