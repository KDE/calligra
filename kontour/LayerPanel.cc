/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Jansen (rm@kde.org)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "LayerPanel.h"

#include <qpainter.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qheader.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>

#include "kontour_doc.h"
#include "kontour_factory.h"
#include "GDocument.h"
#include "GPage.h"
#include "GLayer.h"

TreeItem::TreeItem(QListView *parent):
QListViewItem(parent)
{
}

TreeItem::TreeItem(QListViewItem *parent):
QListViewItem(parent)
{
}

PageTreeItem::PageTreeItem( QListView *parent, GPage *aGPage):
TreeItem(parent)
{
  mGPage = aGPage;
//  setHeight(16);
}

void PageTreeItem::select()
{
  page()->document()->activePage(page());
}

void PageTreeItem::paintCell(QPainter *p, const QColorGroup &cg, int /*column*/, int width, int align)
{
  if(!p)
    return;

  if(isSelected())
  {
    p->fillRect(0, 0, width, height(), cg.brush(QColorGroup::Highlight));
    p->setPen(cg.highlightedText());
  }
  else
    p->fillRect(0, 0, width,height(),cg.base());

  if(page()->document()->activePage() == page())
  {
    p->save();
    p->setPen(QPen(red));
    p->drawRect(1, 1, width - 2, height() - 2);
    p->restore();
  }
  p->drawRect(2, 2, 16, 16); // TODO image
  p->drawText(19, 0, width, height(), align | AlignVCenter, page()->name(), -1);
}

void PageTreeItem::activate()
{
  kdDebug(38000) << "ACTIVATE" << endl;
}

LayerTreeItem::LayerTreeItem( QListViewItem *parent, GLayer *aGLayer):
TreeItem(parent)
{
  mGLayer = aGLayer;
//  setHeight(16);
}

void LayerTreeItem::select()
{
}

void LayerTreeItem::paintCell(QPainter *p, const QColorGroup &cg, int /*column*/, int width, int align)
{
  if(!p)
    return;

  if(isSelected())
  {
    p->fillRect(0, 0, width, height(), cg.brush(QColorGroup::Highlight));
    p->setPen(cg.highlightedText());
  }
  else
    p->fillRect(0, 0, width,height(), cg.base());

  if(layer()->page()->activeLayer() == layer())
  {
    p->drawRect(4, 4, 10, 10);
  }
  p->drawText(19, 0, width, height(), align | AlignVCenter, layer()->name(), -1);
}


LayerView::LayerView(GDocument *aGDoc, QWidget *parent, const char *name):
QListView(parent, name)
{
  mGDoc = aGDoc;
  addColumn("Pages", 200);
  header()->hide();
  setShowSortIndicator(false);
  setMinimumWidth(200);
  setAcceptDrops(true);

  connect(this, SIGNAL(doubleClicked(QListViewItem *)), SLOT(slotDoubleClicked(QListViewItem *)));

  updateView();
}

LayerView::~LayerView()
{
}

void LayerView::updateView()
{
  clear();
  for(QPtrListIterator<GPage> it(mGDoc->getPages()); it.current(); ++it)
  {
    PageTreeItem *p = new PageTreeItem(this, (GPage *)it);
    for(QPtrListIterator<GLayer> itt(((GPage *)it)->getLayers()); itt.current(); ++itt)
      new LayerTreeItem((QListViewItem*)p, (GLayer *)itt);
  }
}

void LayerView::slotDoubleClicked(QListViewItem *item)
{
  TreeItem *titem = (TreeItem *)item;
  titem->select();
}

// LayerPanel

