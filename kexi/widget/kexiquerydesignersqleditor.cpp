/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexiquerydesignersqleditor.h"

//uncomment this to enable Qt-only editor
//#define QT_ONLY_SQL_EDITOR

//TODO: detect if KTextEditor returned something, if not- force QT_ONLY_SQL_EDITOR option

#include <qlayout.h>
#include <qpushbutton.h>
#include <qframe.h>

#include <klocale.h>
#include <kdebug.h>

#ifdef QT_ONLY_SQL_EDITOR
# include <ktextedit.h>
#else
# include <ktexteditor/document.h>
# include <ktexteditor/view.h>
# include <ktexteditor/editorchooser.h>
# include <ktexteditor/highlightinginterface.h>
# include <ktexteditor/editinterface.h>
# include <ktexteditor/viewcursorinterface.h>
#endif

class KexiQueryDesignerSQLEditorPrivate {
	public:
		KexiQueryDesignerSQLEditorPrivate() {}
#ifdef QT_ONLY_SQL_EDITOR
		KTextEdit *view;
#else
		KTextEditor::Document	*doc;
		KTextEditor::View	*view;
#endif
};

//=====================

KexiQueryDesignerSQLEditor::KexiQueryDesignerSQLEditor(
	KexiMainWindow *mainWin, QWidget *parent, const char *name)
 : KexiViewBase(mainWin, parent, name)
 ,d(new KexiQueryDesignerSQLEditorPrivate())
{
	QVBoxLayout *lyr = new QVBoxLayout(this);
	QFrame *fr = new QFrame(this);
	fr->setFrameStyle(QFrame::Sunken|QFrame::WinPanel);// | QFrame::WinPanel);
	lyr->addWidget(fr);
	lyr = new QVBoxLayout(fr);
	lyr->setMargin( 2 );

#ifdef QT_ONLY_SQL_EDITOR
	d->view = new KTextEdit( "", QString::null, fr, "sqlDoc_editor" );
	connect(d->view, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
#else
	d->doc =  KTextEditor::EditorChooser::createDocument(fr, "sqlDoc");
	d->view = d->doc->createView(fr, 0L);
	KTextEditor::HighlightingInterface *hl = KTextEditor::highlightingInterface(d->doc);
	for(uint i=0; i < hl->hlModeCount(); i++)
	{
		if(hl->hlModeName(i) == "SQL-MySQL")
		{
			hl->setHlMode(i);
			break;
		}
		i++;
	}
	connect(d->doc, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
#endif
	setViewWidget(d->view);
	lyr->addWidget(d->view);

//	m_view->installEventFilter(this);
//	setFocusProxy(m_view);

//	QPushButton *btnQuery = new QPushButton(i18n("&Query"), this);
//	btnQuery->setFlat(true);
//	QPushButton *btnClear = new QPushButton(i18n("&Clear"), this);
//	btnClear->setFlat(true);

//	g->addWidget(btnQuery,		0, 0);
//	g->addWidget(btnClear,		0, 1);
//	g->addMultiCellWidget(m_view,	1, 1, 0, 1);
}

KexiQueryDesignerSQLEditor::~KexiQueryDesignerSQLEditor()
{
}

bool
KexiQueryDesignerSQLEditor::eventFilter(QObject *o, QEvent *ev)
{
//TODO is it needed?

	if(ev->type() == QEvent::KeyRelease)
	{
		QKeyEvent *ke = static_cast<QKeyEvent*>(ev);
		if(ke->key() == Key_Return && ke->state() == ControlButton)
		{
			kdDebug() << "KexiQueryDesignerSQLEditor::eventFilter(): magic" << endl;
			emit execQ();
			return true;
		}
	}
	else if (o==d->view && (ev->type() == QEvent::FocusIn || ev->type() == QEvent::FocusOut)) {
		emit focus(ev->type() == QEvent::FocusIn);
	}

	return false;
}

// === KexiQueryDesignerSQLEditor impelmentation using KTextEditor ===

#ifdef QT_ONLY_SQL_EDITOR
# include "kexiquerydesignersqleditor_qt.cpp"
#else

QString
KexiQueryDesignerSQLEditor::text()
{
	KTextEditor::EditInterface *eIface = KTextEditor::editInterface(d->doc);
	kdDebug() << "KexiQueryDesignerSQLEditor::getText(): iface: " << eIface << " " << eIface->text() << endl;
	return eIface->text();
}

void
KexiQueryDesignerSQLEditor::setText(const QString &text)
{
	const bool was_dirty = dirty();
	KTextEditor::EditInterface *eIface = KTextEditor::editInterface(d->doc);
	eIface->setText(text);
	setDirty(was_dirty);
}

void
KexiQueryDesignerSQLEditor::jump(int col)
{
	KTextEditor::ViewCursorInterface *ci = KTextEditor::viewCursorInterface(d->view);
	ci->setCursorPosition(0, col);
}

#endif //!QT_ONLY_SQL_EDITOR

#include "kexiquerydesignersqleditor.moc"

