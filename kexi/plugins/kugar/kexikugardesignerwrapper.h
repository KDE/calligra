/*  This file is part of the KDE project
    Copyright (C) 2002,2003 Joseph Wenninger <jowenn@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef KEXIKUGARDESIGNERWRAPPER_H
#define KEXIKUGARDESIGNERWRAPPER_H

#include "kexidialogbase.h"
#include <qpixmap.h>

/**
  *@author Joseph Wenninger
  */


class KAction;
class KexiProject;
class KoDocument;
class KoView;
class KexiKugarHandlerItem;

namespace KParts {
	class ReadOnlyPart;
}

class KexiKugarDesignerWrapper : public KexiDialogBase
{

	Q_OBJECT
   
	public: 
		KexiKugarDesignerWrapper(KexiView *view, QWidget *parent=0, const char *name=0, KexiKugarHandlerItem *item=0, bool newrep=false);
		~KexiKugarDesignerWrapper();
		
                virtual KXMLGUIClient *guiClient();
		virtual void activateActions();
		virtual void deactivateActions();
		bool initFailed();
	public slots:
                void getPath(QString &path);
	private:
		KoView *m_view;
		KexiKugarHandlerItem *m_item;
		bool m_initFailed;
};

#endif
