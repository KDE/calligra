/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>

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

#include "kexieditor.h"

#include <keximainwindow.h>

//uncomment this to enable KTextEdit-based editor
//#define KTEXTEDIT_BASED_SQL_EDITOR

//TODO: detect if KTextEditor returned something, if not- force KTEXTEDIT_BASED_SQL_EDITOR option

#include <qlayout.h>
#include <qframe.h>

#include <klocale.h>
#include <kdebug.h>

#ifdef KTEXTEDIT_BASED_SQL_EDITOR
# include <ktextedit.h>

class KTextEdit_KexiSharedActionConnector : public KexiSharedActionConnector
{
public:
	KTextEdit_KexiSharedActionConnector( KexiActionProxy* proxy, KTextEdit *obj )
	 : KexiSharedActionConnector( proxy, obj )
	{
		plugSharedAction("edit_cut", SLOT(cut()));
		plugSharedAction("edit_copy", SLOT(copy()));
		plugSharedAction("edit_paste", SLOT(paste()));
		plugSharedAction("edit_clear", SLOT(clear()));

		plugSharedAction("edit_undo", SLOT(undo()));
		plugSharedAction("edit_redo", SLOT(redo()));
	}
};

#else
# include <ktexteditor/document.h>
# include <ktexteditor/view.h>
# include <ktexteditor/editorchooser.h>
# include <ktexteditor/editinterface.h>
# include <ktexteditor/viewcursorinterface.h>
# include <ktexteditor/popupmenuinterface.h>

class KTextEditor_View_KexiSharedActionConnector : public KexiSharedActionConnector
{
public:
	KTextEditor_View_KexiSharedActionConnector( KexiActionProxy* proxy, KTextEditor::View *obj )
	 : KexiSharedActionConnector( proxy, obj )
	{
		QValueList<QCString> actions;
		actions << "edit_cut" << "edit_copy" << "edit_paste" << "edit_clear"
			<< "edit_undo" << "edit_redo";
		plugSharedActionsToExternalGUI(actions, obj);
	}
};

#endif

class KexiEditorPrivate {
	public:
		KexiEditorPrivate()
		 : keyEventFilter_enabled(true)
		{}
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
		KTextEdit *view;
#else
		KTextEditor::Document *doc;
		KTextEditor::View *view;
#endif
		bool keyEventFilter_enabled : 1;
};

//=====================

KexiEditor::KexiEditor( KexiMainWindow *mainWin, QWidget *parent, const char *name)
 : KexiViewBase(mainWin, parent, name)
 ,d(new KexiEditorPrivate())
{
	QVBoxLayout *layout = new QVBoxLayout(this);
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
	d->view = new KTextEdit( "", QString::null, this, "kexi_editor" );
	//adjust font
	connect(d->view, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
	QFont f("Courier");
	f.setStyleStrategy(QFont::PreferAntialias);
	f.setPointSize(d->view->font().pointSize());
	d->view->setFont( f );
	//other settings
	d->view->setCheckSpellingEnabled(false);

	KTextEdit_KexiSharedActionConnector c(this, d->view);

#else
	QFrame *fr = new QFrame(this);
	fr->setFrameStyle(QFrame::Sunken|QFrame::WinPanel);
	layout->addWidget(fr);
	layout = new QVBoxLayout(fr);
	layout->setMargin( 2 );

	d->doc =  KTextEditor::EditorChooser::createDocument(fr);
	if (!d->doc)
		return;
	d->view = d->doc->createView(fr, 0L);
//	setFocusProxy(d->view);

//	f->resetContainer("codefolding");
	plugSharedActionToExternalGUI("edit_cut", d->view);
	plugSharedActionToExternalGUI("edit_copy", d->view);
	plugSharedActionToExternalGUI("edit_paste", d->view);

	plugSharedActionToExternalGUI("edit_undo", d->view);
	plugSharedActionToExternalGUI("edit_redo", d->view);

	KTextEditor::PopupMenuInterface *popupInt = dynamic_cast<KTextEditor::PopupMenuInterface*>( docView() );
        if(popupInt) {
            QPopupMenu *pop = (QPopupMenu*) mainWin->factory()->container("edit", mainWin);
            if(pop) {
                //plugSharedAction("edit_undo", pop);
                popupInt->installPopup(pop);
            }
        }

	connect(d->doc, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
#endif
	d->view->installEventFilter(this);
//	QObject::installEventFilter(this);

	layout->addWidget(d->view);
	setViewWidget(d->view, true/*focus*/);
	d->view->show();
}

KexiEditor::~KexiEditor()
{
//not needed, right?	delete d->view;
//not needed, right?	delete d->doc;
}

void
KexiEditor::updateActions(bool activated)
{
	KexiViewBase::updateActions(activated);
}

/*
void KexiEditor::installEventFilter ( const QObject * filterObj )
{
	d->view->installEventFilter( filterObj );
	QObject::installEventFilter( filterObj );
}

bool KexiEditor::eventFilter(QObject *o, QEvent *ev)
{
	if(ev->type() == QEvent::KeyPress && o==d->view) {
		QKeyEvent *ke = static_cast<QKeyEvent*>(ev);
		kdDebug() << ke->key() << endl;
	}
	return KexiViewBase::eventFilter(o,ev);
}*/


#ifdef KTEXTEDIT_BASED_SQL_EDITOR
// === KexiEditor implementation using KTextEdit only ===
QString
KexiEditor::text()
{
	return d->view->text();
}

void
KexiEditor::setText(const QString &text)
{
	const bool was_dirty = dirty();
	d->view->setText(text);
	setDirty(was_dirty);
}

void
KexiEditor::jump(int character)
{
	//find row and column for this character
	const int numRows = d->view->paragraphs();
	int row = 0, col = 0;
	for (int ch = 0; row < numRows; row++) {
		const int rowLen = d->view->paragraphLength(row)+1;
		if ((ch + rowLen) > character) {
			col = character-ch;
			break;
		}
		ch += rowLen;
	}

	d->view->setCursorPosition(row, col);
}

#else
// === KexiEditor implementation using KTextEditor ===

KTextEditor::Document*
KexiEditor::document()
{
	return d->doc;
}

KTextEditor::View*
KexiEditor::docView()
{
	return d->view;
}

QString
KexiEditor::text()
{
	if (!d->doc)
		return QString::null;
	KTextEditor::EditInterface *eIface = KTextEditor::editInterface(d->doc);
	kdDebug() << "KexiEditor::getText(): iface: " << eIface << " " << eIface->text() << endl;
	return eIface->text();
}

void
KexiEditor::setText(const QString &text)
{
	if (!d->doc)
		return;
	const bool was_dirty = dirty();
	KTextEditor::EditInterface *eIface = KTextEditor::editInterface(d->doc);
	eIface->setText(text);
	setDirty(was_dirty);
}

void
KexiEditor::jump(int character)
{
	//find row and column for this character
	if (!d->doc)
		return;
	KTextEditor::EditInterface *ei = KTextEditor::editInterface(d->doc);
	const int numRows = ei->numLines();

	int row = 0, col = 0;
	for (int ch = 0; row < numRows; row++) {
		const int rowLen = ei->lineLength(row)+1;

		if ((ch + rowLen) > character) {
			col = character-ch;
			break;
		}
		ch += rowLen;
	}

	KTextEditor::ViewCursorInterface *ci = KTextEditor::viewCursorInterface(d->view);
	ci->setCursorPositionReal(row, col);
}

#endif //!KTEXTEDIT_BASED_SQL_EDITOR


#include "kexieditor.moc"

