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

#include <keximainwindow.h>
#include <kexidialogbase.h>

//uncomment this to enable KTextEdit-based editor
//#define KTEXTEDIT_BASED_SQL_EDITOR

//TODO: detect if KTextEditor returned something, if not- force KTEXTEDIT_BASED_SQL_EDITOR option

#include <qlayout.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qasciidict.h>
#include <qobjectlist.h>

#include <klocale.h>
#include <kdebug.h>
#include <kxmlguifactory.h>

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
# include <ktexteditor/highlightinginterface.h>
# include <ktexteditor/editinterface.h>
# include <ktexteditor/viewcursorinterface.h>

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

class KexiQueryDesignerSQLEditorPrivate {
	public:
		KexiQueryDesignerSQLEditorPrivate() 
//		 : firstActivation(true)
//		 , katepartGuiClientAdded(false)
		 : keyEventFilter_enabled(true)
		{}
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
		KTextEdit *view;
#else
		KTextEditor::Document *doc;
		KTextEditor::View *view;
#endif
//		bool firstActivation : 1;
//		bool katepartGuiClientAdded : 1;
		bool keyEventFilter_enabled : 1;
};

//=====================

KexiQueryDesignerSQLEditor::KexiQueryDesignerSQLEditor(
	KexiMainWindow *mainWin, QWidget *parent, const char *name)
 : KexiViewBase(mainWin, parent, name)
 ,d(new KexiQueryDesignerSQLEditorPrivate())
{
	QVBoxLayout *lyr = new QVBoxLayout(this);
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
	d->view = new KTextEdit( "", QString::null, this, "sqlDoc_editor" );
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
	lyr->addWidget(fr);
	lyr = new QVBoxLayout(fr);
	lyr->setMargin( 2 );

	d->doc =  KTextEditor::EditorChooser::createDocument(fr, "sqlDoc");
	d->view = d->doc->createView(fr, 0L);


//	f->resetContainer("codefolding");

// void KAction::unplug  	(   	QWidget *   	 w  	 )   	 [virtual]


//	KAction *a = d->view->action("edit_cut");
//	plugSharedAction("edit_cut", a, SLOT(activate()));

	plugSharedActionToExternalGUI("edit_cut", d->view);
	plugSharedActionToExternalGUI("edit_copy", d->view);
	plugSharedActionToExternalGUI("edit_paste", d->view);

	plugSharedActionToExternalGUI("edit_undo", d->view);
	plugSharedActionToExternalGUI("edit_redo", d->view);

//part()->instanceGuiClient(viewMode())->insertChildClient(d->view);

 #if 0


		QAsciiDict<char> enabledActionsDict(101, true, false);

			static const char* enabledActions[] = {
			"edit_undo",
			"edit_redo",
			"edit_cut",
			"edit_copy",
			"edit_paste",
			"edit_select_all",
			"edit_find",
			"edit_find_next",
			"edit_find_prev",
			"edit_replace",
			"go_goto_line",
			//"view_dynamic_word_wrap",
			//"dynamic_word_wrap_indicators",
			//"view_word_wrap_marker"
			"view_border",
			"view_line_numbers",
			//"view_folding_markers",
			"tools_indent",
			"tools_unindent",
			"tools_cleanIndent",
		//	"tools_comment",
		//	"tools_uncomment",
		//	"tools_join_lines",
			"tools_apply_wordwrap",
			//"set_confdlg",
			"folding_toplevel",
			0};
			for (const char** a = enabledActions; *a; a++)
				enabledActionsDict.insert(*a, (char*)1);

			QAsciiDict<char> disabledMenusDict(17, true, false);
			static const char* disabledMenus[] = { "codefolding", 0 };
			for (const char** m = disabledMenus; *m; m++)
				disabledMenusDict.insert(*m, (char*)1);

			
			if (part() && part()->instanceGuiClient(viewMode())) {
				part()->instanceGuiClient(viewMode())->insertChildClient(d->view);
			}
//			mainWin()->guiFactory()->addClient(d->view);

			KActionCollection *ac = d->view->actionCollection();
			KActionPtrList alist = ac->actions();
			for (KActionPtrList::iterator it=alist.begin(); it!=alist.end(); ++it) {
				if (!enabledActionsDict[(*it)->name()]) {
		//			ac->remove(*it);
					(*it)->unplugAll();
				}
			}

			KXMLGUIFactory *f = d->view->factory();
		//	KAction *a = d->view->action("codefolding");
//			KAction *a = ac->action("folding_toplevel");
			
		/*
			QWidget *popup =f->container("codefolding", d->view);
			QPopupMenu *par_popup = static_cast<QPopupMenu*>(popup->parentWidget());
			int i;
			QString txt = i18n("&Code Folding");
			for (i=0; i<par_popup->count(); i++) {
				if (par_popup->text(i)==txt)
					break;
			}
			if (i<par_popup->count()) {
				par_popup->removeItemAt(i);
			}*/

		#if 1
			//store menu popups list
		//		QObjectList *l = mainWin->queryList( "KActionMenu" );
				QObjectList *l = this->mainWin()->queryList( "QPopupMenu" );
				for (QObjectListIt it( *l ); it.current(); ++it ) {
					kdDebug() << "name=" <<it.current()->name() << " cname="<<it.current()->className()<<endl;
					if (qstrcmp(it.current()->name(),"view")==0) {
						QString txt = i18n("&Code Folding");
						QPopupMenu *par_popup = static_cast<QPopupMenu*>(it.current());
						uint i;
						bool wasSeparator = false;
						for (i=0; i<par_popup->count(); i++) {
							kdDebug() << par_popup->idAt(i) <<" '" << par_popup->text( par_popup->idAt(i) ) << "'" << endl;
							if (par_popup->text( par_popup->idAt(i) )==txt)
								break;
							wasSeparator = par_popup->text( par_popup->idAt(i) ).isEmpty();
						}
						if (i<par_popup->count()) {
							par_popup->setItemVisible( par_popup->idAt(i), false );//removeItemAt(i);
							i++;
							if (wasSeparator && i<par_popup->count()) {
								if (par_popup->text( par_popup->idAt(i) ).isEmpty()) {
									par_popup->setItemVisible( par_popup->idAt(i), false );//removeItemAt(i);
								}
							}
						}
					}


		/*			if ( disabledMenusDict[ it.current()->name() ] ) {
						kdDebug() << "REMOVE" << endl;
						KPopupMenu *menu = static_cast<KPopupMenu*>(it.current());
						menu->hide();
		//				menu->reparent(0, 0, QPoint(0,0));
		//				delete menu;
					}*/
				}
				delete l;
			
		#endif

		//TODO: remove redundant separators in ALL menus



 #endif //0



	KTextEditor::HighlightingInterface *hl = KTextEditor::highlightingInterface(d->doc);
	for(uint i=0; i < hl->hlModeCount(); i++)
	{
//		kdDebug() << "hlmode("<<i<<"): " << hl->hlModeName(i) << endl;
		if (hl->hlModeName(i).contains("sql", false))
		{
			hl->setHlMode(i);
			break;
		}
	}
	connect(d->doc, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
#endif
	setViewWidget(d->view);
	d->view->installEventFilter(this);


//	setFocusProxy(d->view);
	lyr->addWidget(d->view);

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
//	mainWin()->guiFactory()->removeClient(d->view);
}

void KexiQueryDesignerSQLEditor::updateActions(bool activated)
{

	KexiViewBase::updateActions(activated);
}

void
KexiQueryDesignerSQLEditor::jump(int character)
{
	//find row and column for this character
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
	const int numRows = d->view->paragraphs();
#else
	KTextEditor::EditInterface *ei = KTextEditor::editInterface(d->doc);
	const int numRows = ei->numLines();
#endif
	int row = 0, col = 0;
	for (int ch = 0; row < numRows; row++) {
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
	const int rowLen = d->view->paragraphLength(row)+1;
#else
	const int rowLen = ei->lineLength(row)+1;
#endif
		if ((ch + rowLen) > character) {
			col = character-ch;
			break;
		}
		ch += rowLen;
	}
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
	d->view->setCursorPosition(row, col);
#else
	KTextEditor::ViewCursorInterface *ci = KTextEditor::viewCursorInterface(d->view);
	ci->setCursorPositionReal(row, col);
#endif
}


bool KexiQueryDesignerSQLEditor::eventFilter(QObject *o, QEvent *ev)
{
	if(ev->type() == QEvent::KeyPress && o==d->view) {
		QKeyEvent *ke = static_cast<QKeyEvent*>(ev);
		kdDebug() << ke->key() << endl;
	}
	return false;
}

// === KexiQueryDesignerSQLEditor implementation using KTextEditor ===

#ifdef KTEXTEDIT_BASED_SQL_EDITOR
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

#endif //!KTEXTEDIT_BASED_SQL_EDITOR


#include "kexiquerydesignersqleditor.moc"

