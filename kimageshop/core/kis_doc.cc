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
  KisImage *img = newImage(name, 512, 512, RGB, WHITE);
  if (!img)
    return false;

  // add background layer
  img->addRGBLayer(QRect(0, 0, 512, 512), QColor(255, 255, 255), "background");
  img->setLayerOpacity(255);
  img->compositeImage(QRect(0, 0, 512, 512));
  setCurrentImage(img);

  emit imageListUpdated();
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

void KisDoc::renameImage(const QString& oldname, const QString &newname)
{
  KisImage *img = m_Images.first();

  while (img)
    {
      if (img->name() == oldname)
	{
	  img->setName(newname);
	  emit imageListUpdated();
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

KisDoc::~KisDoc()
{
  KisImage *img = m_Images.first();

  while (img)
    {
      delete img;
      img = m_Images.next();
    }
}

int KisDoc::height()
{
  if (m_pCurrent)
    return m_pCurrent->height();
  return 0;
}

int KisDoc::width()
{
  if (m_pCurrent)
    return m_pCurrent->width();
  return 0;
}

QRect KisDoc::imageExtents()
{
  if (m_pCurrent)
    return m_pCurrent->imageExtents();
  return QRect(0, 0, 0, 0);
}

QSize KisDoc::size()
{
  if (m_pCurrent)
    return m_pCurrent->size();
  return QSize(0, 0);
}

KisImage* KisDoc::newImage(const QString& _name, int w, int h, int /*colorModel*/, int /*backgroundMode*/)
{
  KisImage *img = new KisImage( _name, w, h );
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

bool KisDoc::saveImage( const QString& /*file*/, KisImage */*img*/ )
{
  return false;
}

bool KisDoc::saveCurrentImage( const QString& file )
{
  return saveImage(file, m_pCurrent);
}

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

  KisImage *kis_img = newImage(name, w, h, RGB, WHITE);
  if (!kis_img)
    return false;


  kis_img->addRGBLayer(img, alpha, name);
  kis_img->setLayerOpacity(255);
  kis_img->compositeImage(QRect(0, 0, w, h));
  setCurrentImage(kis_img);
  return true;
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
  int bg = m_pNewDialog->background();
  int cm = m_pNewDialog->colorModel();

  QString name;
  name.sprintf("image %d", m_Images.count()+1);

  KisImage *img = newImage(name, w, h, bg, cm);
  if (!img)
    return;

  // add background layer
  img->addRGBLayer(QRect(0, 0, w, h), QColor(255, 255, 255), "background");
  img->setLayerOpacity(255);
  img->compositeImage(QRect(0, 0, w, h));
  setCurrentImage(img);
}

bool KisDoc::loadFromURL( const QString& _url )
{
  cout << "KisDoc::loadFromURL" << endl;

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

void KisDoc::addRGBLayer(QString file)
{
  if (m_pCurrent)
    m_pCurrent->addRGBLayer( file );
}

void KisDoc::addRGBLayer(const QRect& rect, const QColor& c, const QString& name)
{
  if (m_pCurrent)
    m_pCurrent->addRGBLayer( rect, c, name );
}

void KisDoc::removeLayer( unsigned int _layer )
{
  if (m_pCurrent)
    m_pCurrent->removeLayer( _layer );
}

void KisDoc::compositeImage(QRect r)
{
  if (m_pCurrent)
    m_pCurrent->compositeImage( r );
}

KisLayer* KisDoc::layerPtr( KisLayer *_layer )
{
  if (m_pCurrent)
    return m_pCurrent->layerPtr( _layer );
  return 0;
}

KisLayer* KisDoc::getCurrentLayer()
{
  if (m_pCurrent)
    return m_pCurrent->getCurrentLayer();
  return 0;
}

int KisDoc::getCurrentLayerIndex()
{
  if (m_pCurrent)
    return m_pCurrent->getCurrentLayerIndex();
  return 0;
}

void KisDoc::setCurrentLayer( int _layer )
{
  if (m_pCurrent)
    m_pCurrent->setCurrentLayer( _layer );
}

void KisDoc::setLayerOpacity( uchar _opacity, KisLayer *_layer )
{
  if (m_pCurrent)
    m_pCurrent->setLayerOpacity( _opacity, _layer );
}

void KisDoc::moveLayer( int _dx, int _dy, KisLayer *_lay )
{
  if (m_pCurrent)
    m_pCurrent->moveLayer( _dx, _dy, _lay );
}

void KisDoc::moveLayerTo( int _x, int _y, KisLayer *_lay )
{
  if (m_pCurrent)
    m_pCurrent->moveLayerTo( _x, _y, _lay );
}

void KisDoc::mergeAllLayers()
{
  if (m_pCurrent)
    m_pCurrent->mergeAllLayers();
}

void KisDoc::mergeVisibleLayers()
{
  if (m_pCurrent)
    m_pCurrent->mergeVisibleLayers();
}

void KisDoc::mergeLinkedLayers()
{
  if (m_pCurrent)
    m_pCurrent->mergeLinkedLayers();
}

void KisDoc::mergeLayers(QList<KisLayer> list)
{
  if (m_pCurrent)
    m_pCurrent->mergeLayers( list );
}

void KisDoc::upperLayer( unsigned int _layer )
{
  if (m_pCurrent)
    m_pCurrent->upperLayer( _layer );
}

void KisDoc::lowerLayer( unsigned int _layer )
{
  if (m_pCurrent)
    m_pCurrent->lowerLayer( _layer );
}

void KisDoc::setFrontLayer( unsigned int _layer )
{
  if (m_pCurrent)
    m_pCurrent->setFrontLayer( _layer );
}

void KisDoc::setBackgroundLayer( unsigned int _layer )
{
  if (m_pCurrent)
    m_pCurrent->setBackgroundLayer( _layer );
}

void KisDoc::rotateLayer180(KisLayer *_layer)
{
  if (m_pCurrent)
    m_pCurrent->rotateLayer180( _layer );
}

void KisDoc::rotateLayerLeft90(KisLayer *_layer)
{
  if (m_pCurrent)
    m_pCurrent->rotateLayerLeft90( _layer );
}

void KisDoc::rotateLayerRight90(KisLayer *_layer)
{
  if (m_pCurrent)
    m_pCurrent->rotateLayerRight90( _layer );
}

void KisDoc::mirrorLayerX(KisLayer *_layer)
{
  if (m_pCurrent)
    m_pCurrent->mirrorLayerX( _layer );
}

void KisDoc::mirrorLayerY(KisLayer *_layer)
{
  if (m_pCurrent)
    m_pCurrent->mirrorLayerY( _layer );
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

LayerList KisDoc::layerList()
{
  if (m_pCurrent)
    return m_pCurrent->layerList();
  return LayerList();
};

#include "kis_doc.moc"
