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

#ifndef KEXIRELATIONVIEWCONNECTION_H
#define KEXIRELATIONVIEWCONNECTION_H

#include <qstring.h>


class QPainter;
class KexiRelationViewTableContainer;


class KexiRelationViewConnection
{
	public:
	
//		KexiRelationViewConnection(KexiRelationViewTable *srcTbl, KexiRelationViewTable *rcvTbl,
//		  const QString &srcFld, const QString &rcvFld);
		KexiRelationViewConnection(KexiRelationViewTableContainer *srcTbl,KexiRelationViewTableContainer *rcvTbl,
		  const QString &srcFld, const QString &rcvFld);
		~KexiRelationViewConnection();


		/*
		   C++PROGRAMMIERER bestehen darauf, daﬂ der Elefant eine Klasse sei,
		   und somit schlieﬂlich seine Fang-Methoden selbst mitzubringen habe.
		   
		   http://www.c-plusplus.de ;)
		*/
		void		drawConnection(QPainter *p, QWidget *parent);
		const QRect	connectionRect();

		KexiRelationViewTableContainer	*srcTable() { return m_srcTable; }
		KexiRelationViewTableContainer	*rcvTable() { return m_rcvTable; }

	private:
		KexiRelationViewTableContainer	*m_srcTable;
		KexiRelationViewTableContainer	*m_rcvTable;
		QString				m_srcField;
		QString				m_rcvField;
};

#endif
