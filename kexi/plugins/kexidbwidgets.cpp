/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#include <kdebug.h>

#include "kexidbwidgets.h"

/*
 * KexiDBLineEdit
 */

KexiDBLineEdit::KexiDBLineEdit(QWidget *parent, const char *name)
 : QLineEdit(parent, name)
{
}

QString
KexiDBLineEdit::dataSource() const
{
	return m_source;
}

void
KexiDBLineEdit::setDataSource(const QString &source)
{
	kdDebug() << "KexiDBLineEdit::setDataSource(): " << source << endl;
	setText(source);
	m_source = source;
}

void
KexiDBLineEdit::setDBContent(const QString &value)
{
//	m_content = value;
	setText(value);
}

QString
KexiDBLineEdit::content() const
{
//	return m_content;
	return QString();
}

KexiDBLineEdit::~KexiDBLineEdit()
{
}


/*
 * KexiDBLabel
 */

KexiDBLabel::KexiDBLabel(QWidget *parent, const char *name)
 : QLabel("", parent, name)
{
}

KexiDBLabel::~KexiDBLabel()
{
}

QString
KexiDBLabel::dataSource() const
{
	return m_source;
}

void
KexiDBLabel::setDataSource(const QString &source)
{
	setText(source);
	m_source = source;
}

#include "kexidbwidgets.moc"

