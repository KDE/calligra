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

#include "KivioDocument.h"

#include <kcommand.h>

#include <KoMainWindow.h>

#include "KivioView.h"

KivioDocument::KivioDocument(QWidget* parentWidget, QObject* parent, bool singleViewMode)
  : KoDocument(parentWidget, parent, singleViewMode)
{
  m_commandHistory = new KCommandHistory(actionCollection(), true);
  connect(m_commandHistory, SIGNAL(documentRestored()),
          this, SLOT(slotDocumentRestored()));
  connect(m_commandHistory, SIGNAL(commandExecuted(KCommand*)),
          this, SLOT(slotCommandExecuted()));
}

KivioDocument::~KivioDocument()
{
  delete m_commandHistory;
  m_commandHistory = 0;
}

void KivioDocument::paintContent(QPainter &painter, const QRect &rect, bool transparent,
                            double zoomX, double zoomY)
{
  Q_UNUSED(painter);
  Q_UNUSED(rect);
  Q_UNUSED(transparent);
  Q_UNUSED(zoomX);
  Q_UNUSED(zoomY);
}

bool KivioDocument::loadXML(QIODevice* device, const QDomDocument& doc)
{
  Q_UNUSED(device);
  Q_UNUSED(doc);

  return true;
}

bool KivioDocument::loadOasis(const QDomDocument& doc, KoOasisStyles& oasisStyles,
                        const QDomDocument& settings, KoStore* store)
{
  Q_UNUSED(doc);
  Q_UNUSED(oasisStyles);
  Q_UNUSED(settings);
  Q_UNUSED(store);

  return true;
}

bool KivioDocument::saveOasis(KoStore* store, KoXmlWriter* manifestWriter)
{
  Q_UNUSED(store);
  Q_UNUSED(manifestWriter);

  return true;
}

KoView* KivioDocument::createViewInstance(QWidget* parent, const char* name)
{
  return new KivioView(this, parent, name);
}

void KivioDocument::addCommand(KCommand* command, bool execute)
{
  if(!command) return;

  m_commandHistory->addCommand(command, execute);
}

void KivioDocument::slotDocumentRestored()
{
  setModified(false);
}

void KivioDocument::slotCommandExecuted()
{
  setModified(true);
}

void KivioDocument::addShell(KoMainWindow* shell)
{
  connect(shell, SIGNAL(documentSaved()), m_commandHistory, SLOT(documentSaved()));
  connect(shell, SIGNAL(saveDialogShown()), this, SLOT(saveDialogShown()));

  KoDocument::addShell(shell);
}

#include "KivioDocument.moc"
