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

#ifndef KEXIDB_CONNECTION_H
#define KEXIDB_CONNECTION_H

#include <qobject.h>
#include <qstringlist.h>
#include <qintdict.h>
#include <qdict.h>
#include <qptrdict.h>
#include <qvaluevector.h>
#include <qvaluelist.h>
#include <qvariant.h>
#include <qguardedptr.h>

#include <kexidb/object.h>
#include <kexidb/connectiondata.h>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>
#include <kexidb/transaction.h>

namespace KexiDB {

//! structure for storing single record with type information
typedef QValueVector<QVariant> RowData; 

class Cursor;
class Driver;
class ConnectionPrivate;
class RowEditBuffer;

/*! This class represents database connection established with given database source.
*/
class KEXI_DB_EXPORT Connection : public QObject, public KexiDB::Object
{
	Q_OBJECT

	public:

		/*! Opened connection is automatically disconnected and removed 
		 from driver's connections list. 
		 Note for driver developers: you should call destroy() 
		 from you Connection's subclass destructor. */
		virtual ~Connection();

		/*! \return parameters that had been used for create this connection. */
		ConnectionData* data() { return m_data; }

		/*! return driver that is source for this connection. */
		Driver* driver() const { return m_driver; }

		/*! Connects to driver with given parameters. 
		 \return true if successfull. */
		bool connect();

		/*! \return true, if connection is properly estableshed. */
		bool isConnected() { return m_is_connected; }

		/*! \return true, both if connection is properly estableshed
		 and any database within this connection is properly used
		 with useDatabase(). */
		bool isDatabaseUsed();
		
		/*! Disconnects to driver with given parameters. 
		 Database (if used) is closed, and any active transactions 
		 (if supported) are rolled back, so commit these before disconnecting,
		 if you'd like to save your changes. */
		bool disconnect();

		/*! \return list of database names for opened connection.
		 If \a also_system_db is true, system database's names are also returned. */
		QStringList databaseNames(bool also_system_db = false);

		/*! \return true if database \a dbName exists.
		 If \a ignoreErrors if true, error flag of connection 
		  won't be modified for any errors (it will quietly return),
		  else (ignoreErrors == false) we can check why db does 
		  not exist using error(), errorNum() and/or errorMsg(). */
		bool databaseExists( const QString &dbName, bool ignoreErrors = true );

		/*! Creates new database with name \a dbName, using this connection.
		 If database with \a dbName already exists, or other error occured,
		 false is returned. 
		 For file-based drivers, \a dbName should be equal to filename
		 (the same as specified for ConnectionData).
		 \sa useDatabase() */
		bool createDatabase( const QString &dbName );

		/*! Opens existing database \a dbName using this connection.
		 For file-based drivers, \a dbName should be equal to filename
		 (the same as specified for ConnectionData). */
		bool useDatabase( const QString &dbName );

		/*! Closes currently used database for this connection.
		 Any active transactions (if supported) are rolled back,
		 so commit these before closing, if you'd like to save your changes. */
		bool closeDatabase();

		/*! \return name of currently used database for this connection or empty string
			if there is no used database */
		QString currentDatabase() { return m_usedDatabase; }

		/*! Drops database with name \a dbName, using this connection.
		 if dbName is not specified, currently used database name is used
		 (it is closed before dropping).
		*/
		bool dropDatabase( const QString &dbName = QString::null );

		/*! \return names of all table schemas stored in currently 
		 used database. If \a also_system_tables is true, 
		 internal KexiDB system table names (kexi__*) are also returned.
		 \sa kexiDBSystemTableNames() */
		QStringList tableNames(bool also_system_tables = false);
		
		/*! \return list of internal KexiDB system table names 
		 (kexi__*). This does not mean that these tables can be found
		 in currently opened database. Just static list of table 
		 names is returned. 
		 
		 The list contents may depend on KexiDB library version;
		 opened database can contain fewer 'system' tables than in current
		 KexiDB implementation, if the current one is newer than the one used 
		 to build the database. */
		static const QStringList& kexiDBSystemTableNames();
		
