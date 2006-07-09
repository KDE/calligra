/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIFORMEVENTHANDLER_H
#define KEXIFORMEVENTHANDLER_H

#include <qwidget.h>

class KexiMainWindow;

//! The KexiFormEventHandler class handles events defined within Kexi Forms
/*! For now only "onClickAction" property of Push Button widget is handled:
 It's possible to connect this event to predefined global action. 

 Note: This interface will be extended in the future!

 @see KexiFormPart::slotAssignAction()
 */
class KEXIFORMUTILS_EXPORT KexiFormEventHandler
{
	public:
		KexiFormEventHandler();
		virtual ~KexiFormEventHandler();

		/*! Sets \a mainWidget to be a main widget for this handler.
		 Also find widgets having action assigned and connects them 
		 to appropriate actions. 
		 For now, all of them must be KexiPushButton). 
		 \a mainWin is used to get action list. */
		void setMainWidgetForEventHandling(KexiMainWindow *mainWin, QWidget* mainWidget);

	protected:
		QWidget *m_mainWidget;
};

#endif
