/* This file is part of the KDE project
   Copyright (C) 2003-2005 Peter Simonsson <psn@linux.se>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kiviodragobject.h"

#include <qcstring.h>
#include <qstringlist.h>
#include <qdom.h>
#include <qtextstream.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qpainter.h>

#include <kdebug.h>

#include <kozoomhandler.h>
#include <koRect.h>
#include <koGlobal.h>

#include "object.h"
#include "smlobjectloader.h"
#include "smlobjectsaver.h"

KivioDragObject::KivioDragObject(QWidget* dragSource, const char* name)
  : QDragObject(dragSource, name)
{
  m_encodeMimeList[0] = "application/vnd.kde.kivio";
  m_encodeMimeList[1] = "text/xml";
  m_encodeMimeList[2] = "text/plain";
}

KivioDragObject::~KivioDragObject()
{
  QValueList<Kivio::Object*>::iterator it = m_objectList.begin();
  QValueList<Kivio::Object*>::iterator itEnd = m_objectList.end();
  Kivio::Object* object = 0;

  while(it != itEnd) {
    object = *it;
    it = m_objectList.remove(it);
    delete object;
  }
}

const char* KivioDragObject::format(int i) const
{
  if(i < NumEncodeFormats) {
    return m_encodeMimeList[i];
  }

  QImageDrag id;
  id.setImage(QImage()); // We need the format list!!!
  return id.format(i - NumEncodeFormats);
}

QByteArray KivioDragObject::encodedData(const char* mimetype) const
{
  if((m_encodeMimeList[0] == mimetype) ||
    (m_encodeMimeList[1] == mimetype) ||
    (m_encodeMimeList[2] == mimetype))
  {
    return kivioEncoded();
  } else if(qstrnicmp(mimetype, "image/", 6) == 0) {
    return imageEncoded(mimetype);
  }

  return QByteArray();
}

bool KivioDragObject::canDecode(QMimeSource* e)
{
  QValueList<QCString> decodeMimeList;
  decodeMimeList.append("application/vnd.kde.kivio");
  decodeMimeList.append("text/plain");

  for(QValueList<QCString>::Iterator it = decodeMimeList.begin(); it != decodeMimeList.end(); ++it) {
    if(e->provides(*it)) {
      return true;
    }
  }

  return false;
}

bool KivioDragObject::decode(QMimeSource* e, QValueList<Kivio::Object*>& objectList)
{
  bool ok = false;

  if(e->provides("application/vnd.kde.kivio")) {
    QDomDocument doc("KivioSelection");
    QByteArray data = e->encodedData("application/vnd.kde.kivio");
    doc.setContent(QCString(data, data.size() + 1));
    Kivio::SmlObjectLoader loader;
    Kivio::Object* object;
    QDomNode node = doc.documentElement().firstChild();

    while(!node.isNull()) {
      if(node.nodeName() == "KivioShape") {
        object = loader.loadObject(node.toElement());

        if(object) {
          objectList.append(object);
        }
      }

      node = node.nextSibling();
    }

    ok = !objectList.isEmpty();
  } else if(e->provides("text/plain")) {
    // FIXME Port this to object code
/*    QString str;
    ok = QTextDrag::decode(e, str);
    KivioStencilSpawner* ss = page->doc()->findInternalStencilSpawner("Dave Marotti - Text");
    KivioStencil* stencil = ss->newStencil();
    stencil->setPosition(0, 0);
    stencil->setDimensions(100, 100);
    stencil->setText(str);
    stencil->setTextFont(page->doc()->defaultFont());
    sl.clear();
    sl.append(stencil);*/
  }

  return ok;
}

QByteArray KivioDragObject::kivioEncoded() const
{
  if(m_objectList.isEmpty())
    return QByteArray();

  QDomDocument doc("KivioSelection");
  QDomElement elem = doc.createElement( "KivioSelection" );
  doc.appendChild(elem);
  Kivio::SmlObjectSaver saver;
  QValueList<Kivio::Object*>::const_iterator itEnd = m_objectList.end();

  for(QValueList<Kivio::Object*>::const_iterator it = m_objectList.begin(); it != itEnd; ++it) {
    elem.appendChild(saver.saveObject(*it, doc));
  }

  return doc.toCString();
}

QByteArray KivioDragObject::imageEncoded(const char* mimetype) const
{
  KoZoomHandler zoomHandler;
  zoomHandler.setZoomAndResolution(100, KoGlobal::dpiX(), KoGlobal::dpiY());
  KoRect boundingRect;

  QValueList<Kivio::Object*>::const_iterator itEnd = m_objectList.end();

  for(QValueList<Kivio::Object*>::const_iterator it = m_objectList.begin(); it != itEnd; ++it) {
    boundingRect.unite((*it)->boundingBox());
  }

  QPixmap buffer(zoomHandler.zoomItX(boundingRect.width()), zoomHandler.zoomItY(boundingRect.height()));
  buffer.fill(Qt::white);
  QPainter p(&buffer);
  p.translate(-zoomHandler.zoomItX(boundingRect.x()), -zoomHandler.zoomItY(boundingRect.y()));

  for(QValueList<Kivio::Object*>::const_iterator it = m_objectList.begin(); it != itEnd; ++it) {
    (*it)->paint(p, &zoomHandler);
  }

  p.end();

  QImageDrag id;
  id.setImage(buffer.convertToImage());
  return id.encodedData(mimetype);
}

void KivioDragObject::addObject(Kivio::Object* object)
{
  m_objectList.append(object);
}

#include "kiviodragobject.moc"