		/*! \return ids of all table schema names stored in currently 
		 used database. These ids can be later used as argument for tableSchema().
		 This is a shortcut for objectIds(TableObjectType).
		 If \a also_system_tables is true, 
		 Internal KexiDB system tables (kexi__*) are not available here 
		 because these have no identifiers assigned (more formally: id=-1). */
		QValueList<int> tableIds();

		/*! \return ids of all database query schemas stored in currently 
		 used database. These ids can be later used as argument for querySchema().
		 This is a shortcut for objectIds(TableObjectType). */
		QValueList<int> queryIds();

		/*! \return names of all schemas of object with \a objType type 
		 that are stored in currently used database. */
		QValueList<int> objectIds(int objType);

		/*! Creates new transaction handle and starts new transaction.
		 \return KexiDB::Transaction object if transaction has been started 
		 successfully, otherwise null transaction. 
		 For drivers that allow single transaction per connection
		 (Driver::features() && SingleTransactions) this method can be called one time,
		 and then this single transaction will be default ( setDefaultTransaction() will be called).
		 For drivers that allow multiple transactions per connection, no default transaction is set
		 automatically in beginTransaction() method, you could do this by hand.
		 \sa setDefaultTransaction(), defaultTransaction().
		*/
		Transaction beginTransaction();

/*! \todo for nested transactions:
		Tansaction* beginTransaction(transaction *parent_transaction);
*/
		/*! Commits transaction \a trans.
		 If there is not \a trans argument passed, and there is default transaction 
		 (obtained from defaultTransaction()) defined, this one will be commited.
		 If default is not present, false is returned (when ignore_inactive is 
		 false, the default), or true is returned (when ignore_inactive is true).
		 
		 On successfull commit, \a trans object will be destroyed.
		 If this was default transaction, there is no default transaction for now.
		*/
		bool commitTransaction( Transaction trans = Transaction::null,
			bool ignore_inactive = false );
		
		/*! Rollbacks transaction \a trans.
		 If there is not \a trans argument passed, and there is default transaction 
		 (obtained from defaultTransaction()) defined, this one will be rolled back.
		 If default is not present, false is returned (when ignore_inactive is 
		 false, the default), or true is returned (when ignore_inactive is true).
		 
		 or any error occured, false is returned.
			
		 On successfull rollback, \a trans object will be destroyed.
		 If this was default transaction, there is no default transaction for now.
		*/
		bool rollbackTransaction( Transaction trans = Transaction::null,
			bool ignore_inactive = false );
		
		/*! \return handle for default transaction for this connection
		 or null transaction if there is no such a transaction defined. 
		 If transactions are supported: Any operation on database (e.g. inserts)
		 that is started without specifing transaction context, will be performed
		 in the context of this transaction.
		 
		 Returned null transaction doesn't mean that there is no transactions 
		 started at all.
		 Default transaction can be defined automatically for some drivers --
		 see beginTransaction(). 
		 \sa KexiDB::Driver::transactionsSupported()
		*/
		Transaction& defaultTransaction() const;

		/*! Sets default transaction that will be used as context for operations
		 on data in opened database for this connection. */
		void setDefaultTransaction(const Transaction& trans);
		
		/*! \return set of handles of currently active transactions.
		 Note that in multithreading environment some of these 
		 transactions can be already inactive after calling this method.
		 Use Transaction::active() to check that. Inactive transaction 
		 handle is useless and can be safely dropped.
		*/
		const QValueList<Transaction>& transactions();

		/*! \return true if "auto commit" option is on. 

		 When auto commit is on (the default on for any new Connection object),
		 every sql functional statement (statement that changes 
		 data in the database implicitly starts a new transaction. 
		 This transaction is automatically commited 
		 after successfull statement execution or rolled back on error.
		 
		 For drivers that do not support transactions (see Driver::features())
		 this method shouldn't be called because it does nothing ans always returns false.
		 
		 No internal KexiDB object should changes this option, although auto commit's
		 behaviour depends on database engine's specifics. Engines that support only single
		 transaction per connection (see Driver::SingleTransactions),
		 use this single connection for autocommiting, so if there is already transaction 
		 started by the KexiDB user program (with beginTransaction()), this transaction 
		 is commited before any sql functional statement execution. In this situation
		 default transaction is also affected (see defaultTransaction()).
		 
		 Only for drivers that support nested transactions (Driver::NestedTransactions),
		 autocommiting works independently from previously started transaction,
		 
		 For other drivers set this option off if you need use transaction 
		 for grouping more statements together.
		  
		 NOTE: nested transactions are not yet implemented in KexiDB API.
		*/
		bool autoCommit() const;

