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

#include <QMenu>
#include <QHBoxLayout>
#include <QHeaderView>

class KoResourcePopupAction::Private
{
public:
    Private()
    {}
    QMenu *menu;
    KoResourceItemView *resourceList;
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


void KoResourcePopupAction::indexChanged(QModelIndex modelIndex)
{
    if(! modelIndex.isValid())
        return;

    d->menu->hide();

    KoResource * resource = static_cast<KoResource*>( modelIndex.internalPointer());
    if(resource)
        emit resourceSelected(resource);
}

#include <KoResourcePopupAction.moc>
