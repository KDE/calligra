/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXISCROLLVIEW_H
#define KEXISCROLLVIEW_H

#include <qscrollview.h>
#include <qtimer.h>

class QColor;
class QFont;
class KexiRecordNavigator;

//! The scrollview which inlcudes KexiDBForm
/*! It allows to resize its m_widget, following snapToGrid setting. 
 Its contents is resized so the widget can always be resized. */
class KEXIEXTWIDGETS_EXPORT KexiScrollView : public QScrollView
{
	Q_OBJECT

	public:
		KexiScrollView(QWidget *parent, bool preview);
		virtual ~KexiScrollView();

		void setWidget(QWidget *w);
		void setSnapToGrid(bool enable, int gridX=1, int gridY=1);

		void setResizingEnabled(bool enabled) { m_enableResizing = enabled; }
		void setRecordNavigatorVisible(bool visible);

		void setOuterAreaIndicatorVisible(bool visible)  { m_outerAreaVisible = visible; }

		void refreshContentsSizeLater(bool horizontal, bool vertical);
		void updateNavPanelGeometry();

		KexiRecordNavigator* recordNavigator() const;

	public slots:
		/*! Make sure there is a 300px margin around the form contents to allow resizing. */
		void refreshContentsSize();

	signals:
		void outerAreaClicked();
		void resizingStarted();
		void resizingEnded();

	protected:
		virtual void contentsMousePressEvent(QMouseEvent * ev);
		virtual void contentsMouseReleaseEvent(QMouseEvent * ev);
		virtual void contentsMouseMoveEvent(QMouseEvent * ev);
		virtual void drawContents( QPainter * p, int clipx, int clipy, int clipw, int cliph );
		virtual void leaveEvent( QEvent *e );
		virtual void setHBarGeometry( QScrollBar & hbar, int x, int y, int w, int h );

		bool m_resizing;
		bool m_enableResizing;
		QWidget *m_widget;

		int m_gridX, m_gridY;
		QFont m_helpFont;
		QColor m_helpColor;
		QTimer m_delayedResize;
		//! for refreshContentsSizeLater()
		QScrollView::ScrollBarMode m_vsmode, m_hsmode;
		bool m_snapToGrid : 1;
		bool m_preview : 1;
		bool m_smodeSet : 1;
		bool m_outerAreaVisible : 1;
		KexiRecordNavigator* m_navPanel;
};

#endif