		/*! Changes auto commit option. This does not affect currently started transactions.
		 This option can be changed even when connection is not established.
		 \sa autoCommit() */
		bool setAutoCommit(bool on);

		/*! driver-specific string escaping */
//js: MOVED TO Driver		virtual QString escapeString(const QString& str) const = 0;
//		virtual QCString escapeString(const QCString& str) const = 0;
		
		/*! Prepares query described by raw \a statement. 
		 \return opened cursor created for results of this query 
		 or NULL if there was any error. Cursor can have optionally applied \a cursor_options
		 (one of more selected from KexiDB::Cursor::Options).
		 Preparation means that returned cursor is created but not opened.
		 Open this when you would like to do it with Cursor::open().

		 Note for driver developers: you should initialize cursor engine-specific 
		 resources and return Cursor subclass' object 
		 (passing \a statement and \a cursor_options to it's constructor).
		*/
		virtual Cursor* prepareQuery( const QString& statement, uint cursor_options = 0) = 0;

		/*! \overload prepareQuery( const QString& statement = QString::null, uint cursor_options = 0)
		 Prepares query described by \a query schema. 

		 Note for driver developers: you should initialize cursor engine-specific 
		 resources and return Cursor subclass' object 
		 (passing \a query and \a cursor_options to it's constructor).
		*/
		virtual Cursor* prepareQuery( QuerySchema& query, uint cursor_options = 0 ) = 0;

		/*! \overload prepareQuery( const QString& statement = QString::null, uint cursor_options = 0)
		 Statement is build from data provided by \a query schema.
		*/
//		Cursor* prepareQuery( QuerySchema& query, uint cursor_options = 0);

		/*! \overload prepareQuery( const QString& statement = QString::null, uint cursor_options = 0)
		 Statement is build from data provided by \a table schema, 
		 it is like "select * from table_name".*/
		Cursor* prepareQuery( TableSchema& table, uint cursor_options = 0);

		/*! Executes query described by \a statement.
		 \return opened cursor created for results of this query
		 or NULL if there was any error on the cursor creation or opening.
		 Cursor can have optionally applied \a cursor_options 
		 (one of more selected from KexiDB::Cursor::Options).*/
		Cursor* executeQuery( const QString& statement, uint cursor_options = 0 );

		/*! \overload executeQuery( const QString& statement, uint cursor_options = 0 )
		 Statement is build from data provided by \a query schema.*/
		Cursor* executeQuery( QuerySchema& query, uint cursor_options = 0 );

		/*! \overload executeQuery( const QString& statement, uint cursor_options = 0 )
		 Statement is build from data provided by \a table schema, 
		 it is like "select * from table_name".*/
		Cursor* executeQuery( TableSchema& table, uint cursor_options = 0 );

		/*! Deletes cursor \a cursor previously created by functions like executeQuery() 
		 for this connection.
		 There is an attempt to close the cursor with Cursor::close() if it was opened. 
		 Anyway, at last cursor is deleted. 
		 \returns true if cursor is properly closed before deletion. */
		bool deleteCursor(Cursor *cursor);

		/*! \return schema of a table pointed by \a tableId, retrieved from currently 
		 used database. The schema is cached inside connection, 
		 so retrieval is performed only once, on demand. */
		TableSchema* tableSchema( const int tableId );
		
		/*! \return schema of a table pointed by \a tableName, retrieved from currently 
		 used database. KexiDB system table schema can be also retrieved.
		 \sa tableSchema( const int tableId ) */
		TableSchema* tableSchema( const QString& tableName );
		
		/*! \return schema of \a tablename table retrieved from currently 
		 used database. */
		QuerySchema* querySchema( const int queryId );

//js: MOVED TO Driver		QString valueToSQL( const Field::Type ftype, const QVariant& v ) const;
//		QString valueToSQL( const Field *field, const QVariant& v ) const;

