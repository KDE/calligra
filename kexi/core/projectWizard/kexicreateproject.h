/***************************************************************************
                          kexicreateproject.h  -  description
                             -------------------
    begin                : Sun Jun 9 2002
    copyright            : (C) 2002 by lucijan busch, Joseph Wenninger
    email                : lucijan@gmx.at, jowenn@kde.org
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
#include <qptrlist.h>
#include <kexicreateprojectiface.h>
#include <qstringlist.h>

/**
  *@author lucijan busch
  *@author Joseph Wenninger
  */

//class KComboBox;
//class KLineEdit;
//class KListView;
//class KTextBrowser;

class KexiProject;
class KexiCreateProjectPage;

typedef QPtrList<KexiCreateProjectPage> PageList;

/*! this class aims to represent
    the dirtiest class you ever saw
 */

class KexiCreateProject : public KWizard, public KexiCreateProjectIface {
   Q_OBJECT

	public:
		KexiCreateProject(QObject *project, const char *name=0, const QStringList & = QStringList());
		~KexiCreateProject();

		/*! adds the page to the pagelist
		 *  which enables showing on demand
		*/
		void			registerPage(KexiCreateProjectPage *page);

		KexiProject *project()const ;
		int execute() {return exec();}
	protected:
		void			addItem(KexiCreateProjectPage *page, QString title, int index=-1);
		/*! adds pages, needed for a section
		 *  and removes pages, which are'n needed as well,
		 *  note: it requeries that the pages are added in the right order)
		 */
		void			requireSection(const QString &section);

		//always the same pixmap at the left
		QPixmap			*m_wpic;

		//all the pages
		KexiCreateProjectPage	*m_pageEngine;
		KexiCreateProjectPage	*m_pageLocation;
		KexiCreateProjectPage	*m_pageAuth;
		KexiCreateProjectPage	*m_pageDatabase;
		KexiCreateProjectPage	*m_pageFile;

		PageList		m_pageList;

		QString			m_currentSection;

	private:
		KexiProject *m_project;
	protected slots:
		void			slotValueChanged(KexiCreateProjectPage *, QString &);
		void			next();
		void			accept();
};

#endif
