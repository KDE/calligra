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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIRELATIONVIEWCONNECTION_H
#define KEXIRELATIONVIEWCONNECTION_H

#include <qstring.h>
#include <qpointer.h>
#include <kexi_export.h>

class QPainter;
class KexiRelationViewTableContainer;
class KexiRelationView;

class KEXIRELATIONSVIEW_EXPORT KexiRelationViewConnection
{
	public:

		KexiRelationViewConnection(KexiRelationViewTableContainer *masterTbl, 
			KexiRelationViewTableContainer *detailsTbl, struct SourceConnection &s,
			KexiRelationView *parent);
		~KexiRelationViewConnection();


		/*
		   C++PROGRAMMIERER bestehen darauf, da�der Elefant eine Klasse sei,
		   und somit schlie�ich seine Fang-Methoden selbst mitzubringen habe.

		   http://www.c-plusplus.de ;)
		*/
		void		drawConnection(QPainter *p);

		bool		selected() { return m_selected; }
		void		setSelected(bool s) { m_selected = s; }

		const QRect	connectionRect();
		const QRect	oldRect() const { return m_oldRect; }

		KexiRelationViewTableContainer	*masterTable() { return m_masterTable; }
		KexiRelationViewTableContainer	*detailsTable() { return m_detailsTable; }
		QString masterField() const { return m_masterField; }
		QString detailsField() const { return m_detailsField; }


		bool matchesPoint(const QPoint &p, int tolerance=3);
//		SourceConnection	connection() { return m_conn; }

		QString toString() const;

	private:
		QPointer<KexiRelationViewTableContainer> m_masterTable;
		QPointer<KexiRelationViewTableContainer> m_detailsTable;
		QString				m_masterField;
		QString				m_detailsField;
		QRect				m_oldRect;
		bool				m_selected;
		QPointer<KexiRelationView> m_parent;
};

#endif
