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

#ifndef KEXIINPUTTABLEEDIT_H
#define KEXIINPUTTABLEEDIT_H

#include <klineedit.h>
#include <qvariant.h>

#include "kexitableedit.h"

class KEXIDATATABLE_EXPORT KexiInputTableEdit : public KexiTableEdit
{
	Q_OBJECT

	public:
		KexiInputTableEdit(QVariant value, int type, QString ov=QString::null, bool mark=false,
		 QWidget *parent=0, const char *name=0, QStringList comp = QStringList());

		virtual QVariant value();
		virtual bool eventFilter(QObject* watched, QEvent* e);
		void end(bool mark);
		void backspace();
		void clear();

	protected slots:
		void setRestrictedCompletion();
		void completed(const QString &);


	protected:
		void showHintButton();

		int	m_type;
		QVariant	m_value;

		bool		m_calculatedCell;
		QStringList	m_comp;

	private:
		KLineEdit	*m_cview;

	signals:
		void hintClicked();
};

#endif
