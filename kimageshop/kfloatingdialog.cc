/*
 *  kfloatingdialog.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <me@kde.org>
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

#include <qcursor.h>
#include <qpainter.h>
#include <qrect.h>
#include <qwindowsstyle.h>

#include <kglobal.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <kapp.h>

#include "kfloatingdialog.h"

KFloatingDialog::KFloatingDialog(QWidget *parent, const char* name) : QFrame(parent, name)
{
  m_pParent = parent;
  m_shaded = false;
  m_dragging = false;
  m_resizing = false;
  m_cursor = false;
  m_active = true;

  if (m_pParent)
    {
      m_docked = true;
      m_dockedPos = pos();
    }
  else
    {
      m_docked = false;
      m_dockedPos = QPoint(0,0);
    }

  setMouseTracking(true);
  setFrameStyle(QFrame::Panel | QFrame::Raised);
  setLineWidth(FRAMEBORDER);

  m_pCloseButton = new QPushButton(this);
  QPixmap close_pm(locate("appdata", "pics/close.png"));
  m_pCloseButton->setPixmap(close_pm);
  m_pCloseButton->setStyle(new QWindowsStyle);
  m_pCloseButton->setGeometry(width()-FRAMEBORDER-13, FRAMEBORDER+1, 12, 12);
  connect(m_pCloseButton, SIGNAL(clicked()), this, SLOT(slotClose()));

  m_pMinButton = new QPushButton(this);
  QPixmap min_pm(locate("appdata", "pics/minimize.png"));
  m_pMinButton->setPixmap(min_pm);
  m_pMinButton->setStyle(new QWindowsStyle);
  m_pMinButton->setGeometry(width()-FRAMEBORDER-26, FRAMEBORDER+1, 12, 12);
  connect(m_pMinButton, SIGNAL(clicked()), this, SLOT(slotMinimize()));

  m_pDockButton = new QPushButton(this);
  QPixmap dock_pm(locate("appdata", "pics/dock.png"));
  m_pDockButton->setPixmap(dock_pm);
  m_pDockButton->setStyle(new QWindowsStyle);
  m_pDockButton->setGeometry(width()-FRAMEBORDER-39, FRAMEBORDER+1, 12, 12);
  connect(m_pDockButton, SIGNAL(clicked()), this, SLOT(slotDock()));

   // query kwmrc for the titlebar look
  KConfig* config = new KConfig("kwmrc", true);

  config->setGroup("WM");

  m_activeBlend = config->readColorEntry("activeBlend" , &(Qt::black));
  m_inactiveBlend = config->readColorEntry("inactiveBlend" , &palette().normal().background());

  config->setGroup("General");

  QString key = config->readEntry("TitlebarLook");

  m_titleLook = gradient;
  m_gradientType = KPixmapEffect::HorizontalGradient;

  if( key == "shadedHorizontal")
    m_gradientType = KPixmapEffect::HorizontalGradient;
  else if( key == "shadedVertical")
    m_gradientType = KPixmapEffect::VerticalGradient;
  else if( key == "shadedDiagonal")
    m_gradientType = KPixmapEffect::DiagonalGradient;
  else if( key == "shadedCrossDiagonal")
    m_gradientType = KPixmapEffect::CrossDiagonalGradient;
  else if( key == "shadedRectangle")
    m_gradientType = KPixmapEffect::RectangleGradient;
  else if( key == "shadedElliptic")
    m_gradientType = KPixmapEffect::EllipticGradient;
  else if( key == "shadedPyramid")
    m_gradientType = KPixmapEffect::PyramidGradient;
  else if( key == "shadedPipeCross")
    m_gradientType = KPixmapEffect::PipeCrossGradient;
  else if( key == "plain")
    m_titleLook = plain;
  else if( key == "pixmap")
    m_titleLook = pixmap;

  if (m_titleLook == pixmap )
    {
      m_pActivePm = new QPixmap;
      m_pInactivePm = new QPixmap;

      KIconLoader* iconLoader = new KIconLoader(0, "kwm");

      *(m_pActivePm) = iconLoader->reloadIcon("activetitlebar.xpm");
      *(m_pInactivePm) = iconLoader->reloadIcon("inactivetitlebar.xpm");

      if (m_pInactivePm->size() == QSize(0,0))
	*m_pInactivePm = *m_pActivePm;

      if (m_pActivePm->size() == QSize(0,0))
	m_titleLook = plain;
    }
}

KFloatingDialog::~KFloatingDialog()
{
  delete m_pCloseButton;
  delete m_pDockButton;
  delete m_pMinButton;
}

void KFloatingDialog::paintEvent(QPaintEvent *e)
{
  if (!isVisible())
    return;

  QRect r(FRAMEBORDER, FRAMEBORDER, _width(), GRADIENT_HEIGHT);

  QPainter p;
  p.begin(this);

  p.setClipRect(r);
  p.setClipping(true);

  // pixmap
 if (m_titleLook == pixmap)
   {
     QPixmap *pm = m_active ? m_pActivePm : m_pInactivePm;

     for (int x = r.x(); x < r.x() + r.width(); x+=pm->width())
       p.drawPixmap(x, r.y(), *pm);
   }
 // gradient
 else if (m_titleLook == gradient)
   {
     QPixmap* pm = 0;

     if (m_active)
       {
	 if (m_activeShadePm.size() != r.size())
	   {
	     m_activeShadePm.resize(r.width(), r.height());
	     KPixmapEffect::gradient(m_activeShadePm, kapp->activeTitleColor(),
				     m_activeBlend, m_gradientType);
	   }
	 pm = &m_activeShadePm;
       }
     else
       {
	 if (m_inactiveShadePm.size() != r.size())
	   {
	     m_inactiveShadePm.resize(r.width(), r.height());
	     KPixmapEffect::gradient(m_inactiveShadePm, kapp->inactiveTitleColor(),
				     m_inactiveBlend, m_gradientType);
	   }
	 pm = &m_inactiveShadePm;
       }
     p.drawPixmap(r.x(), r.y(), *pm);
   }
 // plain
 else
   {
     p.setBackgroundColor(m_active ? kapp->activeTitleColor()
			  : kapp->inactiveTitleColor());
     p.eraseRect(r);
   }

  p.setClipping(false);
  p.end();
  QFrame::paintEvent(e);
}

void KFloatingDialog::mouseDoubleClickEvent (QMouseEvent *e)
{
  if (e->button() & LeftButton)
    {
      QRect title(0,0, width(), TITLE_HEIGHT);
      if(!title.contains(e->pos()))
		return;

      if (m_shaded)
	setShaded(false);
      else
	setShaded(true);
    }
}

void KFloatingDialog::mousePressEvent(QMouseEvent *e)
{
  setActive(true);
  raise();

  if(!m_docked)
      setActiveWindow();

  QPoint pos = e->pos();

  if (e->button() & LeftButton)
	{
	  QRect title(0, 0, width(), TITLE_HEIGHT);

	  if(bottomRect().contains(pos))
		{
		  m_resizing = true;
		  m_resizeMode = vertical;
		}
	  else if(rightRect().contains(pos))
		{
		  m_resizing = true;
		  m_resizeMode = horizontal;
		}
	  else if(lowerRightRect().contains(pos))
		{
		  m_resizing = true;
		  m_resizeMode = diagonal;
		}
	  else if(title.contains(pos))
	      m_dragging = true;
		
	  if(m_resizing || m_dragging)
		m_start = e->globalPos();
	}
}

void KFloatingDialog::mouseMoveEvent(QMouseEvent *e)
{
  if (m_dragging)
    {
      QPoint dist = m_start - e->globalPos();
      QPoint newPos = pos() - dist;
	
      if (newPos.x() < 0)
		newPos.setX(0);
	
      if (newPos.y() < 0)
		newPos.setY(0);

      if(m_pParent && m_docked)
		{
		  if (newPos.x() + width() > m_pParent->width())
			newPos.setX(m_pParent->width() - width());
		
		  if (newPos.y()+ height() > m_pParent->height())
			newPos.setY(m_pParent->height() - height());
		}
	  move(newPos);
	  m_start = e->globalPos();
    }
  else if (m_resizing)
    {
      QPoint dist = m_start - e->globalPos();

	  if (m_resizeMode == vertical)
		dist.setX(0);
	  else if (m_resizeMode == horizontal)
		dist.setY(0);

	  QPoint newSize = QPoint(width(), height()) - dist;

	  if (newSize.x() < MIN_WIDTH)
		newSize.setX(MIN_WIDTH);

	  if (newSize.y() < MIN_HEIGHT)
		newSize.setY(MIN_HEIGHT);

      if(m_shaded)
		newSize.setY(height());

      resize(newSize.x(), newSize.y());
      m_start = e->globalPos();
    }
				
  if (bottomRect().contains(e->pos()) && !m_shaded)
	{
	  if (!QApplication::overrideCursor() || QApplication::overrideCursor()->shape() != SizeVerCursor)
		{
		  if (m_cursor)
			QApplication::restoreOverrideCursor();
		  QApplication::setOverrideCursor(sizeVerCursor);
		}
	  m_cursor = true;
	}
  else if (rightRect().contains(e->pos()))
	{
	  if (!QApplication::overrideCursor() || QApplication::overrideCursor()->shape() != SizeHorCursor)
		{
		  if (m_cursor)
			QApplication::restoreOverrideCursor();
		  QApplication::setOverrideCursor(sizeHorCursor);
		}
	  m_cursor = true;
	}
  else if (lowerRightRect().contains(e->pos()) && !m_shaded)
	{
	  if (!QApplication::overrideCursor() || QApplication::overrideCursor()->shape() != SizeFDiagCursor)
		{
		  if (m_cursor)
			QApplication::restoreOverrideCursor();
		  QApplication::setOverrideCursor(sizeFDiagCursor);
	}
	  m_cursor = true;
	}
  else if (m_cursor)
	{
	  QApplication::restoreOverrideCursor();
	  m_cursor = false;
	}
}

void KFloatingDialog::mouseReleaseEvent(QMouseEvent *e)
{
  if (e->button() & LeftButton)
    {
      m_dragging = false;
      m_resizing = false;
    }
}

void KFloatingDialog::resizeEvent(QResizeEvent *e)
{
  m_pCloseButton->setGeometry(width()-FRAMEBORDER-13, FRAMEBORDER+1, 12, 12);
  m_pMinButton->setGeometry(width()-FRAMEBORDER-26, FRAMEBORDER+1, 12, 12);
  m_pDockButton->setGeometry(width()-FRAMEBORDER-39, FRAMEBORDER+1, 12, 12);
  kdebug(KDEBUG_INFO, 0, "width: %i, height: %i", width(), height());
}

void  KFloatingDialog::leaveEvent(QEvent *)
{
  if (m_cursor)
	{
	  m_cursor = false;
	  QApplication::restoreOverrideCursor();
	}
}

void KFloatingDialog::slotClose()
{
  hide();
}

void KFloatingDialog::slotDock()
{
  if (m_docked) // docked -> undock
    setDocked(false);
  else // undocked -> dock
    setDocked(true);	
}

void KFloatingDialog::slotMinimize()
{
  if (!m_docked)
	showMinimized(); // hmn...does not work
}

void KFloatingDialog::setActive(bool value)
{
  if (m_active == value)
    return;

  m_active = value;
  if (m_active)
    emit sigActivated();
  repaint();
}

void KFloatingDialog::setShaded(bool value)
{
  if (m_shaded == value)
    return;

  m_shaded = value;

  if (m_shaded)
    {
      m_unshadedHeight = height();
      resize(width(), TITLE_HEIGHT);
    }
  else
      resize(width(), m_unshadedHeight);
}

void KFloatingDialog::setDocked(bool value)
{
  if (m_docked == value)
    return;

  m_docked = value;

  if (m_docked) // dock
    {
      if (!m_pParent)
	{
	  m_docked = false;
	  return;
	}
      reparent(m_pParent, 0, m_dockedPos, true);
    }
  else // undock
    {
      m_dockedPos = pos();
      reparent(0, WStyle_Customize | WStyle_NoBorder, mapToGlobal(QPoint(0,0)), true);
      setActiveWindow();
    }
}

#include "kfloatingdialog.moc"
