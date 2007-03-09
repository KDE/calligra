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

#include "richtextdialog.h"

#include <QLayout>
#include <QAction>
#include <Q3VBoxLayout>

#include <ktoolbar.h>
#include <kfontrequester.h>
#include <kcolorcombo.h>
#include <kdebug.h>
#include <klocale.h>
#include <KIcon>

using namespace KFormDesigner;

//////////////////////////////////////////////////////////////////////////////////
//////////////// A simple dialog to edit rich text   ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

RichTextDialog::RichTextDialog(QWidget *parent, const QString &text)
: KDialog(parent)
{
	setObjectName("richtext_dialog");
	setModal(true);
	setCaption(i18n("Edit Rich Text"));
	setButtons( KDialog::Ok | KDialog::Cancel );
	setDefaultButton( KDialog::Ok );
	
	QFrame *frame = new QFrame(this);
	setMainWidget(frame);
	Q3VBoxLayout *l = new Q3VBoxLayout(frame);
	l->setAutoAdd(true);

	m_toolbar = new KToolBar(frame);

	m_fontCombo = new KFontRequester(m_toolbar);
	m_fontComboAction = m_toolbar->addWidget(/*js: TBFont, 40,*/ m_fontCombo);
	connect(m_fontCombo, SIGNAL(textChanged(const QString&)), 
		this, SLOT(changeFont(const QString &)));

	m_toolbar->addSeparator();

	m_colorCombo = new KColorCombo(m_toolbar);
	m_colorComboAction = m_toolbar->addWidget(/*js: TBColor, 30,*/ m_colorCombo);
	connect(m_colorCombo, SIGNAL(activated(const QColor&)), this, SLOT(changeColor(const QColor&)));

	m_boldTextAction = m_toolbar->addAction(KIcon("format-text-bold"), i18n("Bold"));
	m_boldTextAction->setCheckable(true);
	m_italicTextAction = m_toolbar->addAction(KIcon("format-text-italic"), i18n("Italic"));
	m_italicTextAction->setCheckable(true);
	m_underlineTextAction = m_toolbar->addAction(KIcon("format-text-underline"), i18n("Underline"));
	m_underlineTextAction->setCheckable(true);
	m_toolbar->addSeparator();

	m_superscriptTextAction = m_toolbar->addAction(KIcon("text_super"), i18n("Superscript"));
	m_superscriptTextAction->setCheckable(true);
	m_subscriptTextAction = m_toolbar->addAction(KIcon("text_sub"), i18n("Subscript"));
	m_subscriptTextAction->setCheckable(true);
	m_toolbar->addSeparator();

	m_alignActionGroup = new QActionGroup(this);
	m_alignLeftAction = m_toolbar->addAction(KIcon("text_left"), i18n("Left Align"));
	m_alignLeftAction->setCheckable(true);
	m_alignActionGroup->addAction(m_alignLeftAction);
	m_alignCenterAction = m_toolbar->addAction(KIcon("text_center"), i18n("Centered"));
	m_alignCenterAction->setCheckable(true);
	m_alignActionGroup->addAction(m_alignCenterAction);
	m_alignRightAction = m_toolbar->addAction(KIcon("text_right"), i18n("Right Align"));
	m_alignRightAction->setCheckable(true);
	m_alignActionGroup->addAction(m_alignRightAction);
	m_alignJustifyAction = m_toolbar->addAction(KIcon("format-justify-fill"), i18n("Justified"));
	m_alignJustifyAction->setCheckable(true);
	m_alignActionGroup->addAction(m_alignJustifyAction);

	connect(m_toolbar, SIGNAL(actionTriggered(QAction*)),
		this, SLOT(slotActionTriggered(QAction*)));

	m_edit = new KTextEdit(text, frame);
	m_edit->setAcceptRichText(true);

/* not needed, slotCurrentCharFormatChanged() handles this
	connect(m_edit, SIGNAL(cursorPositionChanged()),
		this, SLOT(cursorPositionChanged()));*/
//	connect(m_edit, SIGNAL(clicked(int, int)),
//		this, SLOT(cursorPositionChanged(int, int)));
	connect(m_edit, SIGNAL(currentCharFormatChanged(const QTextCharFormat&)),
		this, SLOT(slotCurrentCharFormatChanged(const QTextCharFormat&)));

	m_edit->moveCursor(QTextCursor::End);
	slotCurrentCharFormatChanged(m_edit->currentCharFormat());
	//cursorPositionChanged();
//	m_edit->show();
//	frame->show();
	m_edit->setFocus();
}

