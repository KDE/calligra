/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXITABLEPART_H
#define KEXITABLEPART_H

#include <kexipart.h>
#include <kexipartitem.h>

class KexiMainWin;

class KexiTablePart : public KexiPart::Part
{
	Q_OBJECT

	public:
		KexiTablePart(QObject *parent, const char *name, const QStringList &);
		~KexiTablePart();

		virtual bool remove(KexiMainWindow *win, const KexiPart::Item &item);

//		virtual void execute(KexiMainWindow *win, const KexiPart::Item &);
//moved to Part:		virtual void createGUIClient(KexiMainWindow *win);

//		virtual QString instanceName() const;
	protected:
		virtual KexiDialogBase* createInstance(KexiMainWindow *win, const KexiPart::Item &item, bool designMode = false);

		virtual void initPartActions( KActionCollection *col );
		virtual void initInstanceActions( KActionCollection *col );
};

#endif

