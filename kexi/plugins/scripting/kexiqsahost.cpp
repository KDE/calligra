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

#include <qvaluelist.h>
#include <qvariant.h>

#include <qsinterpreter.h>
#include <qsproject.h>

#include <kdebug.h>

#include <koApplication.h>

#include "kexiqsahost.h"
#include "kexiqsaeditor.h"
#include "kexiview.h"
#include "kexiscripthandler.h"

KexiQSAHost::KexiQSAHost(KexiScriptHandler *parent, const char *name)
 : QObject(parent, name)
{
	m_parent = parent;

	m_project = QSInterpreter::defaultInterpreter()->project();
//	m_project->open(QByteArray());
//	m_project->open("/home/luci/db/kexi.qsa");
	m_interpreter = QSInterpreter::defaultInterpreter();
//	m_interpreter->setProject(m_project);

//	m_interpreter->call("helloWorld", QValueList<QVariant>());
}

void
KexiQSAHost::createFile(KexiView *view, const QString &name)
{
	KexiQSAEditor *editorView = new KexiQSAEditor(m_project, name, view, "editor");
	editorView->show();
	m_editors.append(editorView);
}

void
KexiQSAHost::openFile(KexiView *view, const QString &name)
{
	KexiQSAEditor *editorView = new KexiQSAEditor(m_project, name, view, "editor");
	editorView->show();
	m_editors.append(editorView);
}

QStringList
KexiQSAHost::scriptFiles() const
{
	kdDebug() << "KexiQSAHost::scriptFiles()" << endl;

	return m_project->scriptFiles();
}

bool
KexiQSAHost::projectChanged()
{
	for(KexiQSAEditor *it = m_editors.first(); it; it = m_editors.next())
	{
		if(it->changed())
			return true;
	}
}


//	m_project->open(QByteArray());

QByteArray
KexiQSAHost::getProjectData()
{
	for(KexiQSAEditor *it = m_editors.first(); it; it = m_editors.next())
	{
		it->save();
	}

	return m_project->projectData();
}

void
KexiQSAHost::setProjectData(const QByteArray &data)
{
	m_project->open(data, "kexi");
}


QString
KexiQSAHost::appendix()
{
	return QString(".qs");
}

QStringList
KexiQSAHost::globalFunctions() const
{
	return m_interpreter->globalFunctions();
}

KexiQSAHost::~KexiQSAHost()
{
}

#include "kexiqsahost.moc"
