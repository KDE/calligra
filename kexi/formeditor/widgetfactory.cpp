/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#include <qlayout.h>
#include <qdialog.h>
#include <qcursor.h>

#include <keditlistbox.h>
#include <kstdguiitem.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <ktextedit.h>
#include <ktoolbar.h>
#include <kfontcombo.h>
#include <kcolorcombo.h>
#include <ktoolbarradiogroup.h>
#include <kdebug.h>
#include <klocale.h>

#include "resizehandle.h"
#include "objpropbuffer.h"
#include "formmanager.h"
#include "form.h"
#include "container.h"
#include "objecttree.h"
#include "widgetfactory.h"


namespace KFormDesigner {

// A simple dialog to edit rich text

RichTextDialog::RichTextDialog(QWidget *parent, const QString &text)
: KDialogBase(parent, "richtext_dialog", true, i18n("Edit rich text"), Ok|Cancel, Ok, false)
{
	QFrame *frame = makeMainWidget();
	QVBoxLayout *l = new QVBoxLayout(frame);
	l->setAutoAdd(true);

	m_toolbar = new KToolBar(frame);
	m_toolbar->setFlat(true);
	m_toolbar->show();

	m_fcombo = new KFontCombo(m_toolbar);
	m_toolbar->insertWidget(1, 40, m_fcombo);
	connect(m_fcombo, SIGNAL(textChanged(const QString&)), this, SLOT(changeFont(const QString &)));

	m_toolbar->insertSeparator();

	m_colCombo = new KColorCombo(m_toolbar);
	m_toolbar->insertWidget(2, 30, m_colCombo);
	connect(m_colCombo, SIGNAL(activated(const QColor&)), this, SLOT(changeColor(const QColor&)));

	m_toolbar->insertButton("text_bold", 3, true, i18n("Bold"));
	m_toolbar->insertButton("text_italic", 4, true, i18n("Italic"));
	m_toolbar->insertButton("text_under", 50, true, i18n("Underline"));
	m_toolbar->setToggle(3, true);
	m_toolbar->setToggle(4, true);
	m_toolbar->setToggle(50, true);

	m_toolbar->insertSeparator();

	KToolBarRadioGroup *group = new KToolBarRadioGroup(m_toolbar);
	m_toolbar->insertButton("text_left", 6, true, i18n("Right Align"));
	m_toolbar->setToggle(6, true);
	group->addButton(6);
	m_toolbar->insertButton("text_center", 7, true, i18n("Left Align"));
	m_toolbar->setToggle(7, true);
	group->addButton(7);
	m_toolbar->insertButton("text_right", 8, true, i18n("Centered"));
	m_toolbar->setToggle(8, true);
	group->addButton(8);
	m_toolbar->insertButton("text_block", 9, true, i18n("Justified"));
	m_toolbar->setToggle(9, true);
	group->addButton(9);

	connect(m_toolbar, SIGNAL(toggled(int)), this, SLOT(buttonToggled(int)));

	m_edit = new KTextEdit(text, QString::null, frame, "richtext_edit");
	m_edit->setTextFormat(RichText);
	m_edit->setFocus();

	connect(m_edit, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(cursorPositionChanged(int, int)));
	connect(m_edit, SIGNAL(clicked(int, int)), this, SLOT(cursorPositionChanged(int, int)));

	m_edit->moveCursor(QTextEdit::MoveEnd, false);
	cursorPositionChanged(0, 0);
	m_edit->show();
	frame->show();
}

QString
RichTextDialog::text()
{
	return m_edit->text();
}

void
RichTextDialog::changeFont(const QString &font)
{
	m_edit->setFamily(font);
}

void
RichTextDialog::changeColor(const QColor &color)
{
	m_edit->setColor(color);
}

void
RichTextDialog::buttonToggled(int id)
{
	bool isOn = m_toolbar->isButtonOn(id);

	switch(id)
	{
		case 3: m_edit->setBold(isOn); break;
		case 4: m_edit->setItalic(isOn); break;
		case 50: m_edit->setUnderline(isOn); break;
		case 6: case 7:
		case 8: case 9:
		{
			if(!isOn)  break;
			switch(id)
			{
				case 6:  m_edit->setAlignment(Qt::AlignLeft); break;
				case 7:  m_edit->setAlignment(Qt::AlignCenter); break;
				case 8:  m_edit->setAlignment(Qt::AlignRight); break;
				case 9:  m_edit->setAlignment(Qt::AlignJustify); break;
				default: break;
			}
		}
		default: break;
	}

}

void
RichTextDialog::cursorPositionChanged(int, int)
{
//	if (m_edit->hasSelectedText())
//		return;

	m_fcombo->setCurrentFont(m_edit->currentFont().family());
	m_colCombo->setColor(m_edit->color());
	m_toolbar->setButton(3, m_edit->bold());
	m_toolbar->setButton(4, m_edit->italic());
	m_toolbar->setButton(50, m_edit->underline());

	int id = 0;
	switch(m_edit->alignment())
	{
		case Qt::AlignLeft:    id = 6; break;
		case Qt::AlignCenter:  id = 7; break;
		case Qt::AlignRight:   id = 8; break;
		case Qt::AlignJustify: id = 9; break;
		default:  id = 6; break;
	}
	m_toolbar->setButton(id, true);
}

///// Widget Factory //////////////////////////

WidgetFactory::WidgetFactory(QObject *parent, const char *name)
 : QObject(parent, name)
{
}

KLineEdit*
WidgetFactory::createEditor(const QString &text, QWidget *w, QRect geometry, int align,  bool useFrame, BackgroundMode background)
{
	KLineEdit *editor = new KLineEdit(text, w->parentWidget());
	editor->setAlignment(align);
	editor->setPalette(w->palette());
	editor->setFont(w->font());
	editor->setGeometry(geometry);
	if(background == Qt::NoBackground)
		editor->setBackgroundMode(w->backgroundMode());
	else
		editor->setBackgroundMode(background);
	editor->installEventFilter(this);
	editor->setFrame(useFrame);
	editor->show();
	editor->setFocus();
	connect(editor, SIGNAL(textChanged(const QString&)), this, SLOT(changeText(const QString&)));
	connect(w, SIGNAL(destroyed()), this, SLOT(resetEditor()));
	connect(editor, SIGNAL(destroyed()), this, SLOT(editorDeleted()));

	m_handles = new ResizeHandleSet(w, true);

	m_editor = editor;
	m_widget = w;
	return editor;
}

void
WidgetFactory::disableFilter(QWidget *w, Container *container)
{
	ObjectTreeItem *tree = container->form()->objectTree()->lookup(w->name());
	if(!tree)
		return;
	tree->eventEater()->setContainer(this);

	w->setFocus();
	m_handles = new ResizeHandleSet(w, true);
	m_widget = w;
	m_container = container;
	m_editor = 0;

	connect(w, SIGNAL(destroyed()), this, SLOT(resetEditor()));
}

bool
WidgetFactory::editList(QWidget *w, QStringList &list)
{
	KDialogBase* dialog = new KDialogBase(w->topLevelWidget(), "stringlist_dialog", true, i18n("Edit list of items"),
	    KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, false);

	KEditListBox *edit = new KEditListBox(i18n("%1 contents").arg(w->name()), dialog, "editlist");
	dialog->setMainWidget(edit);
	edit->insertStringList(list);
	edit->show();

	if(dialog->exec() == QDialog::Accepted)
	{
		list = edit->items();
		return true;
	}
	else
		return false;
}

bool
WidgetFactory::editRichText(QWidget *w, QString &text)
{
	RichTextDialog *d = new RichTextDialog(w, text);
	if( ((QDialog*)d)->exec()== QDialog::Accepted)
	{
		text = d->text();
		return true;
	}
	else
		return false;
}

bool
WidgetFactory::eventFilter(QObject *obj, QEvent *ev)
{
	QWidget *w = m_editor ? m_editor : (QWidget *)m_widget;

	if(obj != (QObject *)w)
		return false;

	if(ev->type() == QEvent::FocusOut)
	{
		QWidget *focus = w->topLevelWidget()->focusWidget();
		if(w != focus && !w->child(focus->name(), focus->className()))
			resetEditor();
	}
	else if(ev->type() == QEvent::KeyPress)
	{
		QKeyEvent *e = static_cast<QKeyEvent*>(ev);
		if(((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter)) && (e->state() != ControlButton))
			resetEditor();
	}
	else if(ev->type() == QEvent::ContextMenu)
		return true;

	return false;
}

void
WidgetFactory::resetEditor()
{
	if(!m_editor && m_widget)
	{
		ObjectTreeItem *tree = m_container->form()->objectTree()->lookup(m_widget->name());
		if(!tree)
		{
			kdDebug() << "WidgetFactory::resetEditor() : error cannot found a tree item " << endl;
			return;
		}
		tree->eventEater()->setContainer(m_container);
	}
	else if(m_editor)
	{
		changeText(m_editor->text());
		disconnect(m_editor, 0, this, 0);
		m_editor->deleteLater();
	}

	if(m_widget)
		disconnect(m_widget, 0, this, 0);
	delete m_handles;
	m_editor = 0;
	m_widget = 0;
	m_handles = 0;
}

void
WidgetFactory::editorDeleted()
{
	delete m_handles;
	m_editor = 0;
	m_handles = 0;
}

void
WidgetFactory::changeProperty(const char *name, const QVariant &value, Container *container)
{
	if (!container->form()->manager())
		return;
	KFormDesigner::ObjectPropertyBuffer *buff = container->form()->manager()->buffer();
	if((*buff)[name])
		(*buff)[name]->setValue(value, true);
}

void
WidgetFactory::addPropertyDescription(Container *container, const char *prop, const QString &desc)
{
	ObjectPropertyBuffer *buff = container->form()->manager()->buffer();
	buff->addPropertyDescription(prop, desc);
}

void
WidgetFactory::addValueDescription(Container *container, const char *value, const QString &desc)
{
	ObjectPropertyBuffer *buff = container->form()->manager()->buffer();
	buff->addValueDescription(value, desc);
}

WidgetFactory::~WidgetFactory()
{
}

}

#include "widgetfactory.moc"
