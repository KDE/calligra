/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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

#ifndef KEXIRELATIONMAINDLG_H
#define KEXIRELATIONMAINDLG_H

#include <KexiView.h>

class KexiRelationsView;

class KexiRelationMainDlg : public KexiView
{
	Q_OBJECT

	public:
		KexiRelationMainDlg(QWidget *parent);
		virtual ~KexiRelationMainDlg();

		virtual QSize sizeHint() const;

		virtual QWidget* mainWidget();

		virtual QString itemIcon();

	private:
		KexiRelationsView *m_rel;
};

#endif
