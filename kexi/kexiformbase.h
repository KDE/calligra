/***************************************************************************
                          kexiformbase.h  -  description
                             -------------------
    begin                : Mon Jun 17 2002
    copyright            : (C) 2002 by lucijan busch
			   (C) 2002 by Joseph Wenninger <jowenn@kde.org>
    email                : lucijan@gmx.at
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KEXIFORMBASE_H
#define KEXIFORMBASE_H

#include "kexidialogbase.h"

/**
  *@author Joseph Wenninger
  *@author lucijan busch
  */

template <class type> class QPtrList;

class KAction;
  
class KexiFormBase : public KexiDialogBase
{

	Q_OBJECT
   
	public: 
		KexiFormBase(QWidget *parent=0, const char *name=0, QString datasource=0);
		~KexiFormBase();
		
		void setActions(QPtrList<KAction> *actions);
		void unregisterActions(QPtrList<KAction> *actions);

	protected:
		void initActions();
		void registerAction(KAction *action);

		void paintEvent(QPaintEvent *ev);
		void mouseMoveEvent(QMouseEvent *ev);
		void mouseReleaseEvent(QMouseEvent *ev);
		void resizeEvent(QResizeEvent *ev);
		
		void insertWidget(QWidget *widget, int x, int y, int w, int h);
		void installEventFilterRecursive(QObject *obj);


		
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

	protected slots:
		void slotWidgetLineEdit();
		void slotWidgetPushButton();
		void slotWidgetURLRequester();
		
		bool eventFilter(QObject *obj, QEvent *ev);

};

#endif
