/* This file is part of the KDE libraries
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

#ifndef KFE_RESIZEHANDLER_H
#define KFE_RESIZEHANDLER_H

#include <qwidget.h>
#include <qguardedptr.h>

/**
  *@author Joseph Wenninger
  */



namespace KFormDesigner
{
	/**
	 * a single widget which represents a dot for resizing a widget
	 * @author Joseph Wenninger
	 */
	class KFORMEDITOR_EXPORT ResizeHandle : public QWidget
	{
	Q_OBJECT
	public:
		enum HandlePos {TopLeft=0,TopCenter=2,TopRight=4,LeftCenter=8,RightCenter=16,BottomLeft=32,BottomCenter=64,BottomRight=128};
	        ResizeHandle(QWidget *parent,QWidget *buddy, HandlePos pos, bool editing=false);
	        virtual ~ResizeHandle();

	protected:
        	        void mousePressEvent(QMouseEvent *ev);
                	void mouseMoveEvent(QMouseEvent *ev);
	                void mouseReleaseEvent(QMouseEvent *ev);
	protected slots:
		bool eventFilter(QObject *obj, QEvent *ev);
		void updatePos();

	private:
		HandlePos m_pos;
		QWidget *m_buddy;
		bool m_dragging;
		bool m_editing;
		int m_x;
		int m_y;
	};

	/**
	 * a set of resize handles (for resizing widgets)
	 * @author Joseph Wenninger
	 */
	class KFORMEDITOR_EXPORT ResizeHandleSet: public QObject
	{
	Q_OBJECT
	public:
		ResizeHandleSet(QWidget *modify, bool editing = false);
		~ResizeHandleSet();
		void setWidget(QWidget *modify);
		QWidget *widget() const {return m_widget;};
	private:
		QGuardedPtr<ResizeHandle> handles[8];
		QGuardedPtr<QWidget> m_widget;
	};

}

#endif
