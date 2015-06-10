/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004-2005 Jarosław Staniek <staniek@kde.org>
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

#include <QList>
#include <QMenu>

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
    KexiEditorSharedActionConnector(KexiActionProxy* proxy, QObject* obj)
            : KexiSharedActionConnector(proxy, obj) {
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
//! @todo KEXI3 plugSharedActionsToExternalGUI(actions, dynamic_cast<KXMLGUIClient*>(obj));
#endif
    }
};

//! @internal
class KexiEditor::Private
{
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
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
    d->view = new KTextEdit("", QString(), this);
    //adjust font
    connect(d->view, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
    QFont f("Courier");
    f.setStyleStrategy(QFont::PreferAntialias);
    f.setPointSize(d->view->font().pointSize());
    d->view->setFont(f);
    d->view->setCheckSpellingEnabled(false);
#else
    KexiUtils::KTextEditorFrame *fr = new KexiUtils::KTextEditorFrame(this);
    QVBoxLayout *layout = new QVBoxLayout(fr);
    layout->setContentsMargins(2, 2, 2, 2);

    KTextEditor::Editor *editor = KTextEditor::EditorChooser::editor();
    if (!editor)
        return;
//! @todo error handling!

    d->doc = editor->createDocument(fr);
    if (!d->doc)
        return;
    d->view = d->doc->createView(fr);
    // suppresing default saving mechanism of KTextEditor
    d->view->action("file_save")->setEnabled(false);
    // set word wrap by default
    KTextEditor::ConfigInterface *configIface
        =
qobject_cast<KTextEditor::ConfigInterface*>( d->view );
    configIface->setConfigValue("dynamic-word-wrap", true);

//! @todo KEXI3 Q3PopupMenu *pop = qobject_cast<Q3PopupMenu*>( mainWin->factory()->container("edit", mainWin) );
//! @todo KEXI3 d->view->setContextMenu(pop);
    d->view->setContextMenu(d->view->defaultContextMenu());

    connect(d->doc, SIGNAL(textChanged(KTextEditor::Document*)),
            this, SLOT(slotTextChanged(KTextEditor::Document*)));
#endif
    KexiEditorSharedActionConnector c(this, d->view);
    d->view->installEventFilter(this);

    layout->addWidget(d->view);
    setViewWidget(fr, false/*!focus*/);
    setFocusProxy(d->view);
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
    d->doc->setText(text);
    setDirty(was_dirty);
#endif
}

void KexiEditor::setHighlightMode(const QString& highlightmodename)
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
#else
    if (!d->doc)
        return;
    QString n = highlightmodename;
    if (n == "javascript" || n == "qtscript")
        n = "JavaScript";
    else if (n.size() > 0)
        n = n[0].toLower() + n.mid(1);
    if (!d->doc->setMode(n))
        d->doc->setMode(QString()); // don't highlight
    if (!d->doc->setHighlightingMode(n))
        d->doc->setHighlightingMode(QString()); //hl->setHlMode(0); // 0=None, don't highlight anything.

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
        const int rowLen = d->view->paragraphLength(row) + 1;
        if ((ch + rowLen) > character) {
            col = character - ch;
            break;
        }
        ch += rowLen;
    }
    d->view->setCursorPosition(row, col);
#else
    if (!d->doc)
        return;
    const int numRows = d->doc->lines();
    int row = 0, col = 0;
    for (int ch = 0; row < numRows; row++) {
        const int rowLen = d->doc->lineLength(row) + 1;
        if ((ch + rowLen) > character) {
            col = character - ch;
            break;
        }
        ch += rowLen;
    }
    d->view->setCursorPosition(KTextEditor::Cursor(row, col));
#endif
}

void KexiEditor::setCursorPosition(int line, int col)
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
    d->view->setCursorPosition(line, col);
#else
    d->view->setCursorPosition(KTextEditor::Cursor(line, col));
#endif
}

void KexiEditor::clearUndoRedo()
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
    //! @todo how to remove undo/redo from a KTextEdit?
#else
//! @todo KEXI3 KexiEditor::clearUndoRedo()
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
    QMenu* menu = d->view->defaultContextMenu();
    menu->addSeparator();
    menu->addAction(d->view->action("edit_find"));
    menu->addAction(d->view->action("edit_find_next"));
    menu->addAction(d->view->action("edit_find_prev"));
    menu->addAction(d->view->action("edit_replace"));
    menu->addAction(d->view->action("go_goto_line"));
    return menu;
#endif
}

