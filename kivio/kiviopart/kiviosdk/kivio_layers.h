#ifndef KIVIO_LAYERS_H
#define KIVIO_LAYERS_H

#include "kivio_page.h"
#include "kivio_view.h"
#include <KoZoomHandler.h>

  extern KivioPage *page;
  extern KivioView* view;

  void update() { page->update(); }

  bool isStencilSelected( KivioStencil *s ) { return page->isStencilSelected(s); }
  void selectAllStencils() { page->selectAllStencils(); }
  void unselectAllStencils() { page->unselectAllStencils(); }
  bool unselectStencil( KivioStencil *s ) { return page->unselectStencil(s); }
  void selectStencil( KivioStencil *s ) { page->selectStencil(s); }

  KivioLayer *curLayer() { return page->curLayer(); }
  void setCurLayer( KivioLayer *pLayer ) { page->setCurLayer(pLayer); }

  KivioLayer *firstLayer() { return page->firstLayer(); }
  KivioLayer *nextLayer() { return page->nextLayer(); }
  KivioLayer *lastLayer() { return page->lastLayer(); }
  KivioLayer *prevLayer() { return page->prevLayer(); }

  bool removeCurrentLayer() { return page->removeCurrentLayer(); }
  void addLayer( KivioLayer *l ) { page->addLayer(l); }
  void insertLayer( int i, KivioLayer *l ) { page->insertLayer(i,l); }
  KivioLayer *layerAt( int i ) { return page->layerAt(i); }

  bool addStencil( KivioStencil *s ) { page->addStencil(s); return true; }

  void deleteSelectedStencils() { page->deleteSelectedStencils(); }
  void groupSelectedStencils() { page->groupSelectedStencils(); }
  void ungroupSelectedStencils() { page->ungroupSelectedStencils(); }

  void bringToFront() { page->bringToFront(); }
  void sendToBack() { page->sendToBack(); }

  void copy() { page->copy(); }
  void cut() { page->cut(); }
  void paste() { page->paste(view); }

#endif
