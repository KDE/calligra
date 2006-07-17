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

#include <QDomElement>
#include <QDomDocument>

#include <kcommand.h>
#include <kdebug.h>
#include <klocale.h>

#include <KoMainWindow.h>
#include <KoDom.h>
#include <KoXmlNS.h>
#include <KoShape.h>
#include <KoShapeManager.h>

#include "KivioView.h"
#include "KivioMasterPage.h"
#include "KivioPage.h"
#include "KivioAbstractPage.h"

KivioDocument::KivioDocument(QWidget* parentWidget, QObject* parent, bool singleViewMode)
  : KoDocument(parentWidget, parent, singleViewMode)
{
  m_commandHistory = new KCommandHistory(actionCollection(), true);
  connect(m_commandHistory, SIGNAL(documentRestored()),
          this, SLOT(slotDocumentRestored()));
  connect(m_commandHistory, SIGNAL(commandExecuted(KCommand*)),
          this, SLOT(slotCommandExecuted()));

  //TODO Remove when ready testing
  KivioMasterPage* masterPage = addMasterPage("standard");
  addPage(masterPage, "page 1");
}

KivioDocument::~KivioDocument()
{
  delete m_commandHistory;
  m_commandHistory = 0;

  qDeleteAll(m_pageList);
  m_pageList.clear();
  qDeleteAll(m_masterPageList);
  m_masterPageList.clear();
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
  Q_UNUSED(oasisStyles);
  Q_UNUSED(settings);
  Q_UNUSED(store);

  kDebug(43000) << "Start loading OASIS document..." << endl;

  QDomElement contents = doc.documentElement();
  QDomElement realBody = KoDom::namedItemNS( contents, KoXmlNS::office, "body");

  if(realBody.isNull()) {
    kError(43000) << "No office:body found!" << endl;
    setErrorMessage(i18n("Invalid OASIS OpenDocument file. No office:body tag found."));
    return false;
  }

  QDomElement body = KoDom::namedItemNS( body, KoXmlNS::office, "drawing");

  if(body.isNull()) {
    kError(43000) << "No office:drawing found!" << endl;
    QDomElement childElem;
    QString localName;

    forEachElement(childElem, realBody) {
      localName = childElem.localName();
    }

    if(localName.isEmpty()) {
      setErrorMessage(i18n("Invalid OASIS OpenDocument file. No tag found inside office:body." ) );
    } else {
      setErrorMessage(i18n("This is not a graphical document, but %1. Please try opening it with the appropriate application.", KoDocument::tagNameToDocumentType(localName)));
    }

    return false;
  }

  return true;
}

bool KivioDocument::loadMasterPages(const KoOasisContext& oasisContext)
{
  Q_UNUSED(oasisContext);
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

KivioMasterPage* KivioDocument::addMasterPage(const QString& title)
{
  KivioMasterPage* masterPage = new KivioMasterPage(title);
  m_masterPageList.append(masterPage);

  return masterPage;
}

KivioPage* KivioDocument::addPage(KivioMasterPage* masterPage, const QString& title)
{
  if(!masterPage) {
    return 0;
  }

  KivioPage* page = new KivioPage(masterPage, title);
  m_pageList.append(page);

  return page;
}

KivioMasterPage* KivioDocument::masterPageByIndex(int index)
{
  return m_masterPageList.at(index);
}

KivioPage* KivioDocument::pageByIndex(int index)
{
  return m_pageList.at(index);
}

void KivioDocument::addShape(KivioAbstractPage* page, KoShape* shape)
{
  if(page == 0 || shape == 0) {
    return;
  }

  page->addShape(shape);

  KoView* view = 0;
  KivioView* kivioView = 0;
  KivioPage* kivioPage = 0;

  foreach(view, views()) {
    kivioView = qobject_cast<KivioView*>(view);

    if(kivioView && kivioView->activePage()) {
      if(kivioView->activePage() == page) {
        kivioView->shapeManager()->add(shape);
      } else {
        kivioPage = dynamic_cast<KivioPage*>(kivioView->activePage());

        if(kivioPage && (kivioPage->masterPage() == page)) {
          kivioView->shapeManager()->add(shape);
        }
      }
    }
  }
}

void KivioDocument::removeShape(KivioAbstractPage* page, KoShape* shape)
{
  if(page == 0 || shape == 0) {
    return;
  }

  page->removeShape(shape);

  KoView* view = 0;
  KivioView* kivioView = 0;
  KivioPage* kivioPage = 0;

  foreach(view, views()) {
    kivioView = qobject_cast<KivioView*>(view);

    if(kivioView && kivioView->activePage()) {
      if(kivioView->activePage() == page) {
        kivioView->shapeManager()->remove(shape);
      } else {
        kivioPage = dynamic_cast<KivioPage*>(kivioView->activePage());

        if(kivioPage && (kivioPage->masterPage() == page)) {
          kivioView->shapeManager()->remove(shape);
        }
      }
    }
  }
}

int KivioDocument::pageCount() const
{
  return m_pageList.count();
}

#include "KivioDocument.moc"
