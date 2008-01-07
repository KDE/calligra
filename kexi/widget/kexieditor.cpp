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

#include <KexiMainWindowIface.h>
#include <kexiutils/utils.h>

#include <QLayout>
#include <QList>
#include <QMenu>
#include <KDebug>

//uncomment this to enable KTextEdit-based editor
//#define KTEXTEDIT_BASED_SQL_EDITOR

#ifdef KTEXTEDIT_BASED_SQL_EDITOR
# include <ktextedit.h>
#else
# include <ktexteditor/document.h>
# include <ktexteditor/view.h>
# include <ktexteditor/editorchooser.h>
# include <ktexteditor/configinterface.h>
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
		QList<QByteArray> actions;
		actions << "edit_cut" << "edit_copy" << "edit_paste" << "edit_clear"
			<< "edit_undo" << "edit_redo" << "edit_select_all";
#ifdef __GNUC__
#warning TODO	plugSharedActionsToExternalGUI(actions, dynamic_cast<KXMLGUIClient*>(obj));
#else
#pragma WARNING( TODO	plugSharedActionsToExternalGUI(actions, dynamic_cast<KXMLGUIClient*>(obj)); )
#endif
#endif
	}
};

//! @internal
class KexiEditor::Private {
	public:
		Private() {}
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
		KTextEdit *view;
#else
		KTextEditor::Document *doc;
		KTextEditor::View *view;
#endif
};

KexiEditor::KexiEditor(QWidget *parent)
	: KexiView(parent)
	, d(new Private())
{
//	Q3VBoxLayout *layout = new Q3VBoxLayout(this);
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
	d->view = new KTextEdit( "", QString(), this );
	//adjust font
	connect(d->view, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
	QFont f("Courier");
	f.setStyleStrategy(QFont::PreferAntialias);
	f.setPointSize(d->view->font().pointSize());
	d->view->setFont( f );
	d->view->setCheckSpellingEnabled(false);
#else
	KexiUtils::KTextEditorFrame *fr = new KexiUtils::KTextEditorFrame(this);
	QVBoxLayout *layout = new QVBoxLayout(fr);
	layout->setContentsMargins( 2,2,2,2 );

	KTextEditor::Editor *editor = KTextEditor::EditorChooser::editor();
	if (!editor)
		return;
//! @todo error handling!

	d->doc = editor->createDocument(fr);
	if (!d->doc)
		return;
	d->view = d->doc->createView(fr);

#ifdef __GNUC__
#warning TODO	Q3PopupMenu *pop = qobject_cast<Q3PopupMenu*>( mainWin->factory()->container("edit", mainWin) );
#else
#pragma WARNING( TODO	Q3PopupMenu *pop = qobject_cast<Q3PopupMenu*>( mainWin->factory()->container("edit", mainWin) ); )
#endif
#ifdef __GNUC__
#warning TODO	d->view->setContextMenu(pop);
#else
#pragma WARNING( TODO	d->view->setContextMenu(pop); )
#endif
    //d->view->defaultContextMenu();

/*	KTextEditor::PopupMenuInterface *popupInt = dynamic_cast<KTextEditor::PopupMenuInterface*>( d->view );
	if(popupInt) {
		Q3PopupMenu *pop = (Q3PopupMenu*) mainWin->factory()->container("edit", mainWin);
		if(pop) {
			 //plugSharedAction("edit_undo", pop);
			 popupInt->installPopup(pop);
		}
	}*/

	connect(d->doc, SIGNAL(textChanged(KTextEditor::Document *)), 
		this, SLOT(slotTextChanged(KTextEditor::Document *)));
#endif
	KexiEditorSharedActionConnector c(this, d->view);
	d->view->installEventFilter(this);

	layout->addWidget(d->view);
	setViewWidget(fr, false/*!focus*/);
	setFocusProxy(d->view);
//	d->view->show();
}

KexiEditor::~KexiEditor()
{
	delete d;
}

void KexiEditor::updateActions(bool activated)
{
	KexiView::updateActions(activated);
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
		return QString();
//	KTextEditor::EditInterface *eIface = KTextEditor::editInterface(d->doc);
//	return eIface->text();
	return d->doc->text();
#endif
}

void KexiEditor::setText(const QString &text)
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
	const bool was_dirty = m_parentView ? m_parentView->idDirty() : idDirty();
	d->view->setText(text);
	setDirty(was_dirty);
#else
	if (!d->doc)
		return;
	const bool was_dirty = isDirty();
//	KTextEditor::EditInterface *eIface = KTextEditor::editInterface(d->doc);
//	eIface->setText(text);
	d->doc->setText(text);
	setDirty(was_dirty);
#endif
}

void KexiEditor::setHighlightMode(const QString& highlightmodename)
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
#else
    QString n = highlightmodename;
    if( n == "javascript" || n == "qtscript" )
        n = "JavaScript";
    else if( n.size() > 0 )
        n = n[0].toLower() + n.mid(1);
	if (!d->doc->setMode(n))
		d->doc->setMode(QString()); // don't highlight
	if (!d->doc->setHighlightingMode(n) )
        d->doc->setHighlightingMode(QString()); //hl->setHlMode(0); // 0=None, don't highlight anything.

//d->view->viewModeChanged(d->view);
//d->view->reloadXML();

QMetaObject::invokeMethod(d->view, "modeChanged", Q_ARG(KTextEditor::Document*, d->doc));
QMetaObject::invokeMethod(d->view, "highlightingModeChanged", Q_ARG(KTextEditor::Document*, d->doc));

#endif
}

void KexiEditor::slotConfigureEditor()
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
//! @todo show configuration...
#else
	if (!d->doc)
		return;
	d->doc->editor()->configDialog(this);
//! @todo use d->doc->editor()->writeConfig() or KTextEditor::ConfigInterface to save changes
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
//	KTextEditor::EditInterface *ei = KTextEditor::editInterface(d->doc);
	const int numRows = d->doc->lines();
	int row = 0, col = 0;
	for (int ch = 0; row < numRows; row++) {
		const int rowLen = d->doc->lineLength(row)+1;
		if ((ch + rowLen) > character) {
			col = character-ch;
			break;
		}
		ch += rowLen;
	}
//	KTextEditor::ViewCursorInterface *ci = KTextEditor::viewCursorInterface(d->view);
	d->view->setCursorPosition( KTextEditor::Cursor(row, col) );
#endif
}

void KexiEditor::setCursorPosition(int line, int col)
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
	d->view->setCursorPosition(line, col);
#else
//	KTextEditor::ViewCursorInterface *ci = KTextEditor::viewCursorInterface( d->view );
	d->view->setCursorPosition( KTextEditor::Cursor(line, col) );
#endif
}

void KexiEditor::clearUndoRedo()
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
	//TODO how to remove undo/redo from a KTextEdit?
#else
#ifdef __GNUC__
#warning TODO KexiEditor::clearUndoRedo()
#else
#pragma WARNING( TODO KexiEditor::clearUndoRedo() )
#endif
/*
	KTextEditor::UndoInterface* u = KTextEditor::undoInterface( d->doc );
	u->clearUndo();
	u->clearRedo();*/
#endif
}

void KexiEditor::slotTextChanged(KTextEditor::Document *)
{
	emit textChanged();
}

QMenu* KexiEditor::defaultContextMenu()
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
    return d->view->createStandardContextMenu();
#else
    return d->view->defaultContextMenu();
#endif
}

#include "kexieditor.moc"
