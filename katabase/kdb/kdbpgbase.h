/***************************************************************************
                          pgbase.h  -  description                              
                             -------------------                                         
    begin                : Sat Jun 5 1999                                           
    copyright            : (C) 1999 by Ørn E. Hansen                         
    email                : hanseno@mail.bip.net                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/


#ifndef KDB_PGBASE_H
#define KDB_PGBASE_H

#include <kdbdatabase.h>
#include <kdbcriteria.h>

/** Realisation of physical means to access a postgres database.
	*
	* The abstract class of a dataBase is realised with this subclass
	* to provide the physical means to access a postgres database
	* through SQL.
	*
  *@author Ørn E. Hansen
  *@short Realisation of database methods, for Postgres SQL.
  */

class kdbPgBase : public kdbDataBase  {
private:
	/** A private method, that breaks up the field set into a
		* list of fields in a comma separated string.
	*/
	QString fieldList(kdbFieldSet *,bool);

public:
	/**
	 * Construction of the database, requires a database name
	 * to connect to.
	 */
	kdbPgBase(const QString&, const QString&, const QString& p_port="5432");
	/** Destroy the class, and free any memory it occupies. */
	~kdbPgBase();

	/**
	 * Realisation of an abstract method, that creates the
	 * SQL command to successfully create a table.
	 */
	bool create(const QString&,kdbFieldSet*);
	/** Realisation of the abstract method, which uses SQL
		* commands to insert a record into a table.  It takes two
		* parameters, the name of the table, and the fields to
		* be set on insert.
	*/
	bool insert(const QString&,kdbFieldSet*);
	/**
	 * Insert all fields in the dataset, that are new, into the
	 * physical data table.  Update all references in the
	 * dataset, to contain information on the object id for
	 * future reference.
	 */
	bool insert(kdbDataSet*);
	/**
	 * Update all fields in the datatable, to resemble the ones
	 * in the dataset.
	 */
	bool update(kdbDataSet*);
	/**
	 * Remove all records from the physical data table, that are
	 * marked for deletion (with negative recid) in the dataset.
	 */
	bool remove(kdbDataSet*);
	/**
	 * Realizes the abstract method, to remove a table from the
	 * database.
	 */
	bool remove(const QString&);
	/**
	 * Make an SQL command for selecting the data, for the query
	 * and creates a cursor for it.  Then loop over the cursor and
	 * insert all selected records into the dataset, specified in
	 * the query.
	 */
	bool query(kdbDataSet*);
	/**
	 * Make a SQL select statement to the given table, resulting in
	 * no tuples.  All field names in the table, or those specified
	 * in the fieldset, are then initialized with name, type and
	 * length information.
	 */
	bool getFields(kdbDataSet*);
};

#endif






















