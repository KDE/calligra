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

#ifndef __KOCONTEXTHELPACTION_H__
#define __KOCONTEXTHELPACTION_H__

#include <qwidget.h>
#include <qbitmap.h>
#include <qdockwindow.h>

#include <kaction.h>

class QPixmap;
class QLabel;

class KoVerticalLabel : public QWidget
{
	Q_OBJECT
	
	public:
		KoVerticalLabel( QWidget* parent = 0, const char* name = 0 );
		~KoVerticalLabel();
		
	public slots:
		void setText( const QString& text );
		
	protected:
		void paintEvent( QPaintEvent* );
		
	private:
		QString m_text;
}; // KoVerticalLabel

class KoHelpButton : public QWidget
{
	Q_OBJECT
	
	public:
		KoHelpButton( unsigned char *bits, QWidget* parent );

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
}; // KoHelpButton

class KoHelpWidget : public QWidget
{
	Q_OBJECT
	
	public:
		KoHelpWidget( QString help, QWidget* parent );
		
		void setText( QString text );
		void timerEvent( QTimerEvent* );
		void updateButtons();

	public slots:
		void scrollUp();
		void scrollDown();
		void stopScroll();

	private:
		int           m_ypos;
		bool          m_scrollDown;
		QWidget*      m_helpViewport;
		QLabel*       m_helpLabel;
		KoHelpButton* m_upButton;
		KoHelpButton* m_downButton;
}; // KoHelpWidget

/**
 * KoContextHelpPopup is the popup displayed by ContextHelpAction.
 */
class KoContextHelpPopup : public QWidget
{
	Q_OBJECT
	
	public:
		KoContextHelpPopup( QWidget* parent = 0 );
		~KoContextHelpPopup();
	
	public slots:
		void setContextHelp( const QString& title, const QString& text, const QPixmap* icon = 0 );
  
	protected:
		virtual void mousePressEvent( QMouseEvent* );
		virtual void mouseMoveEvent( QMouseEvent* );
		virtual void resizeEvent( QResizeEvent* );
		virtual void paintEvent( QPaintEvent* );
    
	private:
		KoHelpWidget*    m_helpViewer;
		KoVerticalLabel* m_helpTitle;
		QLabel*          m_helpIcon;
		
		QPoint           m_mousePos;
}; // KoContextHelpPopup

/**
 * KoContextHelpAction provides a easy to use context help system.
 * 
 * This action displays on demand a context help in a popup.
 * The context help is set by the updateHelp slot.
 */
class KoContextHelpAction : public KToggleAction
{
	Q_OBJECT
	
	public:
		KoContextHelpAction( KActionCollection* parent, QWidget* parent = 0 );
		~KoContextHelpAction();
		
	public slots:
		void updateHelp( const QString& title, const QString& text, const QPixmap* icon = 0 );
		//void popup();
		
	private:
		KoContextHelpPopup* m_popup;
}; // KoContextHelpAction

class KoContextHelpDocker : public QDockWindow
{
	Q_OBJECT
	
	public:
		KoContextHelpDocker( QWidget* parent = 0, const char* name = 0 );
		~KoContextHelpDocker();
		
	public slots:
		void setContextHelp( const QString& title, const QString& text, const QPixmap* icon = 0 );
		
	private:
		KoHelpWidget*    m_helpViewer;
		KoVerticalLabel* m_helpTitle;
		QLabel*          m_helpIcon;
}; // KoContextHelpDocker

#endif /* __KOCONTEXTHELPACTION_H__ */
