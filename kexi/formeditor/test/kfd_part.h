/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#ifndef KFORMDESIGNER_PART_H
#define KFORMDESIGNER_PART_H

#include <kparts/part.h>
#include <kparts/factory.h>

class KAboutData;
class KInstance;
class QWorkspace;

namespace KFormDesigner {
	class FormManager;
	class Form;
}

using KFormDesigner::Form;

class KFORMEDITOR_EXPORT KFDFactory : public KParts::Factory
{
	Q_OBJECT

	public:
		KFDFactory();
		virtual ~KFDFactory();

		virtual KParts::Part* createPartObject(QWidget *parentWidget=0, const char *widgetName=0, QObject *parent=0, const char *name=0,
		  const char *classname="KParts::Part", const QStringList &args=QStringList());

		static KInstance *instance();
		static KAboutData *aboutData();

	private:
		static KInstance *m_instance;
};

class KFORMEDITOR_EXPORT KFormDesignerPart: public KParts::ReadWritePart
{
	Q_OBJECT

	public:
		KFormDesignerPart(QWidget *parent, const char *name, bool readOnly=true);
		virtual ~KFormDesignerPart();

	public slots:
		void slotPreviewForm();
		//void slotCreateFormSlot(Form *form, const QString &widget, const QString &signal);

	protected slots:
		void slotWidgetSelected(Form *form, bool multiple);
		void slotFormWidgetSelected(Form *form);
		void slotNoFormSelected();
		void slotFormModified();

	protected:
		virtual bool openFile();
		virtual bool saveFile();
		void disableWidgetActions();
		void enableFormActions();
		void setupActions();

	private:
		KFormDesigner::FormManager  *m_manager;
		QWorkspace  *m_workspace;
};

#endif

