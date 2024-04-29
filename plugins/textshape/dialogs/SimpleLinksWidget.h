/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2001 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2005-2007, 2009, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010-2011 Boudewijn Rempt <boud@kogmbh.com>
 * SPDX-FileCopyrightText: 2013 Aman Madaan <madaan.amanmadaan@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SIMPLELINKSWIDGET_H
#define SIMPLELINKSWIDGET_H

#include "FormattingButton.h"
#include <KoTextEditor.h>
#include <QWidget>
#include <ui_SimpleLinksWidget.h>

class ReferencesTool;

class SimpleLinksWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleLinksWidget(ReferencesTool *tool, QWidget *parent = nullptr);
    ~SimpleLinksWidget() override;

Q_SIGNALS:
    void doneWithFocus();

public Q_SLOTS:
    void preparePopUpMenu();

private Q_SLOTS:
    void manageBookmarks();

private:
    Ui::SimpleLinksWidget widget;
    ReferencesTool *m_referenceTool;
};

#endif
