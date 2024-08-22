/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2001 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2005-2007, 2009, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010-2011 Boudewijn Rempt <boud@kogmbh.com>
 * SPDX-FileCopyrightText: 2013 Aman Madaan <madaan.amanmadaan@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SimpleLinksWidget.h"

#include "ManageBookmarkDialog.h"
#include "ReferencesTool.h"
#include <KoBookmark.h>
#include <KoBookmarkManager.h>
#include <KoCanvasBase.h>
#include <KoCanvasResourceManager.h>
#include <KoTextDocument.h>
#include <KoTextRange.h>
#include <KoTextRangeManager.h>
#include <QAction>

SimpleLinksWidget::SimpleLinksWidget(ReferencesTool *tool, QWidget *parent)
    : QWidget(parent)
    , m_referenceTool(tool)
{
    widget.setupUi(this);
    Q_ASSERT(tool);
    widget.insertLink->setDefaultAction(tool->action("insert_link"));
    widget.invokeBookmarkHandler->setDefaultAction(tool->action("invoke_bookmark_handler"));
    connect(widget.insertLink, &QAbstractButton::clicked, this, &SimpleLinksWidget::doneWithFocus);
    connect(widget.invokeBookmarkHandler, &QAbstractButton::clicked, this, &SimpleLinksWidget::doneWithFocus);
    connect(widget.invokeBookmarkHandler, &FormattingButton::aboutToShowMenu, this, &SimpleLinksWidget::preparePopUpMenu);
}

void SimpleLinksWidget::preparePopUpMenu()
{
    if (widget.invokeBookmarkHandler->isFirstTimeMenuShown()) {
        widget.invokeBookmarkHandler->addAction(m_referenceTool->action("insert_bookmark"));
        widget.invokeBookmarkHandler->addSeparator();
        widget.invokeBookmarkHandler->addAction(m_referenceTool->action("manage_bookmarks"));
        connect(m_referenceTool->action("manage_bookmarks"), &QAction::triggered, this, &SimpleLinksWidget::manageBookmarks, Qt::UniqueConnection);
    }
}

void SimpleLinksWidget::manageBookmarks()
{
    QString name;
    const KoBookmarkManager *manager = KoTextDocument(m_referenceTool->editor()->document()).textRangeManager()->bookmarkManager();
    QPointer<ManageBookmarkDialog> dia =
        new ManageBookmarkDialog(manager->bookmarkNameList(), m_referenceTool->editor(), m_referenceTool->canvas()->canvasWidget());
    connect(dia.data(), &ManageBookmarkDialog::nameChanged, manager, &KoBookmarkManager::rename);
    connect(dia.data(), &ManageBookmarkDialog::bookmarkDeleted, manager, &KoBookmarkManager::remove);
    if (dia->exec() == QDialog::Accepted) {
        name = dia->selectedBookmarkName();
    } else {
        delete dia;
        return;
    }
    delete dia;
    KoBookmark *bookmark = manager->bookmark(name);

    KoCanvasResourceManager *rm = m_referenceTool->canvas()->resourceManager();
    if ((bookmark->positionOnlyMode() == false) && bookmark->hasRange()) {
        rm->clearResource(KoText::SelectedTextPosition);
        rm->clearResource(KoText::SelectedTextAnchor);
    }
    if (bookmark->positionOnlyMode()) {
        rm->setResource(KoText::CurrentTextPosition, bookmark->rangeStart());
        rm->setResource(KoText::CurrentTextAnchor, bookmark->rangeStart());
    } else {
        rm->setResource(KoText::CurrentTextPosition, bookmark->rangeStart());
        rm->setResource(KoText::CurrentTextAnchor, bookmark->rangeEnd());
    }
}

SimpleLinksWidget::~SimpleLinksWidget() = default;
