/***************************************************************************
                          kexicreateproject.h  -  description
                             -------------------
    begin                : Sun Jun 9 2002
    copyright            : (C) 2002 by lucijan busch
    email                : lucijan@gmx.at
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KEXICREATEPROJECT_H
#define KEXICREATEPROJECT_H

#include <kwizard.h>
#include <qpixmap.h>

/**
  *@author lucijan busch
  */

class KComboBox;
class KLineEdit;
class KListView;
  
class KexiCreateProject : public KWizard  {
   Q_OBJECT

	public: 
		KexiCreateProject(QWidget *parent=0, const char *name=0, bool modal=false, WFlags f=0);
		~KexiCreateProject();

		
	protected:
		QWidget *generatePage0();
		QWidget *generatePage1();
		QWidget *generatePage2();
		
		bool			m_engineLoaded;
		QString			m_loadedEngine;
		
		QWidget			*m_page1;
		QWidget			*m_page2;
		KComboBox		*m_cEngine;
		
		KLineEdit		*m_dbName;
		KLineEdit		*m_dbHost;
		KLineEdit		*m_dbUser;
		KLineEdit		*m_dbPass;
		
		KListView		*m_connectionLog;
		
		QPixmap			m_wpic;
		
	protected slots:
		void		engineSelectionChanged(const QString &engineName);
		void		nextClicked(const QString &pageTitle);

};

#endif
