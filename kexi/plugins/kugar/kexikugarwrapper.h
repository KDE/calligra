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

#ifndef KEXIKUGARWRAPPER_H
#define KEXIKUGARWRAPPER_H

#include "kexidialogbase.h"
#include <qpixmap.h>

/**
  *@author Joseph Wenninger
  */


class KAction;
class KexiProject;
class KexiKugarHandlerItem;

namespace KParts {
	class ReadOnlyPart;
}

class KexiKugarWrapper : public KexiDialogBase
{

	Q_OBJECT

	public:
		KexiKugarWrapper(KexiView *view, KexiKugarHandlerItem* ,const QString& path,
				QWidget *parent=0, const char *name=0);
		~KexiKugarWrapper();

                virtual KXMLGUIClient *guiClient();
		virtual void activateActions();
		virtual void deactivateActions();

		virtual void setupPrinter(KPrinter &printer);
		virtual void print(KPrinter &printer);
	private:
		KParts::ReadOnlyPart *m_part;
};

#endif