		/*! Executes \a sql query and stores first record's data inside \a data.
		 This is convenient method when we need only first recors from query result,
		 or when we know that query result has only one record.
		 \return true if query was successfully executed and first record has been found. */
		bool querySingleRecord(const QString& sql, KexiDB::RowData &data);

		//PROTOTYPE:
		#define A , const QVariant&
		#define H_INS_REC(args) bool insertRecord(TableSchema &tableSchema args)
		#define H_INS_REC_ALL \
		H_INS_REC(A); \
		H_INS_REC(A A); \
		H_INS_REC(A A A); \
		H_INS_REC(A A A A); \
		H_INS_REC(A A A A A); \
		H_INS_REC(A A A A A A); \
		H_INS_REC(A A A A A A A); \
		H_INS_REC(A A A A A A A A)
		H_INS_REC_ALL;
		
		#undef H_INS_REC
		#define H_INS_REC(args) bool insertRecord(FieldList& fields args)

		H_INS_REC_ALL;
		#undef H_INS_REC_ALL
		#undef H_INS_REC
		#undef A
		
		bool insertRecord(TableSchema &tableSchema, QValueList<QVariant>& values);
		
		bool insertRecord(FieldList& fields, QValueList<QVariant>& values);
		
		/*! Creates table defined by \a tableSchema.
		 Schema information is also added into kexi system tables, for later reuse.
		 \a tableSchema object is inserted to Connection structures - it is
		 owned by Connection object now, so you shouldn't destroy the tableSchema 
		 object by hand (or declare it as local-scope variable). 
		*/
		bool createTable( KexiDB::TableSchema* tableSchema );

		/*! Drops table defined by \a tableSchema.
		 Schema information \a tableSchema is destoyed (because it's owned), so don't keep this anymore!
		*/
//TODO(js): update any structure (e.g. query) that depend on this table!
		bool dropTable( KexiDB::TableSchema* tableSchema );

		/*! It is a convenience function, does exactly the same as 
		 bool dropTable( KexiDB::TableSchema* tableSchema ) */
		bool dropTable( const QString& table );

		/*! \return first field from \a fieldlist that has system name, 
		 null if there are no such field.
		 For checking Driver::isSystemFieldName() is used, so this check can 
		 be driver-dependent. */
		Field* findSystemFieldName(KexiDB::FieldList *fieldlist);

		/*! \return name of any (e.g. first found) database for this connection.
		 This method does not close or open this connection. The method can be used
		 (it is also internally used, e.g. for database dropping) when we need 
		 a database name before we can connect and execute any SQL statement 
		 (e.g. DROP DATABASE).
		 
		 The method can return nul lstring, but in this situation no automatic (implicit)
		 connections could be made, what is useful by e.g. dropDatabase().
		 
		 Note for driver developers: return here a name of database which you are sure 
		 is existing. 
		 Default implementation returns:
		 - value that previously had been set using setAvailableDatabaseName() for 
		   this connection, if it is not empty
		 - else (2nd priority): value of DriverBehaviour::ALWAYS_AVAILABLE_DATABASE_NAME 
		 if it is not empty.
		 
		 See decription of DriverBehaviour::ALWAYS_AVAILABLE_DATABASE_NAME member.
		 You may want to reimplement this method only when you need to depend on
		 this connection specifics 
		 (e.g. you need to check something remotely).
		*/
		virtual QString anyAvailableDatabaseName();
		
		/*! Sets \a dbName as name of a database that can be accessible. 
		 This is option that e.g. application that make use of KexiDB library can set
		 to tune connection's behaviour when it needs to temporary connect to any database
		 in the server to do some work.
		 You can pass empty dbName - then anyAvailableDatabaseName() will try return
		 DriverBehaviour::ALWAYS_AVAILABLE_DATABASE_NAME (the default) value 
		 instead of the one previously set with setAvailableDatabaseName().
		 
		 \sa anyAvailableDatabaseName()
		*/
		void setAvailableDatabaseName(const QString& dbName);

