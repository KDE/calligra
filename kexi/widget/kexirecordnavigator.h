/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
                 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIRECORDNAVIGATOR_H
#define KEXIRECORDNAVIGATOR_H

#include <qframe.h>

class QToolButton;
class KLineEdit;
class QIntValidator;
namespace KexiDB
{
	class Cursor;
}

class KEXIEXTWIDGETS_EXPORT KexiRecordNavigator : public QFrame
{
	Q_OBJECT

	public:
		KexiRecordNavigator(KexiDB::Cursor *cursor, QWidget *parent, const char *name=0);
		~KexiRecordNavigator();

	private:
		QToolButton *m_navBtnFirst;
		QToolButton *m_navBtnPrev;
		QToolButton *m_navBtnNext;
		QToolButton *m_navBtnLast;
		QToolButton *m_navBtnNew;
		KLineEdit *m_navRowNumber;
		QIntValidator *m_navRowNumberValidator;
		KLineEdit *m_navRowCount; //!< readonly counter
		int m_nav1DigitWidth;
};

#endif
