/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include <qlayout.h>

#include <ktoolbar.h>
#include <kfontcombo.h>
#include <kcolorcombo.h>
#include <ktoolbarradiogroup.h>
#include <kdebug.h>
#include <klocale.h>

#include "richtextdialog.h"

namespace KFormDesigner {

//////////////////////////////////////////////////////////////////////////////////
//////////////// A simple dialog to edit rich text   ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

RichTextDialog::RichTextDialog(QWidget *parent, const QString &text)
: KDialogBase(parent, "richtext_dialog", true, i18n("Edit Rich Text"), Ok|Cancel, Ok, false)
{
	QFrame *frame = makeMainWidget();
	QVBoxLayout *l = new QVBoxLayout(frame);
	l->setAutoAdd(true);

	m_toolbar = new KToolBar(frame);
	m_toolbar->setFlat(true);
	m_toolbar->show();

	m_fcombo = new KFontCombo(m_toolbar);
	m_toolbar->insertWidget(TBFont, 40, m_fcombo);
	connect(m_fcombo, SIGNAL(textChanged(const QString&)), this, SLOT(changeFont(const QString &)));

	m_toolbar->insertSeparator();

	m_colCombo = new KColorCombo(m_toolbar);
	m_toolbar->insertWidget(TBColor, 30, m_colCombo);
	connect(m_colCombo, SIGNAL(activated(const QColor&)), this, SLOT(changeColor(const QColor&)));

	m_toolbar->insertButton("text_bold", TBBold, true, i18n("Bold"));
	m_toolbar->insertButton("text_italic", TBItalic, true, i18n("Italic"));
	m_toolbar->insertButton("text_under", TBUnder, true, i18n("Underline"));
	m_toolbar->setToggle(TBBold, true);
	m_toolbar->setToggle(TBItalic, true);
	m_toolbar->setToggle(TBUnder, true);
	m_toolbar->insertSeparator();

	m_toolbar->insertButton("text_super", TBSuper, true, i18n("Superscript"));
	m_toolbar->insertButton("text_sub", TBSub, true, i18n("Subscript"));
	m_toolbar->setToggle(TBSuper, true);
	m_toolbar->setToggle(TBSub, true);
	m_toolbar->insertSeparator();

	KToolBarRadioGroup *group = new KToolBarRadioGroup(m_toolbar);
	m_toolbar->insertButton("text_left", TBLeft, true, i18n("Left Align"));
	m_toolbar->setToggle(TBLeft, true);
	group->addButton(TBLeft);
	m_toolbar->insertButton("text_center", TBCenter, true, i18n("Centered"));
	m_toolbar->setToggle(TBCenter, true);
	group->addButton(TBCenter);
	m_toolbar->insertButton("text_right", TBRight, true, i18n("Right Align"));
	m_toolbar->setToggle(TBRight, true);
	group->addButton(TBRight);
	m_toolbar->insertButton("text_block", TBJustify, true, i18n("Justified"));
	m_toolbar->setToggle(TBJustify, true);
	group->addButton(TBJustify);

	connect(m_toolbar, SIGNAL(toggled(int)), this, SLOT(buttonToggled(int)));

	m_edit = new KTextEdit(text, QString::null, frame, "richtext_edit");
	m_edit->setTextFormat(RichText);
	m_edit->setFocus();

	connect(m_edit, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(cursorPositionChanged(int, int)));
	connect(m_edit, SIGNAL(clicked(int, int)), this, SLOT(cursorPositionChanged(int, int)));
	connect(m_edit, SIGNAL(currentVerticalAlignmentChanged(VerticalAlignment)), this, SLOT(slotVerticalAlignmentChanged(VerticalAlignment)));

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
		case TBBold: m_edit->setBold(isOn); break;
		case TBItalic: m_edit->setItalic(isOn); break;
		case TBUnder: m_edit->setUnderline(isOn); break;
		case TBSuper:
		{
			if(isOn && m_toolbar->isButtonOn(TBSub))
				m_toolbar->setButton(TBSub, false);
			m_edit->setVerticalAlignment(isOn ? QTextEdit::AlignSuperScript : QTextEdit::AlignNormal);
			break;
		}
		case TBSub:
		{
			if(isOn && m_toolbar->isButtonOn(TBSuper))
				m_toolbar->setButton(TBSuper, false);
			m_edit->setVerticalAlignment(isOn ? QTextEdit::AlignSubScript : QTextEdit::AlignNormal);
			break;
		}
		case TBLeft: case TBCenter:
		case TBRight: case TBJustify:
		{
			if(!isOn)  break;
			switch(id)
			{
				case TBLeft:  m_edit->setAlignment(Qt::AlignLeft); break;
				case TBCenter:  m_edit->setAlignment(Qt::AlignCenter); break;
				case TBRight:  m_edit->setAlignment(Qt::AlignRight); break;
				case TBJustify:  m_edit->setAlignment(Qt::AlignJustify); break;
				default: break;
			}
		}
		default: break;
	}

}

void
RichTextDialog::cursorPositionChanged(int, int)
{
	m_fcombo->setCurrentFont(m_edit->currentFont().family());
	m_colCombo->setColor(m_edit->color());
	m_toolbar->setButton(TBBold, m_edit->bold());
	m_toolbar->setButton(TBItalic, m_edit->italic());
	m_toolbar->setButton(TBUnder, m_edit->underline());

	int id = 0;
	switch(m_edit->alignment())
	{
		case Qt::AlignLeft:    id = TBLeft; break;
		case Qt::AlignCenter:  id = TBCenter; break;
		case Qt::AlignRight:   id = TBRight; break;
		case Qt::AlignJustify: id = TBJustify; break;
		default:  id = TBLeft; break;
	}
	m_toolbar->setButton(id, true);
}

void
RichTextDialog::slotVerticalAlignmentChanged(VerticalAlignment align)
{
	switch(align)
	{
		case QTextEdit::AlignSuperScript:
		{
			m_toolbar->setButton(TBSuper, true);
			m_toolbar->setButton(TBSub, false);
			break;
		}
		case QTextEdit::AlignSubScript:
		{
			m_toolbar->setButton(TBSub, true);
			m_toolbar->setButton(TBSuper, false);
			break;
		}
		default:
		{
			m_toolbar->setButton(TBSuper, false);
			m_toolbar->setButton(TBSub, false);
		}
	}
}


}

#include "richtextdialog.moc"
