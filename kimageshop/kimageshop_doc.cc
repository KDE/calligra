/*
 *  kimageshop_doc.cc - part of KImageShop
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

#include <kstddirs.h>
#include <kglobal.h>
#include <kmimetype.h>

#include "kimageshop_doc.h"
#include "kimageshop_view.h"
#include "kimageshop_shell.h"
#include "kimageshop_factory.h"
#include "layer.h"
#include "misc.h"
#include "brush.h"

#define KIS_DEBUG(AREA, CMD)

KImageShopDoc::KImageShopDoc( KoDocument* parent, const char* name )
  : KoDocument( parent, name )
  , m_commands()
{
  QObject::connect( &m_commands, SIGNAL( undoRedoChanged( QString, QString ) ),
                    this, SLOT( slotUndoRedoChanged( QString, QString ) ) );
  QObject::connect( &m_commands, SIGNAL( undoRedoChanged( QStringList, QStringList ) ),
                    this, SLOT( slotUndoRedoChanged( QStringList, QStringList ) ) );

  m_Images.setAutoDelete(false);  
}

bool KImageShopDoc::initDoc()
{
  newImage();
  
  return true;
}

void KImageShopDoc::setCurrentImage(KImageShopImage *img)
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
  emit docUpdated();
}

void KImageShopDoc::setCurrentImage(const QString& _name)
{
  KImageShopImage *img = m_Images.first();
  
  while (img)
    {
      if (img->name() == _name)
	{
	  setCurrentImage(img);
	  return;
	}
    }
}

KImageShopDoc::~KImageShopDoc()
{
  KImageShopImage *img = m_Images.first();
  
  while (img)
    {
      delete img;
      img = m_Images.next();
    }
}

int KImageShopDoc::height()
{
  if (m_pCurrent)
    return m_pCurrent->height();
  return 0;
}

int KImageShopDoc::width()
{
  if (m_pCurrent)
    return m_pCurrent->width();
  return 0;
}

QRect KImageShopDoc::imageExtents()
{
  if (m_pCurrent)
    return m_pCurrent->imageExtents();
  return QRect(0, 0, 0, 0);
}

QSize KImageShopDoc::size()
{
  if (m_pCurrent)
    return m_pCurrent->size();
  return QSize(0, 0);
}

KImageShopImage* KImageShopDoc::newImage()
{
  KImageShopImage *img = new KImageShopImage( QString("image ") + m_Images.count(), 512, 512 );
  m_Images.append(img);
  setCurrentImage(img);
  emit imageAdded(img->name());
}

void KImageShopDoc::saveImage( const QString& file, KImageShopImage *img )
{
  
}

void KImageShopDoc::loadImage( const QString& file )
{
  
}

void KImageShopDoc::slotNewImage()
{
  newImage();
}

void KImageShopDoc::slotLoadImage()
{
  // FIXME: KFileDialog -> loadImage( file );
}

void KImageShopDoc::slotSaveCurrentImage()
{
  // FIXME: KFileDialog -> saveImage(file, m_pCurrent);
}

bool KImageShopDoc::loadFromURL( const QString& _url )
{
  cout << "KImageShopDoc::loadFromURL" << endl;
  
  QString mimetype = KMimeType::findByURL( _url )->mimeType();
  
  if( ( mimetype == "image/png" ) ||
      ( mimetype == "image/jpeg" ) ||
      ( mimetype == "image/bmp" ) ||
      ( mimetype == "image/gif" ) )
    {
      // if( !m_image.load( _url ) )
      //	return false;

      return false;
    }
  return KoDocument::loadFromURL( _url );
}

QCString KImageShopDoc::mimeType() const
{
    return "application/x-kimageshop";
}

View* KImageShopDoc::createView( QWidget* parent, const char* name )
{
    KImageShopView* view = new KImageShopView( this, parent, name );
    addView( view );

    return view;
}

Shell* KImageShopDoc::createShell()
{
    Shell* shell = new KImageShopShell;
    shell->setRootPart( this );
    shell->show();

    return shell;}

void KImageShopDoc::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/)
{
  if (m_pCurrent)
    m_pCurrent->paintPixmap( &painter, rect );
}

QString KImageShopDoc::configFile() const
{
  return readConfigFile( locate("kis", "kimageshop.rc", KImageShopFactory::global()) );
}

void KImageShopDoc::slotUndoRedoChanged( QString /*_undo*/, QString /*_redo*/ )
{
  //####### FIXME
}

void KImageShopDoc::slotUndoRedoChanged( QStringList /*_undo*/, QStringList /*_redo*/ )
{
  //####### FIXME
}

