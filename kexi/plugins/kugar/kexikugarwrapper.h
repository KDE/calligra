/***************************************************************************
                          kexikugarwrapper.h  -  description
                             -------------------
    begin                : Mon Nov 22 2002
    copyright       	 : (C) 2002 by Joseph Wenninger
    email                : jowenn@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KEXIKUGARWRAPPER_H
#define KEXIKUGARWRAPPER_H

#include "kexidialogbase.h"
#include <qpixmap.h>

/**
  *@author Joseph Wenninger
  */


class KAction;
class KexiProject;

namespace KParts {
	class ReadOnlyPart;
}

class KexiKugarWrapper : public KexiDialogBase
{

	Q_OBJECT
   
	public: 
		KexiKugarWrapper(KexiView *view, QWidget *parent=0, const char *name=0, QString identifier=QString::null);
		~KexiKugarWrapper();
		
                virtual KXMLGUIClient *guiClient();
		virtual void activateActions();
		virtual void deactivateActions();

	private:
		KParts::ReadOnlyPart *m_part;
};

#endif
