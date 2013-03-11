/* This file is part of the KDE project
 * Copyright (C) 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 * Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
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
 * Boston, MA 02110-1301, USA
 */

#include "CAuView.h"
#include "CAuFactory.h"
#include "CoverImage.h"

#include "KWDocument.h"

#include <KoPart.h>

#include <QWidget>
#include <QFileDialog>
#include <kaction.h>
#include <kactioncollection.h>

CAuView::CAuView(KoPart *part, KWDocument *document, QWidget *parent)
    :KWView(part, document, parent)
{
        setComponentData(CAuFactory::componentData());
        setXMLFile("author.rc");
        setupActions();
}

void CAuView::setupActions()
{
    // -------- Book
    KAction *action = new KAction(i18n("Insert Cover Image"), this);
    actionCollection()->addAction("insert_coverimage", action);
    action->setToolTip(i18n("Set cover for your ebook"));
    connect(action, SIGNAL(triggered()), this, SLOT(selectCoverImage()));
}

void CAuView::selectCoverImage()
{
    CoverImage cover;

    QString path = QFileDialog::getOpenFileName(0, i18n("Open File"),
                                                QDir::currentPath(),
                                                      i18n("Images (*.png *.xpm *.jpg)"));
    if (!path.isEmpty()) {
        QPair<QString, QByteArray> coverData = cover.readCoverImage(path);
        kwdocument()->setCoverImage(coverData);
    }
}
