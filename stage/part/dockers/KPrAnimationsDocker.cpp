/* This file is part of the KDE project
   Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrAnimationsDocker.h"

#include "KPrView.h"
#include "KPrAnimationsTimeLineView.h"
#include "KPrAnimationsDataModel.h"
#include "KPrPage.h"

//Qt Headers
#include <QToolButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>

//KDE Headers
#include <KIcon>
#include <KLocale>
#include <KIconLoader>

KPrAnimationsDocker::KPrAnimationsDocker(QWidget* parent, Qt::WindowFlags flags)
: QDockWidget(parent, flags)
, m_view(0)
{
    setWindowTitle( i18n( "Shape Animations" ) );

    QWidget* base = new QWidget( this );
    QHBoxLayout *hlayout = new QHBoxLayout;
    QHBoxLayout *hlayout2 = new QHBoxLayout;

    //Setup buttons
    m_editAnimation = new QToolButton();
    m_editAnimation->setText(i18n("Edit animation"));
    m_editAnimation->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_editAnimation->setIconSize(QSize(KIconLoader::SizeSmallMedium, KIconLoader::SizeSmallMedium));
    m_editAnimation->setIcon(KIcon("edit_animation"));
    m_editAnimation->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_editAnimation->setToolTip(i18n("Edit animation"));
    hlayout->addWidget(m_editAnimation);
    hlayout->addStretch();

    //m_moreShapesContainer = new CollectionMenu(mainWidget);
    //m_moreShapes->setMenu(m_moreShapesContainer);
    //m_moreShapes->setPopupMode(QToolButton::InstantPopup);

    m_buttonAddAnimation = new QToolButton();
    m_buttonAddAnimation->setIcon(SmallIcon("list-add", KIconLoader::SizeSmallMedium));
    m_buttonAddAnimation->setToolTip(i18n("Add new animation"));

    m_buttonRemoveAnimation = new QToolButton();
    m_buttonRemoveAnimation->setIcon(SmallIcon("list-remove", KIconLoader::SizeSmallMedium));
    m_buttonRemoveAnimation->setEnabled(false);
    m_buttonRemoveAnimation->setToolTip(i18n("Remove animation"));
    hlayout->addWidget(m_buttonAddAnimation);
    hlayout->addWidget(m_buttonRemoveAnimation);

    QLabel *orderLabel = new QLabel(i18n("Order: "));
    m_buttonAnimationOrderUp = new QToolButton();
    m_buttonAnimationOrderUp->setIcon(SmallIcon("arrow-down"));
    m_buttonAnimationOrderUp->setToolTip(i18n("Move animation down"));
    m_buttonAnimationOrderUp->setEnabled(false);

    m_buttonAnimationOrderDown = new QToolButton();
    m_buttonAnimationOrderDown->setIcon(SmallIcon("arrow-up"));
    m_buttonAnimationOrderDown->setToolTip(i18n("Move animation up"));
    m_buttonAnimationOrderDown->setEnabled(false);
    hlayout2->addStretch();
    hlayout2->addWidget(orderLabel);
    hlayout2->addWidget(m_buttonAnimationOrderUp);
    hlayout2->addWidget(m_buttonAnimationOrderDown);


    //load View
    m_animationsTimeLineView = new KPrAnimationsTimeLineView();
    m_animationsModel = new KPrAnimationsDataModel();
    m_animationsTimeLineView->setModel(m_animationsModel);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(hlayout);
    layout->addWidget(m_animationsTimeLineView);
    layout->addLayout(hlayout2);
    base->setLayout(layout);
    setWidget(base);
}

void KPrAnimationsDocker::setView(KPrView* view)
{
    Q_ASSERT( view );
    if (m_view) {
        // don't disconnect the m_view->proxyObject as the object is already deleted
        disconnect(m_animationsTimeLineView, 0, this, 0);
    }
    m_view = view;
    slotActivePageChanged();
    connect(m_view->proxyObject, SIGNAL(activePageChanged()),
             this, SLOT(slotActivePageChanged()));

    // remove the layouts from the last view
    //m_layoutsView->clear();


    /*connect( m_layoutsView, SIGNAL( itemPressed( QListWidgetItem * ) ),
             this, SLOT( slotItemPressed( QListWidgetItem * ) ) );
    connect( m_layoutsView, SIGNAL( currentItemChanged( QListWidgetItem *, QListWidgetItem * ) ),
             this, SLOT( slotCurrentItemChanged( QListWidgetItem *, QListWidgetItem * ) ) );*/
}

void KPrAnimationsDocker::slotActivePageChanged()
{
    Q_ASSERT( m_view );
    KPrPage *page = dynamic_cast<KPrPage*>(m_view->activePage());
    if ( page ) {
        m_animationsModel->setActivePage(page);
        m_animationsTimeLineView->update();
    }
}
