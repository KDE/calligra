/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#ifndef KEXIDBWIDGETS_H
#define KEXIDBWIDGETS_H

#include <qlineedit.h>
#include <qlabel.h>
#include <qvariant.h>

class KEXI_HAND_FORM_EXPORT KexiDBLineEdit : public QLineEdit
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(QString dbdata READ content WRITE setDBContent DESIGNABLE true)

	public:
		KexiDBLineEdit(QWidget *parent=0, const char *name=0);
		~KexiDBLineEdit();

		QString		dataSource() const;
		QString		content() const;
		void		setDataSource(const QString &source);
		void		setDBContent(const QString &value);

	protected:
		QString		m_source;
		QVariant	m_content;
};


class KEXI_HAND_FORM_EXPORT KexiDBLabel : public QLabel
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)

	public:
		KexiDBLabel(QWidget *parent=0, const char *name=0);
		~KexiDBLabel();

		QString dataSource() const;
		void	setDataSource(const QString &source);

	protected:
		QString	m_source;
};

#endif

