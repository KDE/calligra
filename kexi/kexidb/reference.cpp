/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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
	: m_index1(0)
	, m_index2(0)
{
	m_pairs.setAutoDelete(true);
}

Reference::Reference(IndexSchema* foreign, IndexSchema* referenced)
	: m_index1(0)
	, m_index2(0)
{
	m_pairs.setAutoDelete(true);
	setIndices(foreign, referenced);
}

Reference::~Reference()
{
}

TableSchema* Reference::foreignSide() const
{
	return m_index1 ? m_index1->table() : 0;
}

TableSchema* Reference::referencedSide() const
{
	return m_index2 ? m_index2->table() : 0;
}

void Reference::setIndices(IndexSchema* foreign, IndexSchema* referenced)
{
	m_index1 = 0;
	m_index2 = 0;
	m_pairs.clear();
	if (!foreign || !referenced || !foreign->table() || !referenced->table() 
	|| foreign->table()==referenced->table() || foreign->fieldCount()!=referenced->fieldCount())
		return;
	Field *f1 = foreign->fields()->first();
	Field *f2 = referenced->fields()->first();
	while (f1 && f2) {
		if (f1->type()!=f1->type()) {
			KexiDBDbg << "Reference::setIndices(INDEX on '"<<foreign->table()->name()
			<<"',INDEX on "<<referenced->table()->name()<<"): !equal field types: "
			<<Driver::defaultSQLTypeName(f1->type())<<" "<<f1->name()<<", "
			<<Driver::defaultSQLTypeName(f2->type())<<" "<<f2->name() <<endl;
			m_pairs.clear();
			return;
		}
		if ((f1->isUnsigned() && !f2->isUnsigned()) || (!f1->isUnsigned() && f1->isUnsigned())) {
			KexiDBDbg << "Reference::setIndices(INDEX on '"<<foreign->table()->name()
			<<"',INDEX on "<<referenced->table()->name()<<"): !equal signedness of field types: "
			<<Driver::defaultSQLTypeName(f1->type())<<" "<<f1->name()<<", "
			<<Driver::defaultSQLTypeName(f2->type())<<" "<<f2->name() <<endl;
			m_pairs.clear();
			return;
			
		}
		m_pairs.append( new Field::Pair(f1,f2) );
	}
	//ok: update information
	if (m_index1) {//detach yourself
		m_index1->detachReference(this);
	}
	if (m_index2) {//detach yourself
		m_index2->detachReference(this);
	}
	m_index1 = foreign;
	m_index2 = referenced;
	m_index1->attachReference(this);
	m_index2->attachReference(this);
}

