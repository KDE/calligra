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
#include "CoverSelectionDialog.h"

#include "KWDocument.h"

#include <KoPart.h>
#include <KoDockRegistry.h>
#include <KStatusBar>

#include <QWidget>
#include <QFileDialog>
#include <ktoggleaction.h>
#include <kaction.h>
#include <kactioncollection.h>

CAuView::CAuView(KoPart *part, KWDocument *document, QWidget *parent)
    :KWView(part, document, parent)
{
        setComponentData(CAuFactory::componentData());
        setXMLFile("author.rc");

        buildAssociatedWidget();
        setupActions();
}

void CAuView::setupActions()
{
    // -------- Book
    KAction *action = new KAction(i18n("Select Cover Image..."), this);
    actionCollection()->addAction("insert_coverimage", action);
    action->setToolTip(i18n("Set cover for your ebook"));
    connect(action, SIGNAL(triggered()), this, SLOT(selectCoverImage()));

    // -------- Statistics in the status bar
    KToggleAction *tAction = new KToggleAction(i18n("Word Count"), this);
    tAction->setToolTip(i18n("Shows or hides word counting in status bar"));
    //always display at start so -> "true"
    tAction->setChecked(true);
    actionCollection()->addAction("view_wordCount", tAction);
    connect(tAction, SIGNAL(toggled(bool)), this, SLOT(showStatsInStatusBar(bool)));
}

void CAuView::buildAssociatedWidget() {
    stats = new KWStatisticsWidget(this,true);
    stats->setLayoutDirection(KWStatisticsWidget::LayoutHorizontal);
    stats->setCanvas(dynamic_cast<KWCanvas*>(this->canvas()));
    statusBar()->insertWidget(0,stats);
}

void CAuView::showStatsInStatusBar(bool toggled)
{
    stats->setVisible(toggled);
}

void CAuView::selectCoverImage()
{
    //first "this" for CAuView context and second one for window parent
    CoverSelectionDialog* coverDialog = new CoverSelectionDialog(this,this);
    coverDialog->show();
}

QPair<QString, QByteArray> CAuView::getCurrentCoverImage()
{
    /*if(kwdocument()->coverImage().second.isNull())
        qDebug() << "AUTHOR : get problem";
    else
        qDebug() << "AUTHOR : get ok";*/
    return (kwdocument()->coverImage());
}

void CAuView::setCurrentCoverImage(QPair<QString, QByteArray> img)
{
    kwdocument()->setCoverImage(img);
}
