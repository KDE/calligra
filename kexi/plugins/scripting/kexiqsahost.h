/* This file is part of the KDE project
Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/

#ifndef KEXIQSAHOST_H
#define KEXIQSAHOST_H

#include <qobject.h>
#include <qptrlist.h>

class QSProject;
class QSInterpreter;

class KexiView;
class KexiQSAEditor;
class KexiScriptHandler;

typedef QPtrList<KexiQSAEditor> Editors;

class KexiQSAHost : public QObject
{
	Q_OBJECT

	public:
		KexiQSAHost(KexiScriptHandler *parent, const char *name);
		~KexiQSAHost();

		//views
		void		createFile(KexiView *view, const QString &name);
		void		openFile(KexiView *view, const QString &file);

		//project data
		QStringList	scriptFiles() const;

		//loading saving...
		QByteArray	getProjectData();
		void		setProjectData(const QByteArray &data);

		bool		projectChanged();

		static QString	appendix();

		QStringList	globalFunctions() const;
		QSInterpreter	*interpreter() { return m_interpreter; }

	protected slots:
		void		slotDB();

	private:
		QSProject		*m_project;
		QSInterpreter		*m_interpreter;

		KexiScriptHandler	*m_parent;
		Editors			m_editors;
};

#endif
