/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Joseph Wenninger <jowenn@kde.org>
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

#ifndef KEXIDB_FIELD_H
#define KEXIDB_FIELD_H

#include <qvariant.h>
#include <qstring.h>

namespace KexiDB {

/*! KexiDB::Field provides information about single database field.
*/

class TableSchema;

class KEXI_DB_EXPORT Field
{
	public:

		/*! Unified (most common used) types of fields. */
		enum Type
		{
			InvalidType = 0,   /*! type that cannot be supported or is not implemented */
			Byte = 1,          /*! 1 byte, signed or unsigned */
			ShortInteger = 2,  /*! 2 bytes, signed or unsigned */
			Integer = 3,       /*! 4 bytes, signed or unsigned */
			BigInteger = 4,    /*! 8 bytes, signed or unsigned */
//			AutoIncrement, /* 4 bytes, like LongInteger, used for keys */
			Boolean = 5,       /*! 0 or 1 */
			Date = 6,          /*! */
			DateTime = 7,      /*! */
			Time = 8,          /*! */
			Float = 9,         /*! 4 bytes */
			Double = 10,       /*! 8 bytes */
			Text = 11,         /*! other name: Varchar; no more than 200 bytes, for efficiency */
			LongText = 12,     /*! other name: Memo. More than 200 bytes*/
			BLOB = 13,         /*! large binary object */

			LastType = 14	//! This line should be at the end of the enum!
		};

		const int defaultTextLength() { return 200; }

			/*! Groups of types of fields. */
		enum TypeGroup
		{
			InvalidGroup = 0,
			IntegerGroup = 1,
			FloatGroup = 2,
			BooleanGroup = 3,
			DateTimeGroup = 4,
			TextGroup = 5,
			BLOBGroup = 6, /* large binary object */

			LastGroup	// This line should be at the end of the enum!
		};

/*		enum ColumnType
		{
			SQLInvalid = 0,
			SQLBigInt,
			SQLBinary,
			SQLBoolean,
			SQLDate,
			SQLDecimal,
			SQLDouble,
			SQLFloat,
			SQLInteger,
			SQLInterval,
			SQLLongVarBinary,
			SQLLongVarchar,
			SQLNumeric,
			SQLSmallInt,
			SQLTime,
			SQLTimeStamp,
			SQLTinyInt,
			SQLVarBinary,
			SQLVarchar,
			SQLLastType	// This line should be at the end of the enum!
		};
*/
		enum Constraints
		{
			NoConstraints = 0,
			AutoInc = 1,
			Unique = 2,
			PrimaryKey = 4,
			ForeignKey = 8,
			NotNull = 16
 		};

		enum Options
		{
			NoOptions = 0,
			Unsigned = 1
		};

		typedef QPtrList<Field> List;
		typedef QPtrListIterator<Field> ListIterator;

		Field(TableSchema *tableSchema);
		Field();

		Field(const QString& name, Type ctype,
			int cconst=NoConstraints,
			int options = NoOptions,
			int length=0, int precision=0,
			QVariant defaultValue=QVariant());


		virtual ~Field();

		static QVariant::Type variantType(Type type);
		static QString typeName(Type type);

		virtual QString		name() const;
		
		/*! \return table schema of table that owns this field. */
		virtual TableSchema* table() const;

		/*!
		 *	@return true if the field is autoincrement (e.g. integer/numeric)
		 */
		bool		isAutoIncrement() const { return constraints() & AutoInc; }
		/*!
		 *	@return true if the field is member of single-field primary key
		 */
		bool		isPrimaryKey() const { return constraints() & PrimaryKey; }
		/*!
		 *	@return true if the field is member of single-field unique key
		 */
		bool		isUniqueKey() const { return constraints() & Unique; }
		/*!
		 *	@return true if the field is member of single-field foreign key
		 */
		bool		isForeignKey() const { return constraints() & ForeignKey; }
		/*!
		 *	@return true if the field is not allowed to be null
		 */
		bool		isNotNull() const { return constraints() & NotNull; }
		/*!
		 *	@return the constraints defined for this field
		 */
		virtual int		constraints() const;

//js: we have m_table for this		/*!
//		 *	@return the table.column that this field references or QString::null if !foreign_key()
//		 */
//		virtual QString		references() const;

		int options() const { return m_options; }
		void setOptions(int options) { m_options = options; }

		virtual QVariant::Type	variantType() const;
		virtual Type type() const;
		QString typeName() const;
		virtual QVariant defaultValue() const;
		virtual int length() const;
		virtual int precision() const; //! for numeric and other fields that have both length and precision
		virtual bool isUnsigned() const; //! if the type has the unsigned attribute
//		virtual bool isBinary() const;

		void setType(Type t);
		void setTable(TableSchema *table);
		void setName(const QString& n);
		void setConstraints(int c);
		void setLength(int l);
		void setPrecision(int p);
		void setUnsigned(bool u);
		void setBinary(bool b);
		void setDefaultValue(const QVariant& def);
		/*! Sets default value decoded from QCString. 
			Decoding errors are detected (value is strictly checked against field type) 
			- if one is encountered, default value is cleared (defaultValue()==QVariant()). 
			\return true if given value was valid for field type. */
		bool setDefaultValue(const QCString& def);
		void setAutoIncrement(bool a);
		void setPrimaryKey(bool p);
		void setUniqueKey(bool u);
		void setForeignKey(bool f);
		void setNotNull(bool n);

		//! \return string for for debugging purposes.
		QString debugString() const;

	private:
//		QString m_table;
		TableSchema *m_table;
		QString m_name;
//		QString m_reference;
		Type m_type;
		int m_constraints;
		int m_length;
		int m_precision;
		int m_options;
//		bool m_binary;
		QVariant m_defaultValue;
		QString m_caption;
		QString m_help;

	friend class Connection;
	friend class TableSchema;
};

} //namespace KexiDB

#endif
