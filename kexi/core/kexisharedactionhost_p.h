/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXISHAREDACTIONHOST_P_H
#define KEXISHAREDACTIONHOST_P_H

#include <qobject.h>
#include <qptrdict.h>
#include <qsignalmapper.h>

#include <kaction.h>
#include <kmainwindow.h>

#include "kexiactionproxy.h"

class KexiSharedActionHost;

class KexiVolatileActionData
{
	public:
		KexiVolatileActionData() { plugged=false; }
//		KAction *kaction;
		bool plugged : 1;
};

//! internal class
class KEXICORE_EXPORT KexiSharedActionHostPrivate : public QObject
{
	Q_OBJECT

	public:
		KexiSharedActionHostPrivate(KexiSharedActionHost *h);

	public slots:
		void slotAction(const QString& act_id);

	public:
		QPtrDict<KexiActionProxy> actionProxies;
		KMainWindow *mainWin;
		KActionPtrList sharedActions;
		QSignalMapper actionMapper;
		QPtrDict<KexiVolatileActionData> volatileActions;

		KexiSharedActionHost *host;
};

#endif

