/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#include <kexidb/reference.h>

#include <kexidb/indexschema.h>
#include <kexidb/tableschema.h>
#include <kexidb/driver.h>

#include <kdebug.h>

using namespace KexiDB;

Reference::Reference()
	: m_masterIndex(0)
	, m_detailsIndex(0)
{
	m_pairs.setAutoDelete(true);
}

Reference::Reference(IndexSchema* masterIndex, IndexSchema* detailsIndex)
	: m_masterIndex(0)
	, m_detailsIndex(0)
{
	m_pairs.setAutoDelete(true);
	setIndices(masterIndex, detailsIndex);
}

Reference::~Reference()
{
}

TableSchema* Reference::masterTable() const
{
	return m_masterIndex ? m_masterIndex->table() : 0;
}

TableSchema* Reference::detailsTable() const
{
	return m_detailsIndex ? m_detailsIndex->table() : 0;
}

void Reference::setIndices(IndexSchema* masterIndex, IndexSchema* detailsIndex)
{
	m_masterIndex = 0;
	m_detailsIndex = 0;
	m_pairs.clear();
	if (!masterIndex || !detailsIndex || !masterIndex->table() || !detailsIndex->table() 
	|| masterIndex->table()==detailsIndex->table() || masterIndex->fieldCount()!=detailsIndex->fieldCount())
		return;
	Field *f1 = masterIndex->fields()->first();
	Field *f2 = detailsIndex->fields()->first();
	while (f1 && f2) {
		if (f1->type()!=f1->type()) {
			KexiDBDbg << "Reference::setIndices(INDEX on '"<<masterIndex->table()->name()
			<<"',INDEX on "<<detailsIndex->table()->name()<<"): !equal field types: "
			<<Driver::defaultSQLTypeName(f1->type())<<" "<<f1->name()<<", "
			<<Driver::defaultSQLTypeName(f2->type())<<" "<<f2->name() <<endl;
			m_pairs.clear();
			return;
		}
		if ((f1->isUnsigned() && !f2->isUnsigned()) || (!f1->isUnsigned() && f1->isUnsigned())) {
			KexiDBDbg << "Reference::setIndices(INDEX on '"<<masterIndex->table()->name()
			<<"',INDEX on "<<detailsIndex->table()->name()<<"): !equal signedness of field types: "
			<<Driver::defaultSQLTypeName(f1->type())<<" "<<f1->name()<<", "
			<<Driver::defaultSQLTypeName(f2->type())<<" "<<f2->name() <<endl;
			m_pairs.clear();
			return;
			
		}
		m_pairs.append( new Field::Pair(f1,f2) );
	}
	//ok: update information
	if (m_masterIndex) {//detach yourself
		m_masterIndex->detachReference(this);
	}
	if (m_detailsIndex) {//detach yourself
		m_detailsIndex->detachReference(this);
	}
	m_masterIndex = masterIndex;
	m_detailsIndex = detailsIndex;
	m_masterIndex->attachReference(this);
	m_detailsIndex->attachReference(this);
}

