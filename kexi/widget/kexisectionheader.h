/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXISECTIONHEADER_H
#define KEXISECTIONHEADER_H

#include <qlabel.h>
#include <qlayout.h>

class BoxLayout;

class KEXIEXTWIDGETS_EXPORT KexiSectionHeader : public QWidget
{
	Q_OBJECT
	public:
		KexiSectionHeader(const QString &caption, Orientation o, 
			QWidget* parent = 0, const char * name = 0 );

		virtual bool eventFilter( QObject *o, QEvent *e );

		virtual QSize sizeHint() const;

	public slots:
		void slotFocus(bool in);

	protected:
		Orientation m_orientation;
		QLabel *m_lbl;
		BoxLayout *m_lyr;
};

#endif

