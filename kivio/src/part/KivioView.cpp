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

#include "KivioCanvas.h"

KivioView::KivioView(KoDocument* document, QWidget* parent, const char* name)
  : KoView(document, parent, name)
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
  m_scrollBarX = new QScrollBar(Qt::Horizontal, this);
  m_scrollBarX->setMaximum(m_canvas->pageWidth() - m_canvas->width());
  m_scrollBarX->setPageStep(m_canvas->width());
  connect(m_scrollBarX, SIGNAL(valueChanged(int)), m_canvas, SLOT(setOffsetX(int)));
  m_scrollBarY = new QScrollBar(Qt::Vertical, this);
  m_scrollBarY->setMaximum(m_canvas->pageHeight() - m_canvas->height());
  m_scrollBarY->setPageStep(m_canvas->height());
  connect(m_scrollBarY, SIGNAL(valueChanged(int)), m_canvas, SLOT(setOffsetY(int)));

  layout->addWidget(m_canvas, 0, 0);
  layout->addWidget(m_scrollBarY, 0, 1);
  layout->addWidget(m_scrollBarX, 1, 0);
}

#include "KivioView.moc"
