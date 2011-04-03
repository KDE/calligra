/**  This file is part of the KDE project
 * 
 *  Copyright (C) 2011 Adam Pigg <adam@piggz.co.uk>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef KEXIMOBILEWIDHET_H
#define KEXIMOBILEWIDHET_H

#include <QStackedWidget>
#include <core/kexiproject.h>

class KexiMobileOpenFileWidget;
class KexiMobileNavigator;
class KexiProject;

class KexiMobileWidget : public QStackedWidget
{
	Q_OBJECT
public:
	KexiMobileWidget(KexiProject *p);
	virtual ~KexiMobileWidget();
	void databaseOpened(KexiProject*);
	
        KexiMobileNavigator *navigator();
        void setActiveObject(KexiWindow* win);
        KexiWindow* activeObject();
        
public slots:
        void showNavigator();

private:
	KexiProject *m_project;
	
	KexiMobileNavigator *m_navWidget;
        KexiWindow* m_objectPage;
};

#endif
