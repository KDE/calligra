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

//class KComboBox;
//class KLineEdit;
//class KListView;
//class KTextBrowser;

class KexiCreateProjectPage;

/*! this class aims to represent
    the dirtiest class you ever saw
 */

class KexiCreateProject : public KWizard  {
   Q_OBJECT

	public: 
		KexiCreateProject(QWidget *parent=0, const char *name=0, bool modal=false, WFlags f=0);
		~KexiCreateProject();

		
	protected:
		void			addItem(KexiCreateProjectPage *page, QString title);

		//always the same pixmap at the left
		QPixmap			*m_wpic;

		//all the pages
		KexiCreateProjectPage	*m_pageEngine;
		KexiCreateProjectPage	*m_pageConnection;
		KexiCreateProjectPage	*m_pageDatabase;

	protected slots:
		void			slotValueChanged(KexiCreateProjectPage *, QString &);
		void			next();
		void			accept();
};

#endif
