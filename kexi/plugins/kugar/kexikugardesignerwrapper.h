/***************************************************************************
                          kexikugardesignerwrapper.h  -  description
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

namespace KParts {
	class ReadOnlyPart;
}

class KexiKugarDesignerWrapper : public KexiDialogBase
{

	Q_OBJECT
   
	public: 
		KexiKugarDesignerWrapper(KexiView *view, QWidget *parent=0, const char *name=0, QString identifier=QString::null, bool newrep=false);
		~KexiKugarDesignerWrapper();
		
                virtual KXMLGUIClient *guiClient();
		virtual void activateActions();
		virtual void deactivateActions();

	private:
		KoDocument *m_doc; //kuDoc
		KoView *m_view;
};

#endif
