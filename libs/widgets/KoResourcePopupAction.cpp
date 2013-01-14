/* This file is part of the KDE project
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * Copyright (C) 2012 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
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

#include "KoResourcePopupAction.h"

#include "KoResourceServerProvider.h"
#include "KoResourceServerAdapter.h"
#include "KoResourceItemView.h"
#include "KoResourceModel.h"
#include "KoResourceItemDelegate.h"
#include "KoResource.h"
#include "KoCheckerBoardPainter.h"

#include <QMenu>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPainter>
#include <QGradient>
#include <QRect>

class KoResourcePopupAction::Private
{
public:
    Private() : applyMode(true), checkerPainter(4)
    {}
    QMenu *menu;
    KoResourceItemView *resourceList;
    KoResource *currentResource;
    bool applyMode;
    KoCheckerBoardPainter checkerPainter;
};

KoResourcePopupAction::KoResourcePopupAction(KoAbstractResourceServerAdapter *resourceAdapter, QObject *parent)
:  KAction(parent)
, d(new Private())
{
    Q_ASSERT(resourceAdapter);

    d->menu = new QMenu();
    QWidget *widget = new QWidget(d->menu);
    QWidgetAction *wdgAction = new QWidgetAction(widget);

    d->resourceList = new KoResourceItemView(widget);
    d->resourceList->setModel(new KoResourceModel(resourceAdapter, widget));
    d->resourceList->setItemDelegate(new KoResourceItemDelegate(widget));
    KoResourceModel * resourceModel = qobject_cast<KoResourceModel*>(d->resourceList->model());
    if (resourceModel)
        resourceModel->setColumnCount(1);

    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addWidget(d->resourceList);
    widget->setLayout(layout);

    wdgAction->setDefaultWidget(widget);
    d->menu->addAction(wdgAction);
    setMenu(d->menu);
    new QHBoxLayout(d->menu);
    d->menu->layout()->addWidget(widget);
    d->menu->layout()->setMargin(0);

    connect(d->resourceList, SIGNAL(clicked(QModelIndex)), this, SLOT(indexChanged(QModelIndex)));
}

KoResourcePopupAction::~KoResourcePopupAction()
{
    delete d;
}

KoResource *KoResourcePopupAction::currentResource()
{
    return static_cast<KoResource*>(d->resourceList->currentIndex().internalPointer());
}

void KoResourcePopupAction::setCurrentResource(KoResource* resource) const
{
    d->currentResource = resource;
}

void KoResourcePopupAction::indexChanged(QModelIndex modelIndex)
{
    if(! modelIndex.isValid())
        return;

    d->menu->hide();

    KoResource * resource = static_cast<KoResource*>( modelIndex.internalPointer());
    if(resource)
        emit resourceSelected(resource);
}

void KoResourcePopupAction::updateIcon()
{
    QSize iconSize(16,16);
    // This must be a QImage, as drawing to a QPixmap outside the
    // UI thread will cause sporadic crashes.
    QImage pm = icon().pixmap(iconSize).toImage();
    if(pm.isNull())
    {
        pm = QImage(iconSize, QImage::Format_ARGB32_Premultiplied);
        pm.fill(Qt::transparent);
        // there was no icon set so we assume
        // that we create an icon from the current color
        d->applyMode = false;
    }
    QPainter p(&pm);
    if(d->applyMode) {
        /*
        KoAbstractGradient *gradient = dynamic_cast<KoAbstractGradient*>(currentResource());
        if (gradient) {
            QGradient *newGradient = gradient->toQGradient();
            p.fillRect(QRect(0, iconSize.height() - 4, iconSize.width(), 4), newGradient);
        }
        */
        p.fillRect(0, iconSize.height() - 4, iconSize.width(), 4, currentResource()->image());
    }
    else {
        d->checkerPainter.paint(p, QRect(QPoint(),iconSize));
        p.fillRect(0, 0, iconSize.width(), iconSize.height(), currentResource()->image());
    }

    p.end();

    setIcon(QIcon(QPixmap::fromImage(pm)));
}

#include <KoResourcePopupAction.moc>
