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
#include <qpixmap.h>

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
		KexiFormBase(QWidget *parent=0, const char *name=0, QString identifier=QString::null);
		~KexiFormBase();
		
                virtual KXMLGUIClient *guiClient();
		virtual void activateActions();
		virtual void deactivateActions();

	private:
		class EditGUIClient;
		friend class EditGUIClient;
		static EditGUIClient *m_editGUIClient;

		class ViewGUIClient;
		friend class ViewGUIClient;
		static ViewGUIClient *m_viewGUIClient;
	protected slots:
		void slotWidgetLineEdit();
		void slotWidgetPushButton();
		void slotWidgetURLRequester();
		
};

#endif
