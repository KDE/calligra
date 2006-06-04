/* This file is part of the KDE project
   Copyright (C) 2004-2006 Jaroslaw Staniek <js@iidea.pl>

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


#ifndef KEXIDB_UTILS_H
#define KEXIDB_UTILS_H

#include <q3valuelist.h>
#include <qvariant.h>
#include <QByteArray>

#include <kexidb/connection.h>
#include <kexidb/driver.h>

namespace KexiDB
{
	//! for convenience
	inline KEXI_DB_EXPORT bool deleteRow(Connection &conn, TableSchema *table, 
		const QString &keyname, const QString &keyval)
	{
		return table!=0 && conn.executeSQL("DELETE FROM " + table->name() + " WHERE " 
			+ keyname + "=" + conn.driver()->valueToSQL( Field::Text, QVariant(keyval) ));
	}

	inline KEXI_DB_EXPORT bool deleteRow(Connection &conn, const QString &tableName, 
		const QString &keyname, const QString &keyval)
	{
		return conn.executeSQL("DELETE FROM " + tableName + " WHERE " 
			+ keyname + "=" + conn.driver()->valueToSQL( Field::Text, QVariant(keyval) ));
	}

	inline KEXI_DB_EXPORT bool deleteRow(Connection &conn, TableSchema *table, 
		const QString &keyname, int keyval)
	{
		return table!=0 && conn.executeSQL("DELETE FROM " + table->name() + " WHERE " 
			+ keyname + "=" + conn.driver()->valueToSQL( Field::Integer, QVariant(keyval) ));
	}

	inline KEXI_DB_EXPORT bool deleteRow(Connection &conn, const QString &tableName, 
		const QString &keyname, int keyval)
	{
		return conn.executeSQL("DELETE FROM " + tableName + " WHERE " 
			+ keyname + "=" + conn.driver()->valueToSQL( Field::Integer, QVariant(keyval) ));
	}

	/*! Delete row with two generic criterias. */
	inline KEXI_DB_EXPORT bool deleteRow(Connection &conn, const QString &tableName, 
		const QString &keyname1, Field::Type keytype1, const QVariant& keyval1, 
		const QString &keyname2, Field::Type keytype2, const QVariant& keyval2)
	{
		return conn.executeSQL("DELETE FROM " + tableName + " WHERE " 
			+ keyname1 + "=" + conn.driver()->valueToSQL( keytype1, keyval1 )
			+ " AND " + keyname2 + "=" + conn.driver()->valueToSQL( keytype2, keyval2 ));
	}

	inline KEXI_DB_EXPORT bool replaceRow(Connection &conn, TableSchema *table, 
		const QString &keyname, const QString &keyval, const QString &valname, QVariant val, int ftype)
	{
		if (!table || !KexiDB::deleteRow(conn, table, keyname, keyval))
			return false;
		return conn.executeSQL("INSERT INTO " + table->name() + " (" + keyname + "," + valname + ") VALUES (" + conn.driver()->valueToSQL( Field::Text, QVariant(keyval) ) + "," + conn.driver()->valueToSQL( ftype, val) + ")");
	}

	typedef Q3ValueList<uint> TypeGroupList;

	/*! \return list of types for type group \a typeGroup. */
	KEXI_DB_EXPORT const TypeGroupList typesForGroup(KexiDB::Field::TypeGroup typeGroup);

	/*! \return list of i18n'd type names for type group \a typeGroup. */
	KEXI_DB_EXPORT QStringList typeNamesForGroup(KexiDB::Field::TypeGroup typeGroup);

	/*! \return list of (not-i18n'd) type names for type group \a typeGroup. */
	KEXI_DB_EXPORT QStringList typeStringsForGroup(KexiDB::Field::TypeGroup typeGroup);

	/*! \return default field type for type group \a typeGroup,
	 for example, Field::Integer for Field::IntegerGroup.
	 It is used e.g. in KexiAlterTableDialog, to properly fill 
	 'type' property when user selects type group for a field. */
	KEXI_DB_EXPORT KexiDB::Field::Type defaultTypeForGroup(KexiDB::Field::TypeGroup typeGroup);

	/*! \return true if \a v represents an empty (but not null) value.
	 Values of some types (as for strings) can be both empty and not null. */
	inline bool isEmptyValue(KexiDB::Field *f, const QVariant &v) {
		if (f->hasEmptyProperty() && v.toString().isEmpty() && !v.toString().isNull())
			return true;
		return v.isNull();
	}

	/*! Sets \a msg to an error message retrieved from object \a obj, and \a details
	 to details of this error (server message and result number).
	 Does nothing if \a obj is null or no error occured.
	 \a msg and \a details strings are not overwritten.
	 If \a msg is not empty, \a obj's error message is appended to \a details.
	 */
	KEXI_DB_EXPORT void getHTMLErrorMesage(Object* obj, QString& msg, QString &details);

	/*! This methods works like above, but appends both a message and a description
	 to \a msg. */
	KEXI_DB_EXPORT void getHTMLErrorMesage(Object* obj, QString& msg);

	/*! This methods works like above, but works on \a result's  members instead. */
	KEXI_DB_EXPORT void getHTMLErrorMesage(Object* obj, ResultInfo *result);

	/*! Function useful for building WHERE parts of sql statements.
	Constructs an sql string like "fielname = value" for specific \a drv driver,
	 field type \a t, \a fieldName and \a value. If \a value is null, "fieldname is NULL" 
	 string is returned. */
	inline KEXI_DB_EXPORT QString sqlWhere(KexiDB::Driver *drv, KexiDB::Field::Type t, 
		const QString fieldName, const QVariant value)
	{
		if (value.isNull())
			return fieldName + " is NULL";
		return fieldName + "=" + drv->valueToSQL( t, value );
	}

	/*! \return identifier for object \a objName of type \a objType
	 or 0 if such object does not exist. */
	KEXI_DB_EXPORT int idForObjectName( Connection &conn, const QString& objName, int objType );

	/*! Variant class providing a pointer to table or query. */
	class KEXI_DB_EXPORT TableOrQuerySchema {
		public:
			//! Creates a new TableOrQuerySchema variant object, retrieving table or query schema
			//! using \a conn connection and \a name.
			TableOrQuerySchema(Connection *conn, const QByteArray& name, bool table);

			//! Creates a new TableOrQuerySchema variant object. \a tableOrQuery must be of 
			//! class TableSchema or QuerySchema.
			TableOrQuerySchema(FieldList &tableOrQuery);
			
			//! Creates a new TableOrQuerySchema variant object, retrieving table or query schema
			//! using \a conn connection and \a id.
			TableOrQuerySchema(Connection *conn, int id);

			TableOrQuerySchema(TableSchema* table);
			TableOrQuerySchema(QuerySchema* query);

			//! \return a pointer to the query if it's provided
			QuerySchema* query() const { return m_query; }

			//! \return a pointer to the table if it's provided
			TableSchema* table() const { return m_table; }

			//! \return name of a query or table
			QByteArray name() const;

			//! \return caption (if present) or name of the table/query
			QString captionOrName() const;

			//! \return all columns for the table or the query
			const QueryColumnInfo::Vector columns(bool unique = false);

			/*! \return a field of the table or the query schema for name \a name
			 or 0 if there is no such field. */
			Field* field(const QString& name);

			/*! Like Field* field(const QString& name);
			 but returns all information associated with field/column \a name. */
			QueryColumnInfo* columnInfo(const QString& name);

			/*! \return connection object, for table or query or 0 if there's no table or query defined. */
			Connection* connection() const;

			/*! \return String for debugging purposes. */
			QString debugString();

			/*! Shows debug information about table or query. */
			void debug();

		protected:
			QByteArray m_name; //!< the name is kept here because m_table and m_table can be 0
			                 //! and we still want name() and acptionOrName() work.
			TableSchema* m_table;
			QuerySchema* m_query;
	};

