/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#ifndef KEXIDBCONNECTIONWIDGET_H
#define KEXIDBCONNECTIONWIDGET_H

#include "kexidbconnectionwidgetbase.h"

#include <kexiprojectdata.h>

#include <ktabwidget.h>
#include <kdialogbase.h>

class KTextEdit;
class KPushButton;
class KexiDBDriverComboBox;
class KexiDBConnectionWidgetDetailsBase;

class KEXIEXTWIDGETS_EXPORT KexiDBConnectionWidget : public KexiDBConnectionWidgetBase
{
	Q_OBJECT

	public:
		KexiDBConnectionWidget( QWidget* parent = 0, const char* name = 0 );
		virtual ~KexiDBConnectionWidget();

		/*! Sets connection data \a data.
		 \a shortcutFileName is only used to check if the file is writable 
		 (if no, "save changes" button will be disabled). */
		void setData(const KexiProjectData& data, const QString& shortcutFileName = QString::null);
		KexiProjectData data();

		//! \returns a pointer to 'save changes' button. You can call hide() for this to hide it.
		KPushButton* saveChangesButton() const;

	signals:
		//! emitted when data saving is needed
		void saveChanges();

	protected slots:
		void slotLocationBGrpClicked(int id);
		void slotCBToggled(bool on);

		virtual void languageChange() { KexiDBConnectionWidgetBase::languageChange(); }

	protected:
		KexiProjectData m_data;
		KexiDBDriverComboBox *driversCombo;

		class Private;
		Private *d;

		friend class KexiDBConnectionTabWidget;
};

class KEXIEXTWIDGETS_EXPORT KexiDBConnectionTabWidget : public KTabWidget
{
	Q_OBJECT

	public:
		KexiDBConnectionTabWidget( QWidget* parent = 0, const char* name = 0 );
		virtual ~KexiDBConnectionTabWidget();

		/*! Sets connection data \a data.
		 \a shortcutFileName is only used to check if the file is writable 
		 (if no, "save changes" button will be disabled). */
		void setData(const KexiProjectData& data, const QString& shortcutFileName = QString::null);
		KexiProjectData currentData();

		//! \return true if 'save password' option is selected
		bool savePasswordSelected() const;

		KexiDBConnectionWidget *mainWidget;
		KexiDBConnectionWidgetDetailsBase* detailsWidget;

	protected slots:

	protected:
};

class KEXIEXTWIDGETS_EXPORT KexiDBConnectionDialog : public KDialogBase
{
	Q_OBJECT

	public:
		/*! Creates connection dialog for \a data.
		 \a shortcutFileName is only used to check if the file is writable 
		 (if no, "save changes" button will be disabled). 
		 If \a shortcutFileName is empty, the button will be hidden. */
		KexiDBConnectionDialog(const KexiProjectData& data, const QString& shortcutFileName = QString::null);
		~KexiDBConnectionDialog();

		KexiProjectData currentData() const { return tabWidget->currentData(); }

		//! \return true if 'save password' option is selected
		bool savePasswordSelected() const { return tabWidget->savePasswordSelected(); }

		KexiDBConnectionTabWidget *tabWidget;
};

#endif // KEXIDBCONNECTIONWIDGET_H

