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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexieditor.h"

#include <keximainwindow.h>

#include <qlayout.h>
#include <qframe.h>
#include <klocale.h>
#include <kdebug.h>

//uncomment this to enable KTextEdit-based editor
//#define KTEXTEDIT_BASED_SQL_EDITOR

#ifdef KTEXTEDIT_BASED_SQL_EDITOR
# include <ktextedit.h>
#else
# include <ktexteditor/document.h>
# include <ktexteditor/view.h>
# include <ktexteditor/editorchooser.h>
# include <ktexteditor/editinterface.h>
# include <ktexteditor/viewcursorinterface.h>
# include <ktexteditor/popupmenuinterface.h>
# include <ktexteditor/undointerface.h>
# include <ktexteditor/configinterface.h>
# include <ktexteditor/highlightinginterface.h>
#endif

/** Used for the shared action framework to redirect shared actions like
copy and paste to the editor. */
class KexiEditorSharedActionConnector : public KexiSharedActionConnector
{
public:
	KexiEditorSharedActionConnector( KexiActionProxy* proxy, QObject* obj )
		: KexiSharedActionConnector( proxy, obj )
	{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
		plugSharedAction("edit_cut", SLOT(cut()));
		plugSharedAction("edit_copy", SLOT(copy()));
		plugSharedAction("edit_paste", SLOT(paste()));
		plugSharedAction("edit_clear", SLOT(clear()));
		plugSharedAction("edit_undo", SLOT(undo()));
		plugSharedAction("edit_redo", SLOT(redo()));
                plugSharedAction("edit_select_all", SLOT(selectAll()));
#else
		QValueList<QCString> actions;
		actions << "edit_cut" << "edit_copy" << "edit_paste" << "edit_clear"
			<< "edit_undo" << "edit_redo" << "edit_select_all";
		plugSharedActionsToExternalGUI(actions, dynamic_cast<KXMLGUIClient*>(obj));
#endif
	}
};

//! @internal
class KexiEditorPrivate {
	public:
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
		KTextEdit *view;
#else
		KTextEditor::Document *doc;
		KTextEditor::View *view;
#endif
};

KexiEditor::KexiEditor(KexiMainWindow *mainWin, QWidget *parent, const char *name)
	: KexiViewBase(mainWin, parent, name)
	, d(new KexiEditorPrivate())
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
	d->view->setCheckSpellingEnabled(false);
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

	KTextEditor::PopupMenuInterface *popupInt = dynamic_cast<KTextEditor::PopupMenuInterface*>( d->view );
	if(popupInt) {
			QPopupMenu *pop = (QPopupMenu*) mainWin->factory()->container("edit", mainWin);
			if(pop) {
				 //plugSharedAction("edit_undo", pop);
				 popupInt->installPopup(pop);
			}
	}

	connect(d->doc, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
#endif
	KexiEditorSharedActionConnector c(this, d->view);
	d->view->installEventFilter(this);

	layout->addWidget(d->view);
	setViewWidget(d->view, true/*focus*/);
	d->view->show();
}

KexiEditor::~KexiEditor()
{
	delete d;
}

void KexiEditor::updateActions(bool activated)
{
	KexiViewBase::updateActions(activated);
}

bool KexiEditor::isAdvancedEditor()
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
	return false;
#else
	return true;
#endif
}

QString KexiEditor::text()
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
	return d->view->text();
#else
	if (!d->doc)
		return QString::null;
	KTextEditor::EditInterface *eIface = KTextEditor::editInterface(d->doc);
	return eIface->text();
#endif
}

void KexiEditor::setText(const QString &text)
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
	const bool was_dirty = m_parentView ? m_parentView->dirty() : dirty();
	d->view->setText(text);
	setDirty(was_dirty);
#else
	if (!d->doc)
		return;
	const bool was_dirty = dirty();
	KTextEditor::EditInterface *eIface = KTextEditor::editInterface(d->doc);
	eIface->setText(text);
	KTextEditor::UndoInterface *undoIface = KTextEditor::undoInterface(d->doc);
	undoIface->clearUndo();
	undoIface->clearRedo();
	setDirty(was_dirty);
#endif
}

void KexiEditor::setHighlightMode(const QString& highlightmodename)
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
#else
	KTextEditor::HighlightingInterface *hl = KTextEditor::highlightingInterface( d->doc );
	for(uint i = 0; i < hl->hlModeCount(); i++) {
			//kdDebug() << "hlmode("<<i<<"): " << hl->hlModeName(i) << endl;
			if (hl->hlModeName(i).contains(highlightmodename, false))  {
				hl->setHlMode(i);
				return;
			}
	}
	hl->setHlMode(0); // 0=None, don't highlight anything.
#endif
}

void KexiEditor::slotConfigureEditor()
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
	//TODO show errormessage?
#else
	KTextEditor::ConfigInterface *config = KTextEditor::configInterface( d->doc );
	if (config)
		config->configDialog();
#endif
}

void KexiEditor::jump(int character)
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
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
#else
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
#endif
}

void KexiEditor::setCursorPosition(int line, int col)
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
	d->view->setCursorPosition(line, col);
#else
	KTextEditor::ViewCursorInterface *ci = KTextEditor::viewCursorInterface( d->view );
	ci->setCursorPosition(line, col);
#endif
}

void KexiEditor::clearUndoRedo()
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
	//TODO how to remove undo/redo from a KTextEdit?
#else
	KTextEditor::UndoInterface* u = KTextEditor::undoInterface( d->doc );
	u->clearUndo();
	u->clearRedo();
#endif
}

#include "kexieditor.moc"

