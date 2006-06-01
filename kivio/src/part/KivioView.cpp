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

#include <KoCanvasView.h>

#include "KivioCanvas.h"
#include "KivioDocument.h"

KivioView::KivioView(KivioDocument* document, QWidget* parent, const char* name)
  : KoView(document, parent, name), m_document(document)
{
  initGUI();
}

KivioView::~KivioView()
{
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

  KoCanvasView* canvasView = new KoCanvasView(this);
  canvasView->setCanvas(m_canvas);

  layout->addWidget(canvasView, 0, 0);
}

#include "KivioView.moc"
