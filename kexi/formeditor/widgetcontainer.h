/* This file is part of the KDE libraries
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002 Joseph Wenninger <jowenn@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KFE_WIDGETCONTAINER_H
#define KFE_WIDGETCONTAINER_H

#include <qpixmap.h>

/**
  *@author Joseph Wenninger
  *@author lucijan busch
  */



namespace KFormEditor
{

	class ResizeHandleSet;

	class WidgetContainer : public QWidget
	{

		Q_OBJECT
 
		public: 
			WidgetContainer(QWidget *parent=0, const char *name=0, QString identifier=QString::null);
			~WidgetContainer();
		
			static int dotSpacing();
		protected:
			void paintEvent(QPaintEvent *ev);
			void mouseMoveEvent(QMouseEvent *ev);
			void mouseReleaseEvent(QMouseEvent *ev);
			void resizeEvent(QResizeEvent *ev);
			
			void insertWidget(QWidget *widget, int x, int y, int w, int h);
			void installEventFilterRecursive(QObject *obj);

			void setResizeHandles(QWidget *m_activeWidget);
			void activateWidget(QWidget *widget);
		
			QWidget	*m_pendingWidget;
		
			QPixmap	m_dotBg;
		
			int	m_dotSpacing;

			bool	m_widgetRectRequested;
			bool	m_widgetRect;
	
			int	m_widgetRectBX;
			int	m_widgetRectBY;
			int	m_widgetRectEX;
			int	m_widgetRectEY;

			int m_moveBX;
			int m_moveBY;

			QWidget *m_activeWidget;
			QWidget *m_activeMoveWidget;
			ResizeHandleSet *m_resizeHandleSet;

		protected slots:
			void slotWidgetLineEdit();
			void slotWidgetPushButton();
			void slotWidgetURLRequester();
		
			bool eventFilter(QObject *obj, QEvent *ev);

	};

};
#endif
