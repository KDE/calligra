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
class KexiFormHandlerItem;
class KexiDBWidgetContainer;

namespace KFormEditor {
	class WidgetContainer;
}

class KexiFormBase : public KexiDialogBase
{

	Q_OBJECT
   
	public: 
		KexiFormBase(KexiView *view, KexiFormHandlerItem *item, QWidget *parent=0,
		 const QString &data=QString::null, const char *name=0, QString identifier=QString::null,
		 KFormEditor::WidgetContainer *content=0);
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
		KexiDBWidgetContainer *topLevelEditor;
		QString m_source;
		KexiProject *m_project;
		KexiFormHandlerItem *m_item;

	protected slots:
		void slotWidgetLabel();
		void slotWidgetLineEdit();
		void slotWidgetPushButton();
		void slotWidgetURLRequester();
		void slotWidgetFrame();
		void slotWidgetTabWidget();

		void slotToggleFormMode(bool state);

		void slotWidgetInserted(QObject *);
};

#endif