		/*! Because some engines need to have opened any database before
		 executing administrative sql statements like "create database" or "drop database",
		 this method is used to use appropriate, existing database for this connection.
		 For file-based db drivers this always return true and does not set tmpdbName
		 to any value. For other db drivers: this sets tmpdbName to db name computed 
		 using anyAvailableDatabaseName(), and if the name computed is empty, false 
		 is returned; if it is not empty, useDatabase() is called. 
		 False is returned also when useDatabase() fails.
		 You can call this method from your application's level if you really want to perform 
		 tasks that require any used database. In such a case don't forget 
		 to closeDatabase() if returned tmpdbName is not empty.
		 
		 Note: This method has nothing to do with creating or using temporary databases
		 in such meaning that these database are not persistent
		*/
		bool useTemporaryDatabaseIfNeeded(QString &tmpdbName);

		/*! \return autoincrement field's \a aiFieldName value 
		 of last inserted record. This refers \a tableName table.
		 
		 Simply, method internally fetches last inserted record and returns selected 
		 field's value. Requirements: field must be of integer type, there must be a
		 record inserted in current database session (whatever this means).
		 On error -1 is returned.
		*/
		int lastInsertedAutoIncValue(const QString& aiFieldName, const QString& tableName);
		
		/*! \overload int lastInsertedAutoIncValue(const QString&, const QString&)
		*/
		int lastInsertedAutoIncValue(const QString& aiFieldName, const KexiDB::TableSchema& table);

		/*! Executes query \a statement, but without returning resulting 
		 rows (used mostly for functional queries). 
		 Only use this method if you really need. */
		virtual bool drv_executeSQL( const QString& statement ) = 0;

	protected:
		/*! Used by Driver */
		Connection( Driver *driver, ConnectionData &conn_data );

		/*! Method to be called form Connection's subclass destructor.
		 \sa ~Connection() */
		void destroy();

		/*! For reimplemenation: connects to database
			\return true on success. */
		virtual bool drv_connect() = 0;

		/*! For reimplemenation: disconnects database
			\return true on success. */
		virtual bool drv_disconnect() = 0;

		/*! For reimplemenation: loads list of databases' names available for this connection
		 and adds these names to \a list. If your server is not able to offer such a list,
		 consider reimplementing drv_databaseExists() instead. 
		 The mehod should return true only if there was no error on getting database names 
		 list from the server.
		 Default implementation puts empty liost into \a list and returns true. */
		virtual bool drv_getDatabasesList( QStringList &list );

		/*! For optional reimplemenation: asks server if database \a dbName exists.
		 This method is used internally in databaseExists(). The default  implementation
		 calls databaseNames and checks if that list contains \a dbName. If you need to
		 ask the server specifically if a database exists, eg. if you can't retrieve a list
		 of all available database names, please reimplement this method and do all 
		 needed checks.
			
		 See databaseExists() description for details about ignoreErrors argument. 
		 You should use this appropriately in your implementation.

		 Note: This method should also work if there is already database used (with useDatabase());
		 in this situation no changes should be made in current database selection. */
		virtual bool drv_databaseExists( const QString &dbName, bool ignoreErrors = true );

		/*! For reimplemenation: creates new database using connection */
		virtual bool drv_createDatabase( const QString &dbName = QString::null ) = 0;

		/*! For reimplemenation: opens existing database using connection */
		virtual bool drv_useDatabase( const QString &dbName = QString::null ) = 0;

		/*! For reimplemenation: closes previously opened database 
			using connection. */
		virtual bool drv_closeDatabase() = 0;
		
		/*! \return true if internal driver's structure is still in opened/connected 
		 state and database is used. 
		 Note for driver developers: Put here every test that you can do using your 
		 internal engine's database API, 
		 eg (a bit schematic):  my_connection_struct->isConnected()==true. 
		 Do not check things like Connection::isDatabaseUsed() here or other things 
		 that "KexiDB already knows" at its level. 
		 If you cannot test anything, just leave default implementation (that returns true).
		 
		 Result of this method is used as an addtional chance to check for isDatabaseUsed().
		 Do not call this method from your driver's code, it should be used at KexiDB 
		 level only.
		*/
		virtual bool drv_isDatabaseUsed() const { return true; }

