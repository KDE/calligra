/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIRELATIONVIEWCONNECTION_H
#define KEXIRELATIONVIEWCONNECTION_H

#include <qstring.h>
#include <qguardedptr.h>

class QPainter;
class KexiRelationViewTableContainer;
class KexiRelationView;
//struct SourceConnection;

class KEXIRELATIONSVIEW_EXPORT KexiRelationViewConnection
{
	public:

		KexiRelationViewConnection(KexiRelationViewTableContainer *srcTbl, KexiRelationViewTableContainer *rcvTbl, struct SourceConnection &s, KexiRelationView *parent);
		~KexiRelationViewConnection();


		/*
		   C++PROGRAMMIERER bestehen darauf, daﬂ der Elefant eine Klasse sei,
		   und somit schlieﬂlich seine Fang-Methoden selbst mitzubringen habe.

		   http://www.c-plusplus.de ;)
		*/
		void		drawConnection(QPainter *p);

		bool		selected() { return m_selected; }
		void		setSelected(bool s) { m_selected = s; }

		const QRect	connectionRect();
		const QRect	oldRect() { return m_oldRect; }

		KexiRelationViewTableContainer	*srcTable() { return m_srcTable; }
		KexiRelationViewTableContainer	*rcvTable() { return m_rcvTable; }

		bool matchesPoint(const QPoint &p, int tolerance=3);
//		SourceConnection	connection() { return m_conn; }

		QString toString() const;

	private:
		QGuardedPtr<KexiRelationViewTableContainer> m_srcTable;
		QGuardedPtr<KexiRelationViewTableContainer> m_rcvTable;
		QString				m_srcField;
		QString				m_rcvField;

		QRect				m_oldRect;

		bool				m_selected;

		QGuardedPtr<KexiRelationView> m_parent;
};

#endif
