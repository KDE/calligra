/* This file is part of the KDE project
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

#ifndef KEXIPROJECTPARTPROXY_H
#define KEXIPROJECTPARTPROXY_H

#include <qobject.h>
#include <qguardedptr.h>

#include "kexiprojecthandler.h"
#include "kexiproject.h"
#include "kexihandlerpopupmenu.h"
#include "kexiview.h"

class KEXICORE_EXPORT KexiProjectHandlerProxy : public QObject
{
	Q_OBJECT

	public:
		KexiProjectHandlerProxy(KexiProjectHandler *part, KexiView *view);
		virtual ~KexiProjectHandlerProxy() {};

		virtual KexiPartPopupMenu	*groupContext() = 0;
		virtual KexiPartPopupMenu	*itemContext(const QString &identifier) = 0;

		KexiView *kexiView()const;
		KexiProjectHandler *part()const;

	public slots:
		virtual void executeItem(const QString& identifier)=0;
	protected:
		QGuardedPtr<KexiView> m_view;
		KexiProjectHandler *m_part;

};

#endif
