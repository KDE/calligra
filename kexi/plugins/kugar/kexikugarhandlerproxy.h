/*  This file is part of the KDE project
    Copyright (C) 2002,2003 Joseph Wenninger <jowenn@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Library General Public License version 2 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef KEXIKUGARHANDLERPROXY_H
#define KEXIKUGARHANDLERPROXY_H

class QPixmap;

#include "kexikugarhandler.h"
#include "kexiprojecthandlerproxy.h"

class KexiView;

class KexiKugarHandlerProxy : public KexiProjectHandlerProxy, public KXMLGUIClient
{
	Q_OBJECT

	public:
		KexiKugarHandlerProxy(KexiKugarHandler *part, KexiView*view);

//                virtual KexiPartPopupMenu       *groupContext();
//                virtual KexiPartPopupMenu       *itemContext(const QString& identifier);



        public slots:
		virtual bool executeItem(const QString& identifier);
				void slotCreateReport();
				void slotEdit(const QString& identifier);
				void slotDelete(const QString& identifier);
				void slotGeneratorLicense();

	private:
		KexiKugarHandler *m_kugarManager;
};

#endif
