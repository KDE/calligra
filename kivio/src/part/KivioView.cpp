/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/

#include "KivioView.h"

#include <QGridLayout>
#include <QScrollBar>

#include <klocale.h>
#include <kiconloader.h>

#include <KoCanvasController.h>
#include <KoShapeManager.h>
#include <KoToolManager.h>
#include <KoToolBox.h>
#include <KoShapeSelector.h>
#include <KoZoomAction.h>
#include <KoZoomHandler.h>
#include <KoPageLayout.h>

#include "KivioCanvas.h"
#include "KivioDocument.h"
#include "KivioAbstractPage.h"
#include "KivioPage.h"

KivioView::KivioView(KivioDocument* document, QWidget* parent, const char* name)
  : KoView(document, parent, name), m_document(document)
{
  m_activePage = 0;
  m_zoomHandler = new KoZoomHandler;

  initGUI();
  initActions();

  if(m_document->pageCount() > 0)
    setActivePage(m_document->pageByIndex(0));
}

KivioView::~KivioView()
{
  delete m_zoomHandler;
  m_zoomHandler = 0;
}

KivioCanvas* KivioView::canvasWidget() const
{
  return m_canvas;
}

QWidget* KivioView::canvas() const
{
  return static_cast<QWidget*>(m_canvas);
}

KivioDocument* KivioView::document() const
{
  return m_document;
}

void KivioView::updateReadWrite(bool readwrite)
{
  Q_UNUSED(readwrite);
}

void KivioView::initGUI()
{
  QGridLayout* layout = new QGridLayout(this);
  layout->setSpacing(0);
  layout->setMargin(0);
  setLayout(layout);

  m_canvas = new KivioCanvas(this);

  m_canvasController = new KoCanvasController(this);
  m_canvasController->setCanvas(m_canvas);

  layout->addWidget(m_canvasController, 0, 0);

  KoToolManager::instance()->addControllers(m_canvasController, this);
  KoToolManager::instance()->toolBox()->show();

  KoShapeSelector *selector = new KoShapeSelector(0, m_canvasController, ".*");
  selector->resize(QSize(100, 200));
  selector->show();
}

void KivioView::initActions()
{
  setXMLFile("kivio.rc");

  m_viewZoomAction = new KoZoomAction(KoZoomMode::ZOOM_WIDTH | KoZoomMode::ZOOM_PAGE,
                                       i18n("Zoom"), KIcon("viewmag"), 0,
                                       actionCollection(), "view_zoom");
  connect(m_viewZoomAction, SIGNAL(zoomChanged(const QString&)),
          this, SLOT(viewZoom(const QString&)));
}

KivioAbstractPage* KivioView::activePage() const
{
  return m_activePage;
}

KoShapeManager* KivioView::shapeManager() const
{
  return m_canvas->shapeManager();
}

void KivioView::setActivePage(KivioAbstractPage* page)
{
  if(!page) {
    return;
  }

  m_activePage = page;
  shapeManager()->setShapes(page->shapeList());
  m_canvas->updateSize();
}

void KivioView::addShape(KoShape* shape)
{
  m_document->addShape(m_activePage, shape);
}

void KivioView::removeShape(KoShape* shape)
{
  m_document->removeShape(m_activePage, shape);
}

KoZoomHandler* KivioView::zoomHandler() const
{
  return m_zoomHandler;
}

void KivioView::setZoom(int zoom)
{
  m_zoomHandler->setZoom(zoom);
  m_canvas->updateSize();
}

void KivioView::viewZoom(const QString& zoomStr)
{
  // No point trying to zoom something that isn't there...
  if(m_activePage == 0) {
    return;
  }

  bool ok = true;
  int zoom = 0;

  QString zoomString = zoomStr;
  zoomString.replace("&",""); // hack to work around bug in KSelectAction

  if(zoomString == KoZoomMode::toString(KoZoomMode::ZOOM_WIDTH) ) {
    m_zoomHandler->setZoomMode(KoZoomMode::ZOOM_WIDTH);
    KoPageLayout layout = m_activePage->pageLayout();
    zoom = qRound(static_cast<double>(m_canvasController->visibleWidth() * 100) /
        (m_zoomHandler->resolutionX() * layout.ptWidth)) - 1;

    ok = true;
  } else if(zoomString == KoZoomMode::toString(KoZoomMode::ZOOM_PAGE)) {
    m_zoomHandler->setZoomMode(KoZoomMode::ZOOM_PAGE);
    KoPageLayout layout = m_activePage->pageLayout();
    double height = m_zoomHandler->resolutionY() * layout.ptHeight;
    double width = m_zoomHandler->resolutionX() * layout.ptWidth;
    zoom = qMin(qRound(static_cast<double>(m_canvasController->visibleHeight() * 100) / height),
                qRound(static_cast<double>(m_canvasController->visibleWidth() * 100) / width)) - 1;

    ok = true;
  } else {
    m_zoomHandler->setZoomMode(KoZoomMode::ZOOM_CONSTANT);
    QRegExp regexp(".*(\\d+).*"); // "Captured" non-empty sequence of digits
    int pos = regexp.indexIn(zoomString);

    if(pos > -1) {
      zoom = regexp.cap(1).toInt(&ok);
    }
  }

  // Don't allow smaller zoom then 10%
  if(!ok || (zoom < 10) || (zoom == m_zoomHandler->zoomInPercent())) {
    return;
  }

  m_viewZoomAction->setZoom(zoomString);
  setZoom(zoom);
  m_canvas->setFocus();
}

void KivioView::resizeEvent(QResizeEvent* event)
{
  Q_UNUSED(event);

  if((m_zoomHandler->zoomMode() == KoZoomMode::ZOOM_WIDTH) ||
        (m_zoomHandler->zoomMode() == KoZoomMode::ZOOM_PAGE))
  {
    viewZoom(m_viewZoomAction->currentText());
  }
}

#include "KivioView.moc"
