/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

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

#ifndef _VCONTEXTHELPDOCKER_H_
#define _VCONTEXTHELPDOCKER_H_

#include <qframe.h>
#include <qbitmap.h>
#include <klocale.h>

#include "vdocker.h"
#include "vgradient.h"

class QLabel;
class VTool;
class KarbonView;

class VHelpButton : public QWidget
{
	Q_OBJECT
	
	public:
		VHelpButton( unsigned char *bits, QWidget* parent );

	signals:
		void pressed();
		void released();

	protected:
		void paintEvent( QPaintEvent* );
		void enterEvent( QEvent* );
		void leaveEvent( QEvent* );

	private:
		QBitmap      m_bitmap;
		bool         m_pressed;
}; // VHelpButton

class VHelpWidget : public QFrame
{
	Q_OBJECT
	
	public:
		VHelpWidget( QString help, QWidget* parent );
		
		void setText( QString text );
		void timerEvent( QTimerEvent* );
		void updateButtons();

	public slots:
		void scrollUp();
		void scrollDown();
		void stopScroll();

	private:
		int           ypos;
		bool          m_scrollDown;
		QWidget*      m_helpViewport;
		QLabel*       m_helpLabel;
		VHelpButton*  m_upButton;
		VHelpButton*  m_downButton;
}; // VHelpWidget

class VContextHelpDocker : public VDocker
{
	Q_OBJECT
	
	public:
		VContextHelpDocker( KarbonView* view );
		~VContextHelpDocker();

		void manageTool( VTool* tool );
		
	private:
		VHelpWidget* m_helpWidget;
}; // VContextHelpDocker

#endif /* _VCONTEXTHELPDOCKER_H_ */