		/*! For reimplemenation: drops database from the server
			using connection. After drop, database shouldn't be accessible 
			anymore. */
		virtual bool drv_dropDatabase( const QString &dbName = QString::null ) = 0;

		/*! returns "CREATE TABLE ..." statement string needed for \a tableSchema
		 creation in the database. 
		 
		 Note: The statement string can be specific for this connection's driver database, 
		 and thus not reusable in general.
		*/
		QString createTableStatement( const KexiDB::TableSchema& tableSchema ) const;

		/*! returns "SELECT ..." statement's string needed for executing query 
		 defined by \a querySchema.

		 Note: The statement string can be specific for this connection's driver database, 
		 and thus not reusable in general.
		*/
		QString selectStatement( KexiDB::QuerySchema& querySchema ) const;

		/*! returns "SELECT ..." statement's string needed for executing query 
		 defined by "select * from <table_name>" where <table_name> is \a tableSchema's name.
		 This method's variant can be useful when there is no appropriate QuerySchema defined.

		 Note: The statement string can be specific for this connection's driver database, 
		 and thus not reusable in general.
		*/
		QString selectStatement( KexiDB::TableSchema& tableSchema ) const;

		/*! Creates table using \a tableSchema information.
		 \return true on success. Default implementation 
		 builds a statement using createTableStatement() and calls drv_executeSQL()
		 Note for driver developers: reimplement this only if you want do to 
		 this in other way.
		 */
		virtual bool drv_createTable( const KexiDB::TableSchema& tableSchema );

		/*! 
		 Creates table named by \a tableSchemaName. Schema object must be on
		 schema tables' list before calling this method (otherwise false if returned).
		 Just uses drv_createTable( const KexiDB::TableSchema& tableSchema ).
		 Used internally, e.g. in createDatabase().
		 \return true on success
		*/
		virtual bool drv_createTable( const QString& tableSchemaName );

//		/*! Executes query \a statement and returns resulting rows 
//			(used mostly for SELECT query). */
//		virtual bool drv_executeQuery( const QString& statement ) = 0;

		/*! Returns unique identifier of last inserted row. 
		 Typically this is just primary key value. 
		 This identifier could be reused when we want to reference
		 just inserted row.
		 Note for driver developers: contact js (at) iidea.pl 
		 if your engine do not offers this information. */
		virtual Q_ULLONG drv_lastInsertRowID() = 0;

		/*! Note for driver developers: begins new transaction
		 and returns handle to it. Default implementation just
		 executes "BEGIN" sql statement and returns just empty data (TransactionData object).
		 
		 Drivers that do not support transactions (see Driver::features())
		 do never call this method.
		 Reimplement this method if you need to do something more 
		 (e.g. if you driver will support multiple transactions per connection).
		 Make subclass of TransactionData (declared in transaction.h)
		 and return object of this subclass.
		 You should return NULL if any error occured.
		 Do not check anything in connection (isConnected(), etc.) - all is already done.
		*/
		virtual TransactionData* drv_beginTransaction();
		
		/*! Note for driver developers: begins new transaction
		 and returns handle to it. Default implementation just
		 executes "COMMIT" sql statement and returns true on success.
		 
		 \sa drv_beginTransaction()
		*/
		virtual bool drv_commitTransaction(TransactionData* trans);
		
		/*! Note for driver developers: begins new transaction
		 and returns handle to it. Default implementation just
		 executes "ROLLBACK" sql statement and returns true on success.
		 
		 \sa drv_beginTransaction()
		*/
		virtual bool drv_rollbackTransaction(TransactionData* trans);

		/*! Changes autocommiting option for established connection.
		 \return true on success.
		 
		 Note for driver developers: reimplement this only if your engine
		 allows to set special auto commit option (like "SET AUTOCOMMIT=.." in MySQL).
		 If not, auto commit behaviour will be simulated if at least single 
		 transactions per connection are supported by the engine.
		 Do not set any internal flags for autocommiting -- it is already done inside
		 setAutoCommit().
		 
		 Default implementation does nothing with connection, just returns true.
		 
		 \sa drv_beginTransaction(), autoCommit(), setAutoCommit()
		*/
		virtual bool drv_setAutoCommit(bool on);

