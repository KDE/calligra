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

#include "kis_doc.h"
#include "kis_view.h"
#include "kis_shell.h"
#include "kis_factory.h"
#include "kis_dlg_new.h"

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
  img->addLayer(QRect(0, 0, 512, 512), KisColor::white(), "background");
  img->setLayerOpacity(255);
  img->compositeImage(QRect(0, 0, 512, 512));
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
  if (!img)
	return false;

  QDomDocument doc( "image" );
  doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );

  // image element
  QDomElement image = doc.createElement( "image" );
  image.setAttribute( "author", "Matthias Elter" ); // FIXME
  image.setAttribute( "email", "elter@kde.org" ); // FIXME
  image.setAttribute( "editor", "KImageShop" );
  image.setAttribute( "mime", "application/x-kimageshop" );
  image.setAttribute( "width", img->width() );
  image.setAttribute( "height", img->height() );
  image.setAttribute( "bitDepth", "8" ); // FIXME
  image.setAttribute( "cMode", "3" ); // FIXME
  doc.appendChild( image );

  // layer elements
  QList<KisLayer> layers = img->layerList();
  for (KisLayer *lay = layers.first(); lay != 0; lay = layers.next())
	{
	  QDomElement layer = doc.createElement( "layer" );
	  layer.setAttribute( "name", lay->name() );
	  layer.setAttribute( "x", lay->imageExtents().x() );
	  layer.setAttribute( "y", lay->imageExtents().y() );
	  layer.setAttribute( "width", lay->imageExtents().width() );
	  layer.setAttribute( "height", lay->imageExtents().height() );
	  layer.setAttribute( "opacity", static_cast<int>(lay->opacity()) );

	  if (lay->visible())
		layer.setAttribute( "isVisible", "true" );
	  else
		layer.setAttribute( "isVisible", "false" );

	  if (lay->linked())
		layer.setAttribute( "isLinked", "true" );
	  else
		layer.setAttribute( "isLinked", "false" );

	  image.appendChild( layer );

	  // channel elements // FIXME
	  QDomElement c1 = doc.createElement( "channel" );
	  c1.setAttribute( "id", "R" );
	  layer.appendChild( c1 );
	  QDomElement c2 = doc.createElement( "channel" );
	  c2.setAttribute( "id", "G" );
	  layer.appendChild( c2 );
	  QDomElement c3 = doc.createElement( "channel" );
	  c3.setAttribute( "id", "B" );
	  layer.appendChild( c3 );
	  QDomElement c4 = doc.createElement( "channel" );
	  c4.setAttribute( "id", "A" );
	  layer.appendChild( c4 );
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

bool KisDoc::completeSaving( KoStore* )
{
  // TODO: Store binary image data.
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

bool KisDoc::loadXML( const QDomDocument& , KoStore* )
{
  // TODO: Load XML file.
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

/*
bool KisDoc::loadImage( const QString& file )
{
  QImage img(file);

  if (img.isNull())
    return false;

  QString alphaName = file;
  alphaName.replace(QRegExp("\\.jpg$"),"-alpha.jpg");
  qDebug("alphaname=%s\n",alphaName.latin1());
  QImage alpha(alphaName);

  QString name = QFileInfo(file).fileName();
  int w = img.width();
  int h = img.height();

  KisImage *kis_img = newImage(name, w, h, cm_RGBA, bm_White);
  if (!kis_img)
    return false;


  kis_img->addRGBLayer(img, alpha, name);
  kis_img->setLayerOpacity(255);
  kis_img->compositeImage(QRect(0, 0, w, h));
  setCurrentImage(kis_img);
  return true;
}
*/

void KisDoc::slotNewImage()
{
  if (!m_pNewDialog)
    m_pNewDialog = new NewDialog();
  m_pNewDialog->show();

  if(!m_pNewDialog->result() == QDialog::Accepted)
    return;

  int w = m_pNewDialog->newwidth();
  int h = m_pNewDialog->newheight();
  //bgMode bg = m_pNewDialog->backgroundMode();
  cMode cm = m_pNewDialog->colorMode();

  QString name;
  name.sprintf("image %d", m_Images.count()+1);

  KisImage *img = newImage(name, w, h, cm, 8);
  if (!img)
    return;
  
  // add background layer
  img->addLayer(QRect(0, 0, w, h), KisColor::white(), "background");
  img->setLayerOpacity(255);
  img->compositeImage(QRect(0, 0, w, h));
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
    shell->setRootDocument( this );
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
