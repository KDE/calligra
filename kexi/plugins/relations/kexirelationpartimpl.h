/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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

#ifndef KEXIRELATIONPARTIMPL_H
#define KEXIRELATIONPARTIMPL_H

#include <kgenericfactory.h>
#include <kexiinternalpart.h>

class KexiRelationPartImpl : public KexiInternalPart
{
	Q_OBJECT

	public:
		KexiRelationPartImpl(QObject *parent, const char *name, const QStringList &args);
		virtual ~KexiRelationPartImpl();
	
	protected:
		virtual QWidget *createWidget(const char* widgetClass, KexiMainWindow* mainWin, 
		 QWidget *parent, const char *objName=0);
		
		virtual KexiDialogBase *createDialog(KexiMainWindow* mainWin,
		 const char *objName=0);

		//virtual KexiDialogBase *createWindow(KexiMainWindow *parent);
		//virtual QWidget *createWidget(QWidget *parent, KexiMainWindow *win);
};

#endif