		/*! Internal, for handling autocommited transactions:
		 begins transaction is one is supported.
		 If driver only supports single transaction,
		 and there is already transaction started, it is commited before
		 starting a new one. \return true if new transaction started
		 successfully or no transactions are supported at all by the driver
		 or if autocommit option is turned off.
		 Newly created transaction (or null on error)
		 is passed to \a trans parameter.
		*/
		bool beginAutoCommitTransaction(Transaction& trans);
		
		/*! Internal, for handling autocommited transactions:
		 Commits transaction prevoiusly started with beginAutoCommitTransaction().
		 \return true on success or when no transactions are supported 
		 at all by the driver.
		*/
		bool commitAutoCommitTransaction(const Transaction& trans);
		
		/*! Internal, for handling autocommited transactions:
		 Rollbacks transaction prevoiusly started with beginAutoCommitTransaction().
		 \return true on success or when no transactions are supported 
		 at all by the driver.
		*/
		bool rollbackAutoCommitTransaction(const Transaction& trans);

		/*! Creates cursor data and initializes cursor 
			using \a statement for later data retrieval. */
//		virtual CursorData* drv_createCursor( const QString& statement ) = 0;
		/*! Closes and deletes cursor data. */
//		virtual bool drv_deleteCursor( CursorData *data ) = 0;
		
		/*! Helper: checks if connection is established; 
			if not: error message is set up and false returned */
		bool checkConnected();

		/*! Helper: checks both if connection is established and database any is used; 
			if not: error message is set up and false returned */
		bool checkIsDatabaseUsed();

		/*! Setups schema data for object that owns sdata (e.g. table, query)
			using \a cursor opened on 'kexi__objects' table, pointing to a record
			corresponding to given object. */
		bool setupObjectSchemaData( const KexiDB::RowData &data, SchemaData &sdata );
//		bool setupObjectSchemaData( const KexiDB::Cursor *cursor, SchemaData *sdata );
		
		/*! Setups full table schema for table \a t using 'kexi__*' system tables. 
			Used internally by tableSchema() methods. */
		KexiDB::TableSchema* setupTableSchema( const KexiDB::RowData &data );

		bool updateRow(QuerySchema &query, RowData& data, RowEditBuffer& buf);

		bool insertRow(QuerySchema &query, RowData& data, RowEditBuffer& buf);

		bool deleteRow(QuerySchema &query, RowData& data);

		/*! Allocates all needed table KexiDB system objects for kexi__* KexiDB liblary's
		 system tables schema.
		 These objects are used internally in this connection
		 and are added to list of tables (by name, 
		 not by id because these have no ids).
		*/
		bool setupKexiDBSystemSchema();

		/*! used internally by setupKexiDBSystemSchema():
		 Allocates single table KexiDB system object named \a tsname 
		 and adds this to list of such objects (for later removal on closeDatabase()). 
		*/
		TableSchema* newKexiDBSystemTableSchema(const QString& tsname);

		/*! Called by TableSchema -- signals destruction to Connection object
		 To avoid having deleted table object on its list. */
		void removeMe(TableSchema *ts);
				
		QGuardedPtr<ConnectionData> m_data;
		QString m_name;
		QString m_usedDatabase; //!< database name that is opened now

		//! Table schemas retrieved on demand with tableSchema()
		QIntDict<TableSchema> m_tables;
		QDict<TableSchema> m_tables_byname;
		QIntDict<QuerySchema> m_queries;
		QDict<QuerySchema> m_queries_byname;

		//! used just for removing system TableSchema objects on db close.
		QPtrList<TableSchema> m_kexiDBSystemtables;

		//! cursors created for this connection
		QPtrDict<KexiDB::Cursor> m_cursors;
//		ConnectionInternal* m_internal;
	friend class KexiDB::Driver;
	friend class KexiDB::Cursor;
	friend class KexiDB::TableSchema; //!< for removeMe()

		ConnectionPrivate *d;
	private:
		Driver *m_driver;
		bool m_is_connected : 1;
		bool m_autoCommit : 1;
		bool m_destructor_started : 1; //!< helper: true if destructor is started
		
		QString m_availableDatabaseName; //! used by anyAvailableDatabaseName()
};

} //namespace KexiDB

#endif

