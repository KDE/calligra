/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexidbtextedit.h"
#include "kexidblineedit.h"
#include <kexidb/queryschema.h>

#include <kapplication.h>
#include <kstdaccel.h>
#include <kdebug.h>

#include <QPaintEvent>
#include <qpainter.h>

KexiDBTextEdit::KexiDBTextEdit(QWidget *parent)
 : KTextEdit(parent)
 , KexiDBTextWidgetInterface()
 , KexiFormDataItemInterface()
 , m_menuExtender(this, this)
 , m_slotTextChanged_enabled(true)
{
	setMinimumHeight(fontMetrics().height() +2);
	connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
	installEventFilter(this);
}

KexiDBTextEdit::~KexiDBTextEdit()
{
}

void KexiDBTextEdit::setInvalidState( const QString& displayText )
{
	setReadOnly(true);
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
	if (focusPolicy() & Qt::TabFocus)
		setFocusPolicy(Qt::ClickFocus);
	KTextEdit::setPlainText(displayText);
}

void KexiDBTextEdit::setValueInternal(const QVariant& add, bool removeOld)
{
//! @todo how about rich text?
	if (m_columnInfo && m_columnInfo->field->type()==KexiDB::Field::Boolean) {
//! @todo temporary solution for booleans!
		KTextEdit::setPlainText( add.toBool() ? "1" : "0" );
	}
	else {
		if (removeOld)
			KTextEdit::setPlainText( add.toString() );
		else
			KTextEdit::setPlainText( m_origValue.toString() + add.toString() );
	}
}

QVariant KexiDBTextEdit::value()
{
//! @todo how about rich text?
	return toPlainText();
}

void KexiDBTextEdit::slotTextChanged()
{
	if (!m_slotTextChanged_enabled)
		return;
	signalValueChanged();
}

bool KexiDBTextEdit::valueIsNull()
{
	return toPlainText().isNull();
}

bool KexiDBTextEdit::valueIsEmpty()
{
	return toPlainText().isEmpty();
}

bool KexiDBTextEdit::isReadOnly() const
{
	return KTextEdit::isReadOnly();
}

void KexiDBTextEdit::setReadOnly( bool readOnly )
{
	KTextEdit::setReadOnly( readOnly );
#ifdef __GNUC__
#warning TODO KexiDBTextEdit::setReadOnly() - bg color
#else
#pragma WARNING( TODO KexiDBTextEdit::setReadOnly() - bg color )
#endif
#if 0//TODO
	QPalette p = palette();
	QColor c(readOnly 
		? lighterGrayBackgroundColor(kapp->palette()) : p.color(QPalette::Normal, QColorGroup::Base));
	setPaper( c );
	p.setColor(QColorGroup::Base, c);
	p.setColor(QColorGroup::Background, c);
	setPalette( p );
#endif
}

/* Qt4 
void KexiDBTextEdit::setText( const QString & text, const QString & context )
{
	KTextEdit::setText(text, context);
}*/

QWidget* KexiDBTextEdit::widget()
{
	return this;
}

bool KexiDBTextEdit::cursorAtStart()
{
	return textCursor().atStart();
}

bool KexiDBTextEdit::cursorAtEnd()
{
	return textCursor().atEnd();
}

void KexiDBTextEdit::clear()
{
	document()->clear();
}

void KexiDBTextEdit::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
	KexiFormDataItemInterface::setColumnInfo(cinfo);
	if (!cinfo)
		return;
	KexiDBTextWidgetInterface::setColumnInfo(m_columnInfo, this);
}

void KexiDBTextEdit::paintEvent ( QPaintEvent *pe )
{
	KTextEdit::paintEvent( pe );
	QPainter p(this);
	KexiDBTextWidgetInterface::paint( this, &p, toPlainText().isEmpty(), alignment(), hasFocus() );
}

QMenu * KexiDBTextEdit::createPopupMenu(const QPoint & pos)
{
	Q_UNUSED(pos);
	QMenu *contextMenu = KTextEdit::createStandardContextMenu();//pos);
	m_menuExtender.createTitle(contextMenu);
	return contextMenu;
}

void KexiDBTextEdit::undo()
{
	cancelEditor();
}

void KexiDBTextEdit::setDisplayDefaultValue(QWidget* widget, bool displayDefaultValue)
{
	KexiFormDataItemInterface::setDisplayDefaultValue(widget, displayDefaultValue);
	// initialize display parameters for default / entered value
	KexiDisplayUtils::DisplayParameters * const params 
		= displayDefaultValue ? m_displayParametersForDefaultValue : m_displayParametersForEnteredValue;
	QPalette pal(palette());
	pal.setColor(QPalette::Active, QColorGroup::Text, params->textColor);
	setPalette(pal);
	setFont(params->font);
//! @todo support rich text...
/*	m_slotTextChanged_enabled = false;
		//for rich text...
		const QString origText( text() );
		KTextEdit::setText(QString());
		setCurrentFont(params->font);
		setColor(params->textColor);
		KTextEdit::setText(origText);
	m_slotTextChanged_enabled = true;*/
}

void KexiDBTextEdit::moveCursorToEnd()
{
	moveCursor( QTextCursor::End );
}

void KexiDBTextEdit::moveCursorToStart()
{
	moveCursor( QTextCursor::Start );
}

void KexiDBTextEdit::selectAll()
{
	KTextEdit::selectAll();
}

void KexiDBTextEdit::keyPressEvent( QKeyEvent *ke )
{
	// for instance, Windows uses Ctrl+Tab for moving between tabs, so do not steal this shortcut
	if (KStandardShortcut::tabNext().contains( QKeySequence(ke->key()|ke->modifiers()) )
		|| KStandardShortcut::tabPrev().contains( QKeySequence(ke->key()|ke->modifiers()) ) )
	{
		ke->ignore();
		return;
	}
	KTextEdit::keyPressEvent(ke);
}

#include "kexidbtextedit.moc"
