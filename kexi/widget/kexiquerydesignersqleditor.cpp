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

#include <qlayout.h>
#include <qpushbutton.h>

#include <klocale.h>
#include <kdebug.h>
#include <ktexteditor/document.h>
#include <ktexteditor/view.h>
#include <ktexteditor/editorchooser.h>
#include <ktexteditor/highlightinginterface.h>
#include <ktexteditor/editinterface.h>

#include <ktextedit.h>//TEMP

#include "kexiquerydesignersqleditor.h"

KexiQueryDesignerSQLEditor::KexiQueryDesignerSQLEditor(QWidget *parent, const char *name)
 : QWidget(parent, name)
#ifndef Q_WS_WIN //(TEMP)
	, m_doc( KTextEditor::EditorChooser::createDocument(this, "sqlDoc") )
	, m_view( m_doc->createView(this, 0L) )
#else
	, m_editor( new  KTextEdit( "", QString::null, this, "sqlDoc_editor" ) )
#endif
{
#ifndef Q_WS_WIN //(TEMP)
	KTextEditor::HighlightingInterface *hl = KTextEditor::highlightingInterface(m_doc);
	for(uint i=0; i < hl->hlModeCount(); i++)
	{
		if(hl->hlModeName(i) == "SQL-MySQL")
		{
			hl->setHlMode(i);
			break;
		}
		i++;
	}
#endif
//	QPushButton *btnQuery = new QPushButton(i18n("&Query"), this);
//	btnQuery->setFlat(true);
//	QPushButton *btnClear = new QPushButton(i18n("&Clear"), this);
//	btnClear->setFlat(true);

	QGridLayout *g = new QGridLayout(this);
//	g->addWidget(btnQuery,		0, 0);
//	g->addWidget(btnClear,		0, 1);
//	g->addMultiCellWidget(m_view,	1, 1, 0, 1);
#ifndef Q_WS_WIN //(TEMP)
	g->addWidget(m_view,		0, 0);
#else
	g->addWidget(m_editor,		0, 0);
#endif
}

QString
KexiQueryDesignerSQLEditor::getText()
{
#ifndef Q_WS_WIN //(TEMP)
	KTextEditor::EditInterface *eIface = KTextEditor::editInterface(m_doc);
	kdDebug() << "KexiQueryDesignerSQLEditor::getText(): iface: " << eIface << " " << eIface->text() << endl;
	return eIface->text();
#else
	return m_editor->text();
#endif
}

KexiQueryDesignerSQLEditor::~KexiQueryDesignerSQLEditor()
{
}

#include "kexiquerydesignersqleditor.moc"