void KImageShopDoc::paintPixmap(QPainter *p, QRect area)
{
  if (m_pCurrent)
    m_pCurrent->paintPixmap(p, area);
}

void KImageShopDoc::addRGBLayer(QString file)
{
  if (m_pCurrent)
    m_pCurrent->addRGBLayer( file );
}

void KImageShopDoc::addRGBLayer(const QRect& rect, const QColor& c, const QString& name)
{
  if (m_pCurrent)
    m_pCurrent->addRGBLayer( rect, c, name );
}

void KImageShopDoc::removeLayer( unsigned int _layer )
{
  if (m_pCurrent)
    m_pCurrent->removeLayer( _layer );
}

void KImageShopDoc::compositeImage(QRect r)
{
  if (m_pCurrent)
    m_pCurrent->compositeImage( r );
}

Layer* KImageShopDoc::layerPtr( Layer *_layer )
{
  if (m_pCurrent)
    return m_pCurrent->layerPtr( _layer );
  return 0;
}

Layer* KImageShopDoc::getCurrentLayer()
{
  if (m_pCurrent)
    return m_pCurrent->getCurrentLayer();
  return 0;
}

int KImageShopDoc::getCurrentLayerIndex()
{
  if (m_pCurrent)
    return m_pCurrent->getCurrentLayerIndex();
  return 0;
}

void KImageShopDoc::setCurrentLayer( int _layer )
{
  if (m_pCurrent)
    m_pCurrent->setCurrentLayer( _layer );
}

void KImageShopDoc::setLayerOpacity( uchar _opacity, Layer *_layer )
{
  if (m_pCurrent)
    m_pCurrent->setLayerOpacity( _opacity, _layer );
}

void KImageShopDoc::moveLayer( int _dx, int _dy, Layer *_lay )
{
  if (m_pCurrent)
    m_pCurrent->moveLayer( _dx, _dy, _lay );
}

void KImageShopDoc::moveLayerTo( int _x, int _y, Layer *_lay )
{
  if (m_pCurrent)
    m_pCurrent->moveLayerTo( _x, _y, _lay );
}

void KImageShopDoc::mergeAllLayers()
{
  if (m_pCurrent)
    m_pCurrent->mergeAllLayers();
}

void KImageShopDoc::mergeVisibleLayers()
{
  if (m_pCurrent)
    m_pCurrent->mergeVisibleLayers();
}

void KImageShopDoc::mergeLinkedLayers()
{
  if (m_pCurrent)
    m_pCurrent->mergeLinkedLayers();
}

void KImageShopDoc::mergeLayers(QList<Layer> list)
{
  if (m_pCurrent)
    m_pCurrent->mergeLayers( list );
}

void KImageShopDoc::upperLayer( unsigned int _layer )
{
  if (m_pCurrent)
    m_pCurrent->upperLayer( _layer );
}

void KImageShopDoc::lowerLayer( unsigned int _layer )
{
  if (m_pCurrent)
    m_pCurrent->lowerLayer( _layer );
}

void KImageShopDoc::setFrontLayer( unsigned int _layer )
{
  if (m_pCurrent)
    m_pCurrent->setFrontLayer( _layer );
}

void KImageShopDoc::setBackgroundLayer( unsigned int _layer )
{
  if (m_pCurrent)
    m_pCurrent->setBackgroundLayer( _layer );
}

void KImageShopDoc::rotateLayer180(Layer *_layer)
{
  if (m_pCurrent)
    m_pCurrent->rotateLayer180( _layer );
}

void KImageShopDoc::rotateLayerLeft90(Layer *_layer)
{
  if (m_pCurrent)
    m_pCurrent->rotateLayerLeft90( _layer );
}

void KImageShopDoc::rotateLayerRight90(Layer *_layer)
{
  if (m_pCurrent)
    m_pCurrent->rotateLayerRight90( _layer );
}

void KImageShopDoc::mirrorLayerX(Layer *_layer)
{
  if (m_pCurrent)
    m_pCurrent->mirrorLayerX( _layer );
}

void KImageShopDoc::mirrorLayerY(Layer *_layer)
{
  if (m_pCurrent)
    m_pCurrent->mirrorLayerY( _layer );
}

void KImageShopDoc::slotImageUpdated()
{
  emit docUpdated();
}

void KImageShopDoc::slotImageUpdated( const QRect& rect )
{
  emit docUpdated(rect);
}

void KImageShopDoc::slotLayersUpdated()
{
  emit layersUpdated();
}

LayerList KImageShopDoc::layerList()
{
  if (m_pCurrent)
    return m_pCurrent->layerList();
  return LayerList();
};

#include "kimageshop_doc.moc"