//! @todo perhaps use quint64 here?
	/*! \return a number of rows that can be retrieved from \a tableSchema. 
	 The table must be created or retrieved using a Connection object,
	 i.e. tableSchema.connection() must not return 0.
	 For SQL sata sources it does not fetch any records, only "COUNT()" 
	 SQL aggregation is used at the backed. 
	 -1 is returned if error occured. */
	KEXI_DB_EXPORT int rowCount(const TableSchema& tableSchema);

	/*! Like above but operates on a query schema. */
	KEXI_DB_EXPORT int rowCount(QuerySchema& querySchema);

	/*! Like above but operates on a table or query schema variant. */
	KEXI_DB_EXPORT int rowCount(TableOrQuerySchema& tableOrQuery);

	/*! \return a number of columns that can be retrieved from table or query schema. 
	 In case of query, expanded fields are counted. Can return -1 if \a tableOrQuery 
	 has neither table or query assigned. */
	KEXI_DB_EXPORT int fieldCount(TableOrQuerySchema& tableOrQuery);

	/*! shows connection test dialog with a progress bar indicating connection testing 
	 (within a second thread).
	 \a data is used to perform a (temporary) test connection. \a msgHandler is used to display errors.
	 On successful connecting, a message is displayed. After testing, temporary connection is closed. */
	KEXI_DB_EXPORT void connectionTestDialog(QWidget* parent, const KexiDB::ConnectionData& data, 
		KexiDB::MessageHandler& msgHandler);

	/*! Saves connection data \a data into \a map. */
	KEXI_DB_EXPORT QMap<QString,QString> toMap( const ConnectionData& data );

	/*! Restores connection data \a data from \a map. */
	KEXI_DB_EXPORT void fromMap( const QMap<QString,QString>& map, ConnectionData& data );

	//! Used in splitToTableAndFieldParts().
	enum SetFieldNameIfNoTableNameOptions {
		FailIfNoTableOrFieldName = 0, //!< default value for splitToTableAndFieldParts()
		SetFieldNameIfNoTableName = 1 //!< @see splitToTableAndFieldParts()
	};

	/*! Splits \a string like "table.field" into "table" and "field" parts. 
	 On success, a table name is passed to \a tableName and a field name is passed to \a fieldName.
	 The function fails if either:
	 - \a string is empty, or
	 - \a string does not contain '.' character and \a option is FailIfNoTableOrFieldName 
	    (the default), or
	 - '.' character is the first of last character of \a string (in this case table name 
	   or field name could become empty what is not allowed).

	 If \a option is SetFieldNameIfNoTableName and \a string does not contain '.', 
	 \a string is passed to \a fieldName and \a tableName is set to QString::null
	 without failure.

	 If function fails, \a tableName and \a fieldName remain unchanged.
	 \return true on success. */
	KEXI_DB_EXPORT bool splitToTableAndFieldParts(const QString& string, 
		QString& tableName, QString& fieldName, 
		SetFieldNameIfNoTableNameOptions option = FailIfNoTableOrFieldName);

	/*! \return true if \a type supports "visibleDecimalPlaces" property. */
	KEXI_DB_EXPORT bool supportsVisibleDecimalPlacesProperty(Field::Type type);

	/*! \return string constructed by converting \a value. 
	 * If \a decimalPlaces is < 0, all meaningful fractional digits are returned.
	 * If \a automatically is 0, just integer part is returned. 
	 * If \a automatically is > 0, fractional part should take exactly 
	   N digits: if the fractional part is shorter than N, additional zeros are appended. 
	   For example, "12.345" becomes "12.345000" if N=6.

	 No rounding is actually performed.
	 KLocale::formatNumber() and KLocale::decimalSymbol() are used to get locale settings.

	 @see KexiDB::Field::visibleDecimalPlaces() */
	KEXI_DB_EXPORT QString formatNumberForVisibleDecimalPlaces(double value, int decimalPlaces);

	//! \return true if \a propertyName is an extended field property.
	KEXI_DB_EXPORT bool isExtendedTableProperty( const QByteArray& propertyName );

	/*! \return type of field for integer value \a type. 
	 If \a type cannot be casted to KexiDB::Field::Type, KexiDB::Field::InvalidType is returned.
	 This can be used when type information is deserialized from a string or QVariant. */
	KEXI_DB_EXPORT KexiDB::Field::Type intToFieldType( int type );

	/*! Sets property values for \a field. \return true if all the values are valid and allowed.
	 On failure contents of \a field is undefined.
	 Properties coming from extended schema are also supported.
	 This function is used e.g. by AlterTableHandler when property information comes in form of text.
	 */
	KEXI_DB_EXPORT bool setFieldProperties( Field& field, const QMap<QByteArray, QVariant>& values );

	/*! Sets property value for \a field. \return true if the property has been found and 
	 the value is valid for this property. On failure contents of \a field is undefined.
	 Properties coming from extended schema are also supported.
	 This function is used e.g. by AlterTableHandler when property information comes in form of text.
	 */
	KEXI_DB_EXPORT bool setFieldProperty(Field& field, const QByteArray& propertyName, 
		const QVariant& value);
}

#endif
