/***************************************************************************
                          kexiformbase.h  -  description
                             -------------------
    begin                : Mon Jun 17 2002
    copyright            : (C) 2002 by lucijan busch
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
		void registerAction(KAction *action);

		void paintEvent(QPaintEvent *ev);
		void mouseMoveEvent(QMouseEvent *ev);
		void mouseReleaseEvent(QMouseEvent *ev);
		
		int	m_dotSpacing;

		bool	m_widgetRect;
	
		int	m_widgetRectBX;
		int	m_widgetRectBY;
		int	m_widgetRectEX;
		int	m_widgetRectEY;

	protected slots:
		void slotWidgetLineEdit();

};

#endif