LayerPanel::LayerPanel(GDocument *aGDoc, QWidget *parent, const char *name):
QDockWindow(QDockWindow::InDock, parent, name)
{
  mGDoc = aGDoc;

  QWidget *mLayerPanel = new QWidget(this);
  mLayerView = new LayerView(mGDoc, mLayerPanel);
  mRaiseButton = new QPushButton(mLayerPanel);
  mRaiseButton->setFixedSize(20, 20);
  mRaiseButton->setPixmap(SmallIcon("raiselayer", KontourFactory::global()));
  mLowerButton = new QPushButton(mLayerPanel);
  mLowerButton->setFixedSize(20, 20);
  mLowerButton->setPixmap(SmallIcon("lowerlayer", KontourFactory::global()));
  mNewButton = new QPushButton(mLayerPanel);
  mNewButton->setFixedSize(20, 20);
  mNewButton->setPixmap(SmallIcon("newlayer", KontourFactory::global()));
  mDeleteButton = new QPushButton(mLayerPanel);
  mDeleteButton->setFixedSize(20, 20);
  mDeleteButton->setPixmap(SmallIcon("deletelayer", KontourFactory::global()));

  QHBoxLayout *mButtonsLayout = new QHBoxLayout();
  mButtonsLayout->addWidget(mRaiseButton);
  mButtonsLayout->addWidget(mLowerButton);
  mButtonsLayout->addWidget(mNewButton);
  mButtonsLayout->addWidget(mDeleteButton);

  mGrid = new QGridLayout(mLayerPanel);
  mGrid->addLayout(mButtonsLayout, 0, 0);
  mGrid->addMultiCellWidget(mLayerView, 1, 1, 0, 1);

  //connect(layerView,SIGNAL(layerChanged()),this,SLOT(slotLayerChanged()));
  connect(mRaiseButton, SIGNAL(clicked()), SLOT(upPressed()));
  connect(mLowerButton, SIGNAL(clicked()), SLOT(downPressed()));
  connect(mNewButton, SIGNAL(clicked()), SLOT(newPressed()));
  connect(mDeleteButton, SIGNAL(clicked()), SLOT(deletePressed()));

  setWidget(mLayerPanel);
  setCloseMode(QDockWindow::Always);
  setOpaqueMoving(true);
  setCaption(i18n("Layers"));
  stateOfButton();
}

LayerPanel::~LayerPanel()
{
}

void LayerPanel::updatePanel()
{
  mLayerView->updateView();
  QPtrList<GLayer> list = mGDoc->activePage()->getLayers();
  mLowerButton->setEnabled(list.first() != mGDoc->activePage()->activeLayer());
  mRaiseButton->setEnabled(list.last() != mGDoc->activePage()->activeLayer());
}

void LayerPanel::upPressed()
{
/*    if(!document->document()->isReadWrite())
        return;
  document->activePage()->raiseLayer (document->activePage()->activeLayer ());
  layerView->setActiveDocument (document);
  slotLayerChanged();*/
}

void LayerPanel::downPressed()
{
/*    if(!document->document()->isReadWrite())
        return;
  document->activePage()->lowerLayer (document->activePage()->activeLayer ());
  layerView->setActiveDocument (document);
  slotLayerChanged();*/
}

void LayerPanel::newPressed()
{
  if(!mGDoc->document()->isReadWrite())
    return;
  GLayer *layer = mGDoc->activePage()->addLayer();
  mGDoc->activePage()->activeLayer(layer);
  updatePanel();
}

void LayerPanel::deletePressed()
{
/*    if(!document->document()->isReadWrite())
        return;
  document->activePage()->deleteLayer (document->activePage()->activeLayer ());
  layerView->setActiveDocument (document);
  stateOfButton();*/
}

void LayerPanel::stateOfButton()
{
/*    if(document && document->activePage())
    {
        bool state=document->activePage()->getLayers().count()>1;
        bool readWrite=document->document()->isReadWrite();
        btn_nl->setEnabled(readWrite);
        btn_dl->setEnabled(state&&readWrite);

        btn_rl->setEnabled(state);
        btn_ll->setEnabled(state);
        slotLayerChanged();
    }*/
}

void LayerPanel::slotLayerChanged()
{
/*    QPtrList<GLayer> list =document->activePage()->getLayers();
    btn_ll->setEnabled(list.first()!=document->activePage()->activeLayer ());
    btn_rl->setEnabled(list.last()!=document->activePage()->activeLayer ());*/
}

#include "LayerPanel.moc"
