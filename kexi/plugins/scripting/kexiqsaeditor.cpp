/* This file is part of the KDE project
Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

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

#include <qlayout.h>
#include <qseditor.h>

#include <klocale.h>

#include "kexiqsaeditor.h"

KexiQSAEditor::KexiQSAEditor(QSProject *project, const QString &file, KexiView *view, const char *name)
 : KexiDialogBase(view, 0, name)
{
	m_editor = new QSEditor(project, this, "embedded");
	m_editor->setSource(file);
	setFocusProxy(m_editor);

	QHBoxLayout *l = new QHBoxLayout(this);
	l->addWidget(m_editor);

	setCaption(file);
	registerAs(DocumentWindow);
}

void
KexiQSAEditor::save()
{
	m_editor->save();
}

KexiQSAEditor::~KexiQSAEditor()
{
}

#include "kexiqsaeditor.moc"
