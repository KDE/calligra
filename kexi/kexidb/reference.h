/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

/*! KexiDB::Reference provides information about reference between two tables.
 Reference is defined by a pair: 
 - foreign key
 - referenced key.
 Later, in this documentation, we will call table that owns fields of foreign key as "foreign side of relation"
 and table that owns fields of referenced key as "referenced side of relation". Use foreignSide()
  and referencedSide() to get foreign-side-table and referenced-side-table, respectively.

 Note: some engines (e.g. MySQL with InnoDB) requires that indices at foreign and referenced 
 side are explicity created. 

 \todo (js) It is planned that this will be handled by KexiDB internally.

 Each (of the two) key can be defined (just like index) as list of fields owned by one table.
 Indeed, reference info can retrieved from Reference object in two ways:
 -# pair of indices; use foreignIndex(), referencedIndex() for that
 -# ordered list of field pairs (<foreign_field>,<referenced_field); use fieldPairs() for that

 No assigned objects (like fields, indices) are owned by Reference object. The exception is that 
 list of field-pairs is internally created (on demand) and owned.

 Reference object is owned by IndexSchema object (the one that is defined at foreign reference's side).
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
		Reference(IndexSchema* foreign, IndexSchema* referenced);

		virtual ~Reference();

		/*! \return index defining foreign side of this reference
		 or null if there is no information defined. */
		IndexSchema* foreignIndex() const { return m_index1; }

		/*! \return index defining referenced side of this reference.
		 or null if there is no information defined. */
		IndexSchema* referencedIndex() const { return m_index2; }

		/*! \return ordered list of field pairs -- alternative form 
		 for representation of reference
		 or null if there is no information defined. */
		Field::PairList* fieldPairs() { return &m_pairs; }

		/*! \return table assigned at foreign side of this reference.
		 or null if there is no information defined. */
		TableSchema* foreignSide() const;

		/*! \return table assigned at referenced side of this reference.
		 or null if there is no information defined. */
		TableSchema* referencedSide() const;

		/*! Sets \a foreign and \a referenced index for this reference.
		 This automatically also sets information about foreign- and referenced-side tables.
		 Notes: 
		 - both indices must contain the same number of fields
		 - both indices must not be owned by the same table, and table (owner) must be not null.
		 - corresponding filed types must be the same
		 - corresponding filed types' signedness must be the same
		 If above rules are not fulfilled, method information is cleared. 
		 On success, Reference object is detached from previous IndexSchema object that 
		 defined at foreign side, and is attached as a child to \a foreign IndexSchema object.
		 */
		void setIndices(IndexSchema* foreign, IndexSchema* referenced);

	protected:
		IndexSchema *m_index1;
		IndexSchema *m_index2;

		Field::PairList m_pairs;

	friend class Connection;
	friend class TableSchema;
	friend class IndexSchema;
};

} //namespace KexiDB

#endif
