/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDB_REFERENCE_H
#define KEXIDB_REFERENCE_H

#include <kexidb/field.h>

namespace KexiDB {

/*! KexiDB::Reference provides information about one-to-many reference between two tables.
 Reference is defined by a pair of (potentially multi-field) indices: 
 - "one" or "master" side: unique key
 - "many" or "details" side: referenced foreign key
 <pre>
 [unique, master] ----< [fk, details]
 </pre>

 In this documentation, we will call table that owns fields of "one" side as 
 "master side of the relation", and the table that owns foreign key fields of 
 as "details side of the relation".
 Use masterTable(), and detailsTable() to get one-side table and many-side table, respectively.

 Note: some engines (e.g. MySQL with InnoDB) requires that indices at both sides 
 have to be explicity created. 

 \todo (js) It is planned that this will be handled by KexiDB internally and transparently.

 Each (of the two) key can be defined (just like index) as list of fields owned by one table.
 Indeed, reference info can retrieved from Reference object in two ways:
 -# pair of indices; use masterIndex(), detailsIndex() for that
 -# ordered list of field pairs (<master-side-field, details-side-field>); use fieldPairs() for that

 No assigned objects (like fields, indices) are owned by Reference object. The exception is that 
 list of field-pairs is internally created (on demand) and owned.

 Reference object is owned by IndexSchema object (the one that is defined at master-side of the 
 reference).
 Note also that IndexSchema objects are owned by appropriate tables, so thus 
 there is implicit ownership between TableSchema and Reference.
 If Reference object is not attached to IndexSchema object, you should care about destroying it by hand.
*/

class IndexSchema;
class TableSchema;

class KEXI_DB_EXPORT Reference
{
	public:
		typedef QPtrList<Reference> List;

		/*! Creates uninitialized Reference object. 
			setIndices() will be required to call.
		*/
		Reference();

		/*! Creates Reference object and initialises it just by 
		 calling setIndices(). If setIndices() failed, object is still uninitialised.
		*/
		Reference(IndexSchema* masterIndex, IndexSchema* detailsIndex);

		virtual ~Reference();

		/*! \return index defining master side of this reference
		 or null if there is no information defined. */
		IndexSchema* masterIndex() const { return m_masterIndex; }

		/*! \return index defining referenced side of this reference.
		 or null if there is no information defined. */
		IndexSchema* detailsIndex() const { return m_detailsIndex; }

		/*! \return ordered list of field pairs -- alternative form 
		 for representation of reference or null if there is no information defined.
		 Each pair has a form of <master-side-field, details-side-field>. */
		Field::PairList* fieldPairs() { return &m_pairs; }

		/*! \return table assigned at "master / one" side of this reference.
		 or null if there is no information defined. */
		TableSchema* masterTable() const;

		/*! \return table assigned at "details / many / foreign" side of this reference.
		 or null if there is no information defined. */
		TableSchema* detailsTable() const;

		/*! Sets \a masterIndex and \a detailsIndex indices for this reference.
		 This also sets information about tables for master- and details- sides.
		 Notes: 
		 - both indices must contain the same number of fields
		 - both indices must not be owned by the same table, and table (owner) must be not null.
		 - corresponding filed types must be the same
		 - corresponding filed types' signedness must be the same
		 If above rules are not fulfilled, information about this reference is cleared. 
		 On success, this Reference object is detached from previous IndexSchema objects that were
		 assigned before, and new are attached.
		 */
		void setIndices(IndexSchema* masterIndex, IndexSchema* detailsIndex);

	protected:
		IndexSchema *m_masterIndex;
		IndexSchema *m_detailsIndex;

		Field::PairList m_pairs;

	friend class Connection;
	friend class TableSchema;
	friend class IndexSchema;
};

} //namespace KexiDB

#endif
