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
#include <kpixmap.h>
#include <kpixmapeffect.h>
#include <kdebug.h>
#include <kapp.h>

#include "kfloatingdialog.h"

KFloatingDialog::KFloatingDialog(QWidget *parent, const char* name) : QFrame(parent, name)
{
  m_pParent = parent;
  m_shaded = false;
  m_dragging = false;
  m_resizing = false;
  m_cursor = false;

  setMouseTracking(true);
  setFrameStyle(QFrame::Panel | QFrame::Raised);
  setLineWidth(FRAMEBORDER);

  m_pCloseButton = new QPushButton(this);
  QPixmap close_pm(locate("data", "kimageshop/pics/close.xpm"));
  m_pCloseButton->setPixmap(close_pm);
  m_pCloseButton->setStyle(new QWindowsStyle);
  m_pCloseButton->setGeometry(width()-FRAMEBORDER-13, FRAMEBORDER+1, 12, 12);
  connect(m_pCloseButton, SIGNAL(clicked()), this, SLOT(slotClose()));

  m_pMinButton = new QPushButton(this);
  QPixmap min_pm(locate("data", "kimageshop/pics/minimize.xpm"));
  m_pMinButton->setPixmap(min_pm);
  m_pMinButton->setStyle(new QWindowsStyle);
  m_pMinButton->setGeometry(width()-FRAMEBORDER-26, FRAMEBORDER+1, 12, 12);
  connect(m_pMinButton, SIGNAL(clicked()), this, SLOT(slotMinimize()));

  m_pDockButton = new QPushButton(this);
  QPixmap dock_pm(locate("data", "kimageshop/pics/dock.xpm"));
  m_pDockButton->setPixmap(dock_pm);
  m_pDockButton->setStyle(new QWindowsStyle);
  m_pDockButton->setGeometry(width()-FRAMEBORDER-39, FRAMEBORDER+1, 12, 12);
  connect(m_pDockButton, SIGNAL(clicked()), this, SLOT(slotDock()));

}

KFloatingDialog::~KFloatingDialog()
{
  delete m_pCloseButton;
  delete m_pDockButton;
  delete m_pMinButton;
}

void KFloatingDialog::paintEvent(QPaintEvent *e)
{
  KPixmap pm;

  pm.resize(_width(), GRADIENT_HEIGHT);
  QRect gradient(FRAMEBORDER, FRAMEBORDER, _width(), GRADIENT_HEIGHT);

  KPixmapEffect::gradient(pm, Qt::blue, Qt::black, KPixmapEffect::HorizontalGradient);
  
  QPainter p(this);
  p.drawPixmap(FRAMEBORDER, FRAMEBORDER, pm);
 
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
		{
		  resize(width(), m_unshadedHeight);
		  m_shaded = false;
		}
      else
		{
		  m_shaded = true;
		  m_unshadedHeight = height();
		  resize(width(), TITLE_HEIGHT);
		}
    }
}

void KFloatingDialog::mousePressEvent(QMouseEvent *e)
{
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
      
      if(m_pParent)
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

void  KFloatingDialog::slotClose()
{
  hide();
}

void  KFloatingDialog::slotDock()
{
  // TODO
}

void  KFloatingDialog::slotMinimize()
{
  // TODO
}

#include "kfloatingdialog.moc"
