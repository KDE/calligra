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

#include <kstddirs.h>
#include <kglobal.h>
#include <kmimetype.h>

#include "kis_doc.h"
#include "kis_view.h"
#include "kis_shell.h"
#include "kis_factory.h"
#include "kis_layer.h"
#include "kis_util.h"
#include "kis_brush.h"

#define KIS_DEBUG(AREA, CMD)

kisDoc::kisDoc( KoDocument* parent, const char* name )
  : KoDocument( parent, name )
  , m_commands()
{
  m_pCurrent = 0L;
  QObject::connect( &m_commands, SIGNAL( undoRedoChanged( QString, QString ) ),
                    this, SLOT( slotUndoRedoChanged( QString, QString ) ) );
  QObject::connect( &m_commands, SIGNAL( undoRedoChanged( QStringList, QStringList ) ),
                    this, SLOT( slotUndoRedoChanged( QStringList, QStringList ) ) );

  m_Images.setAutoDelete(false);  
}

bool kisDoc::initDoc()
{
  QString name;
  name.sprintf("image %d", m_Images.count()+1);
  newImage(name, 512, 512, RGB, WHITE);
  
  return true;
}

void kisDoc::setCurrentImage(kisImage *img)
{
  if (!img)
    return;

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

void kisDoc::setCurrentImage(const QString& _name)
{
  qDebug("kisDoc::setCurrentImage: %s", _name.latin1());
  kisImage *img = m_Images.first();
  
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

QStringList kisDoc::images()
{
  QStringList lst;
  
  kisImage *img = m_Images.first();
  
  while (img)
    {
      lst.append(img->name());
      img = m_Images.next();
    }
  return lst;  
}

QString kisDoc::currentImage()
{
  if (m_pCurrent)
    return m_pCurrent->name();
  return QString("");
}

kisDoc::~kisDoc()
{
  kisImage *img = m_Images.first();
  
  while (img)
    {
      delete img;
      img = m_Images.next();
    }
}

int kisDoc::height()
{
  if (m_pCurrent)
    return m_pCurrent->height();
  return 0;
}

int kisDoc::width()
{
  if (m_pCurrent)
    return m_pCurrent->width();
  return 0;
}

QRect kisDoc::imageExtents()
{
  if (m_pCurrent)
    return m_pCurrent->imageExtents();
  return QRect(0, 0, 0, 0);
}

QSize kisDoc::size()
{
  if (m_pCurrent)
    return m_pCurrent->size();
  return QSize(0, 0);
}

kisImage* kisDoc::newImage(const QString& _name, int w, int h, int /*colorModel*/, int /*backgroundMode*/)
{
  kisImage *img = new kisImage( _name, w, h );
  m_Images.append(img);
  setCurrentImage(img);
  emit imageAdded(img->name());
  return img;
}

void kisDoc::removeImage( kisImage *img )
{
  m_Images.remove(img);
  delete img;
}

void kisDoc::slotRemoveImage( const QString& _name )
{
  kisImage *img = m_Images.first();
  
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

void kisDoc::saveImage( const QString& /*file*/, kisImage */*img*/ )
{
  
}

void kisDoc::loadImage( const QString& /*file*/ )
{
  
}

void kisDoc::slotNewImage()
{
  if (!m_pNewDialog)
    m_pNewDialog = new NewDialog();
  m_pNewDialog->show();
  
  if(!m_pNewDialog->result() == QDialog::Accepted)
    return;

  int w = m_pNewDialog->newwidth();
  int h = m_pNewDialog->newheight();
  int bg = m_pNewDialog->background();
  int cm = m_pNewDialog->colorModel();

  QString name;
  name.sprintf("image %d", m_Images.count()+1);

  newImage(name, w, h, bg, cm);
}

void kisDoc::slotLoadImage()
{
  // FIXME: KFileDialog -> loadImage( file );
}

void kisDoc::slotSaveCurrentImage()
{
  // FIXME: KFileDialog -> saveImage(file, m_pCurrent);
}

bool kisDoc::loadFromURL( const QString& _url )
{
  cout << "kisDoc::loadFromURL" << endl;
  
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

QCString kisDoc::mimeType() const
{
    return "application/x-kimageshop";
}

View* kisDoc::createView( QWidget* parent, const char* name )
{
    kisView* view = new kisView( this, parent, name );
    addView( view );

    return view;
}

Shell* kisDoc::createShell()
{
    Shell* shell = new KImageShopShell;
    shell->setRootPart( this );
    shell->show();

    return shell;}

void kisDoc::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/)
{
  if (m_pCurrent)
    m_pCurrent->paintPixmap( &painter, rect );
}

QString kisDoc::configFile() const
{
  return readConfigFile( locate("kis", "kimageshop.rc", KImageShopFactory::global()) );
}

void kisDoc::slotUndoRedoChanged( QString /*_undo*/, QString /*_redo*/ )
{
  //####### FIXME
}

void kisDoc::slotUndoRedoChanged( QStringList /*_undo*/, QStringList /*_redo*/ )
{
  //####### FIXME
}

void kisDoc::paintPixmap(QPainter *p, QRect area)
{
  if (m_pCurrent)
    m_pCurrent->paintPixmap(p, area);
}

void kisDoc::addRGBLayer(QString file)
{
  if (m_pCurrent)
    m_pCurrent->addRGBLayer( file );
}

void kisDoc::addRGBLayer(const QRect& rect, const QColor& c, const QString& name)
{
  if (m_pCurrent)
    m_pCurrent->addRGBLayer( rect, c, name );
}

void kisDoc::removeLayer( unsigned int _layer )
{
  if (m_pCurrent)
    m_pCurrent->removeLayer( _layer );
}

void kisDoc::compositeImage(QRect r)
{
  if (m_pCurrent)
    m_pCurrent->compositeImage( r );
}

Layer* kisDoc::layerPtr( Layer *_layer )
{
  if (m_pCurrent)
    return m_pCurrent->layerPtr( _layer );
  return 0;
}

Layer* kisDoc::getCurrentLayer()
{
  if (m_pCurrent)
    return m_pCurrent->getCurrentLayer();
  return 0;
}

int kisDoc::getCurrentLayerIndex()
{
  if (m_pCurrent)
    return m_pCurrent->getCurrentLayerIndex();
  return 0;
}

void kisDoc::setCurrentLayer( int _layer )
{
  if (m_pCurrent)
    m_pCurrent->setCurrentLayer( _layer );
}

void kisDoc::setLayerOpacity( uchar _opacity, Layer *_layer )
{
  if (m_pCurrent)
    m_pCurrent->setLayerOpacity( _opacity, _layer );
}

void kisDoc::moveLayer( int _dx, int _dy, Layer *_lay )
{
  if (m_pCurrent)
    m_pCurrent->moveLayer( _dx, _dy, _lay );
}

void kisDoc::moveLayerTo( int _x, int _y, Layer *_lay )
{
  if (m_pCurrent)
    m_pCurrent->moveLayerTo( _x, _y, _lay );
}

void kisDoc::mergeAllLayers()
{
  if (m_pCurrent)
    m_pCurrent->mergeAllLayers();
}

void kisDoc::mergeVisibleLayers()
{
  if (m_pCurrent)
    m_pCurrent->mergeVisibleLayers();
}

void kisDoc::mergeLinkedLayers()
{
  if (m_pCurrent)
    m_pCurrent->mergeLinkedLayers();
}

void kisDoc::mergeLayers(QList<Layer> list)
{
  if (m_pCurrent)
    m_pCurrent->mergeLayers( list );
}

void kisDoc::upperLayer( unsigned int _layer )
{
  if (m_pCurrent)
    m_pCurrent->upperLayer( _layer );
}

void kisDoc::lowerLayer( unsigned int _layer )
{
  if (m_pCurrent)
    m_pCurrent->lowerLayer( _layer );
}

void kisDoc::setFrontLayer( unsigned int _layer )
{
  if (m_pCurrent)
    m_pCurrent->setFrontLayer( _layer );
}

void kisDoc::setBackgroundLayer( unsigned int _layer )
{
  if (m_pCurrent)
    m_pCurrent->setBackgroundLayer( _layer );
}

void kisDoc::rotateLayer180(Layer *_layer)
{
  if (m_pCurrent)
    m_pCurrent->rotateLayer180( _layer );
}

void kisDoc::rotateLayerLeft90(Layer *_layer)
{
  if (m_pCurrent)
    m_pCurrent->rotateLayerLeft90( _layer );
}

void kisDoc::rotateLayerRight90(Layer *_layer)
{
  if (m_pCurrent)
    m_pCurrent->rotateLayerRight90( _layer );
}

void kisDoc::mirrorLayerX(Layer *_layer)
{
  if (m_pCurrent)
    m_pCurrent->mirrorLayerX( _layer );
}

void kisDoc::mirrorLayerY(Layer *_layer)
{
  if (m_pCurrent)
    m_pCurrent->mirrorLayerY( _layer );
}

void kisDoc::slotImageUpdated()
{
  emit docUpdated();
}

void kisDoc::slotImageUpdated( const QRect& rect )
{
  emit docUpdated(rect);
}

void kisDoc::slotLayersUpdated()
{
  emit layersUpdated();
}

LayerList kisDoc::layerList()
{
  if (m_pCurrent)
    return m_pCurrent->layerList();
  return LayerList();
};

#include "kis_doc.moc"
