/*
 *  kis_doc.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <string.h>
#include <stdio.h>
#include <iostream.h>
#include <sys/time.h>
#include <unistd.h>

#include <qpainter.h>
#include <qwidget.h>
#include <qregexp.h>
#include <qfileinfo.h>
#include <qdom.h>
#include <qtextstream.h>
#include <qbuffer.h>

#include <kstddirs.h>
#include <kglobal.h>
#include <kmimetype.h>

#include <koStore.h>
#include <koStoreStream.h>

#include "kis_doc.h"
#include "kis_view.h"
#include "kis_shell.h"
#include "kis_factory.h"
#include "kis_dlg_new.h"
#include "kis_channel.h"

#define KIS_DEBUG(AREA, CMD)

KisDoc::KisDoc( QObject* parent, const char* name, bool singleViewMode )
  : KoDocument( parent, name, singleViewMode )
  , m_commands()
{
  setInstance( KisFactory::global() );
  m_pCurrent = 0L;
  m_pNewDialog = 0L;

  m_Images.setAutoDelete(false);
}

bool KisDoc::initDoc()
{
  QString name;
  name.sprintf("image %d", m_Images.count()+1);
  KisImage *img = newImage(name, 512, 512, cm_RGBA, 8);
  if (!img)
    return false;

  // add background layer
  img->addLayer(QRect(0, 0, 512, 512), KisColor::white(), false, "background");
  img->markDirty(QRect(0, 0, 512, 512));
  setCurrentImage(img);

  emit imageListUpdated();
  return true;
}

bool KisDoc::hasToWriteMultipart()
{
  return true; // Always save to a KoStore, never to a plain XML file.
}

bool KisDoc::save( ostream& out, const char* )
{
  cout << " --- KisDoc::save --- " << endl;
  KisImage *img = m_pCurrent;
  if (!img)	return false;

  // FIXME: implement saving of non-RGB modes.
  if (img->colorMode() != cm_RGB
	  && img->colorMode() != cm_RGBA)
	return false;

  QDomDocument doc( "image" );
  doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );

  // image element
  QDomElement image = doc.createElement( "image" );
  image.setAttribute( "name", img->name() );
  image.setAttribute( "author", img->author() );
  image.setAttribute( "email", img->email() );
  image.setAttribute( "editor", "KImageShop" );
  image.setAttribute( "mime", "application/x-kimageshop" );
  image.setAttribute( "width", img->width() );
  image.setAttribute( "height", img->height() );
  image.setAttribute( "bitDepth", static_cast<int>(img->bitDepth()) );
  image.setAttribute( "cMode", static_cast<int>(img->colorMode()) );
  doc.appendChild( image );

  // layers element
  QDomElement layers = doc.createElement( "layers" );
  image.appendChild( layers );

  // layer elements
  QList<KisLayer> l_lst = img->layerList();
  for (KisLayer *lay = l_lst.first(); lay != 0; lay = l_lst.next())
	{
	  QDomElement layer = doc.createElement( "layer" );
	  layer.setAttribute( "name", lay->name() );
	  layer.setAttribute( "x", lay->imageExtents().x() );
	  layer.setAttribute( "y", lay->imageExtents().y() );
	  layer.setAttribute( "width", lay->imageExtents().width() );
	  layer.setAttribute( "height", lay->imageExtents().height() );
	  layer.setAttribute( "opacity", static_cast<int>(lay->opacity()) );

	  if (lay->visible())
		layer.setAttribute( "visible", "true" );
	  else
		layer.setAttribute( "visible", "false" );

	  if (lay->linked())
		layer.setAttribute( "linked", "true" );
	  else
		layer.setAttribute( "linked", "false" );

	  layer.setAttribute( "bitDepth", static_cast<int>(lay->bitDepth()) );
	  layer.setAttribute( "cMode", static_cast<int>(lay->colorMode()) );

	  layers.appendChild( layer );

	  // channels element
	  QDomElement channels = doc.createElement( "channels" );
	  layer.appendChild( channels );

	  // channel elements
	  for ( KisChannel* ch = lay->firstChannel(); ch != 0; ch = lay->nextChannel())
		{
		  QDomElement channel = doc.createElement( "channel" );
		  channel.setAttribute( "cId", static_cast<int>(ch->channelId()) );
		  channel.setAttribute( "bitDepth", static_cast<int>(ch->bitDepth()) );
		  channels.appendChild( channel );
		}
	}

  // Save to buffer
  QBuffer buffer;
  buffer.open( IO_WriteOnly );
  QTextStream str( &buffer );
  str << doc;
  buffer.close();

  out.write( buffer.buffer().data(), buffer.buffer().size() );

  setModified( false );

  return true;
}

bool KisDoc::completeSaving( KoStore* store )
{
  if ( !store ) return false;
  if (!m_pCurrent)	return false;

  QList<KisLayer> layers = m_pCurrent->layerList();

  for (KisLayer *lay = layers.first(); lay != 0; lay = layers.next())
	{
	  for ( KisChannel* ch = lay->firstChannel(); ch != 0; ch = lay->nextChannel())
		{
		  QString url = QString( "layers/%1/channels/ch%2.bin" ).arg( lay->name() )
			.arg( static_cast<int>(ch->channelId()) );

		  if ( store->open( url ) )
			{
			  ostorestream out( store );
			  ch->writeToStore( &out );
			  out.flush();
			  store->close();
		  }
		}
	}
  return true;
}

bool KisDoc::load( istream& in, KoStore* store )
{
  QBuffer buffer;
  buffer.open( IO_WriteOnly );

  char buf[ 4096 ];
  int anz;
  do
    {
	  in.read( buf, 4096 );
	  anz = in.gcount();
	  buffer.writeBlock( buf, anz );
    } while( anz > 0 );

  buffer.close();

  buffer.open( IO_ReadOnly );
  QDomDocument doc( &buffer );

  bool b = loadXML( doc, store );

  buffer.close();
  return b;
}

bool KisDoc::loadXML( const QDomDocument& doc , KoStore* )
{
  if ( doc.doctype().name() != "image" )
	return false;

  QDomElement image = doc.documentElement();

  if (image.attribute( "mime" ) != "application/x-kimageshop") return false;

  QString name = image.attribute( "name" );
  int w = image.attribute( "width" ).toInt();
  int h = image.attribute( "height" ).toInt();
  int cm = image.attribute( "cMode" ).toInt();
  int bd = image.attribute( "bitDepth" ).toInt();

  cMode colorMode;

  switch (cm)
	{
	case 0:
	  colorMode = cm_Indexed;
	  break;
	case 1:
	  colorMode = cm_Greyscale;
	  break;
	case 2:
	  colorMode = cm_RGB;
	  break;
	case 3:
	  colorMode = cm_RGBA;
	  break;
	case 4:
	  colorMode = cm_CMYK;
	  break;
	case 5:
	  colorMode = cm_CMYKA;
	  break;
	case 6:
	  colorMode = cm_Lab;
	  break;
	case 7:
	  colorMode = cm_LabA;
	  break;
	default:
	  return false;
	}

  KisImage *img = newImage(name, w, h, colorMode, bd);
  if (!img) return false;

  img->setAuthor( image.attribute( "author" ));
  img->setEmail( image.attribute( "email" ));

  // layers element
  QDomElement layers = image.namedItem( "layers" ).toElement();
  if (layers.isNull()) return false;

  // layer elements
  QDomNode l = layers.firstChild();

  while (!l.isNull())
	{
      QDomElement layer = l.toElement();
      if (layer.tagName() != "layer" ) continue;

	  cout << "--- layer ---" << endl;

	  // channels element
	  QDomElement channels = layer.namedItem( "channels" ).toElement();
	  if (channels.isNull()) continue;

	  // channel elements
	  QDomNode c = channels.firstChild();
	
	  while (!c.isNull())
		{
		  QDomElement channel = c.toElement();
		  if (channel.tagName() != "channel" ) continue;

		  cout << "--- channel ---" << endl;
		  c = c.nextSibling();
		}
	  l = l.nextSibling();
	}

  // add background layer
  img->addLayer(QRect(0, 0, w, h), KisColor::white(), false, "background");
  img->markDirty(QRect(0, 0, w, h));
  setCurrentImage(img);

  return true;
}

bool KisDoc::completeLoading( KoStore* )
{
  // TODO: Load binary image data.
  return true;
}

void KisDoc::setCurrentImage(KisImage *img)
{
  if (m_pCurrent)
    {
      // disconnect old current image
      QObject::disconnect( m_pCurrent, SIGNAL( updated() ),
			   this, SLOT( slotImageUpdated() ) );
      QObject::disconnect( m_pCurrent, SIGNAL( updated( const QRect& ) ),
			   this, SLOT( slotImageUpdated( const QRect& ) ) );
      QObject::disconnect( m_pCurrent, SIGNAL( layersUpdated() ),
			this, SLOT( slotLayersUpdated() ) );
    }

  m_pCurrent = img;

  QObject::connect( m_pCurrent, SIGNAL( updated() ),
		    this, SLOT( slotImageUpdated() ) );
  QObject::connect( m_pCurrent, SIGNAL( updated( const QRect& ) ),
		    this, SLOT( slotImageUpdated( const QRect& ) ) );
  QObject::connect( m_pCurrent, SIGNAL( layersUpdated() ),
		    this, SLOT( slotLayersUpdated() ) );
  emit imageListUpdated();
  emit layersUpdated();
  emit docUpdated();
}

void KisDoc::setCurrentImage(const QString& _name)
{
  KisImage *img = m_Images.first();

  while (img)
    {
      if (img->name() == _name)
	{
	  setCurrentImage(img);
	  return;
	}
      img = m_Images.next();
    }
}

QStringList KisDoc::images()
{
  QStringList lst;

  KisImage *img = m_Images.first();

  while (img)
    {
      lst.append(img->name());
      img = m_Images.next();
    }
  return lst;
}

bool KisDoc::isEmpty()
{
  if (m_pCurrent)
    return false;
  return true;
}

QString KisDoc::currentImage()
{
  if (m_pCurrent)
    return m_pCurrent->name();
  return QString("");
}

KisImage* KisDoc::current()
{
  return m_pCurrent;
}

KisDoc::~KisDoc()
{
  KisImage *img = m_Images.first();

  while (img)
    {
      delete img;
      img = m_Images.next();
    }
}

KisImage* KisDoc::newImage(const QString& n, int width, int height, cMode cm , uchar bitDepth )
{
  KisImage *img = new KisImage( n, width, height, cm, bitDepth );
  m_Images.append(img);

  return img;
}

void KisDoc::removeImage( KisImage *img )
{
  m_Images.remove(img);
  delete img;

  if (m_Images.isEmpty())
    setCurrentImage(0L);
  else
    setCurrentImage(m_Images.last()); // #### FIXME
}

void KisDoc::slotRemoveImage( const QString& _name )
{
  KisImage *img = m_Images.first();

  while (img)
    {
      if (img->name() == _name)
	{
	  removeImage(img);
	  return;
	}
      img = m_Images.next();
    }
}

void KisDoc::slotNewImage()
{
  if (!m_pNewDialog)
    m_pNewDialog = new NewDialog();
  m_pNewDialog->show();

  if(!m_pNewDialog->result() == QDialog::Accepted)
    return;

  int w = m_pNewDialog->newwidth();
  int h = m_pNewDialog->newheight();
  bgMode bg = m_pNewDialog->backgroundMode();
  cMode cm = m_pNewDialog->colorMode();

  QString name;
  name.sprintf("image %d", m_Images.count()+1);

  KisImage *img = newImage(name, w, h, cm, 8);
  if (!img)
    return;

  // add background layer
  if (bg == bm_White)
	img->addLayer(QRect(0, 0, w, h), KisColor::white(), false, "background");
  else if (bg == bm_Transparent)
	img->addLayer(QRect(0, 0, w, h), KisColor::white(), true, "background");
  else if (bg == bm_ForegroundColor)
	img->addLayer(QRect(0, 0, w, h), KisColor::white(), false, "background"); // FIXME
  else if (bg == bm_BackgroundColor)
	img->addLayer(QRect(0, 0, w, h), KisColor::white(), false, "background"); // FIXME

  img->markDirty(QRect(0, 0, w, h));
  setCurrentImage(img);
}

QCString KisDoc::mimeType() const
{
    return "application/x-kimageshop";
}

KoView* KisDoc::createView( QWidget* parent, const char* name )
{
  KisView* view = new KisView( this, parent, name );
  addView( view );

  QObject::connect( &m_commands, SIGNAL( undoRedoChanged( QString, QString ) ),
                    view, SLOT( slotUndoRedoChanged( QString, QString ) ) );
  QObject::connect( &m_commands, SIGNAL( undoRedoChanged( QStringList, QStringList ) ),
                    view, SLOT( slotUndoRedoChanged( QStringList, QStringList ) ) );

  return view;
}

KoMainWindow* KisDoc::createShell()
{
    KoMainWindow* shell = new KisShell;
    shell->show();

    return shell;}

void KisDoc::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/)
{
  if (m_pCurrent)
    m_pCurrent->paintPixmap( &painter, rect );
}

void KisDoc::paintPixmap(QPainter *p, QRect area)
{
  if (m_pCurrent)
    m_pCurrent->paintPixmap(p, area);
}

void KisDoc::slotImageUpdated()
{
  emit docUpdated();
}

void KisDoc::slotImageUpdated( const QRect& rect )
{
  emit docUpdated(rect);
}

void KisDoc::slotLayersUpdated()
{
  emit layersUpdated();
}

#include "kis_doc.moc"