RichTextDialog::~RichTextDialog()
{
}

QString
RichTextDialog::text() const
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
RichTextDialog::slotActionTriggered(QAction* action)
{
	const bool isChecked = action->isChecked();

	if (action==m_boldTextAction)
		m_edit->setBold(isChecked);
	else if (action==m_italicTextAction)
		m_edit->setItalic(isChecked);
	else if (action==m_underlineTextAction)
		m_edit->setUnderline(isChecked);
	else if (action==m_superscriptTextAction) {
		if (isChecked && m_subscriptTextAction->isChecked()) {
			m_subscriptTextAction->setChecked(false);
			QTextCharFormat currentCharFormat = m_edit->currentCharFormat();
			currentCharFormat.setVerticalAlignment(
				isChecked ? QTextCharFormat::AlignSuperScript : QTextCharFormat::AlignNormal);
			m_edit->setCurrentCharFormat(currentCharFormat);
		}
	}
	else if (action==m_superscriptTextAction) {
		if (isChecked && m_superscriptTextAction->isChecked()) {
			m_superscriptTextAction->setChecked(false);
			QTextCharFormat currentCharFormat = m_edit->currentCharFormat();
			currentCharFormat.setVerticalAlignment(
				isChecked ? QTextCharFormat::AlignSubScript : QTextCharFormat::AlignNormal);
			m_edit->setCurrentCharFormat(currentCharFormat);
		}
	}
	else if (action==m_alignLeftAction) {
		if (isChecked)
			m_edit->setAlignment(Qt::AlignLeft);
	}
	else if (action==m_alignCenterAction) {
		if (isChecked)
			m_edit->setAlignment(Qt::AlignCenter);
	}
	else if (action==m_alignRightAction) {
		if (isChecked)
			m_edit->setAlignment(Qt::AlignRight);
	}
	else if (action==m_alignJustifyAction) {
		if (isChecked)
			m_edit->setAlignment(Qt::AlignJustify);
	}
}

/* not needed, slotCurrentCharFormatChanged() handles this
void
RichTextDialog::cursorPositionChanged()
{
	m_fontCombo->setCurrentFont(m_edit->currentFont().family());
	m_colorCombo->setColor(m_edit->color());
	m_boldTextAction->setChecked(m_edit->bold());
	m_italicTextAction->setChecked(m_edit->italic());
	m_underlineTextAction->setChecked(m_edit->underline());

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
}*/

void
RichTextDialog::slotCurrentCharFormatChanged(const QTextCharFormat& f)
{
	m_superscriptTextAction->setChecked(f.verticalAlignment()==QTextCharFormat::AlignSuperScript);
	m_subscriptTextAction->setChecked(f.verticalAlignment()==QTextCharFormat::AlignSubScript);

	switch (m_edit->alignment()) {
	case Qt::AlignLeft:
		m_alignLeftAction->setChecked(true);
		break;
	case Qt::AlignCenter:
		m_alignCenterAction->setChecked(true);
		break;
	case Qt::AlignRight:
		m_alignRightAction->setChecked(true);
		break;
	case Qt::AlignJustify:
		m_alignJustifyAction->setChecked(true);
		break;
	}
	
//! @todo add more formatting options (buttons)
}

#include "richtextdialog.moc"
