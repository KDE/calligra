/*
                          kexiformbase.h  -  description
                             -------------------
    begin                : Mon Jun 17 2002
    copyright            : (C) 2002 by lucijan busch
			   (C) 2002 by Joseph Wenninger <jowenn@kde.org>
    email                : lucijan@gmx.at

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
class KexiProject;
namespace KFormEditor {
	class WidgetContainer;
}

class KexiFormBase : public KexiDialogBase
{

	Q_OBJECT
   
	public: 
		KexiFormBase(KexiView *view, QWidget *parent=0, const char *name=0, QString identifier=QString::null);
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
		KFormEditor::WidgetContainer *topLevelEditor;
	protected slots:
		void slotWidgetLineEdit();
		void slotWidgetPushButton();
		void slotWidgetURLRequester();
		void slotWidgetFrame();
		void slotWidgetTabWidget();
		
};

#endif
