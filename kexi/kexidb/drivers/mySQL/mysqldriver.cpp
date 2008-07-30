/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
Daniel Molkentin <molkentin@kde.org>
Copyright (C) 2003   Joseph Wenninger<jowenn@kde.org>

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

#ifdef Q_WS_WIN
# include <mysql/config-win.h>
#endif
#include <mysql_version.h>
#include <mysql.h>
#define BOOL bool

#include <QVariant>
#include <QFile>

#include <kgenericfactory.h>
#include <kdebug.h>

#include "mysqldriver.h"
#include "mysqlconnection.h"
#include <kexidb/field.h>
#include <kexidb/driver_p.h>
#include <kexidb/utils.h>

using namespace KexiDB;

KEXIDB_DRIVER_INFO( MySqlDriver, mysql )

/* TODO: Implement buffered/unbuffered, rather than buffer everything.
   Each MYSQL connection can only handle at most one unbuffered cursor,
   so MySqlConnection should keep count?
 */

/*! 
 * Constructor sets database features and
 * maps the types in KexiDB::Field::Type to the MySQL types.
 *
 * See: http://dev.mysql.com/doc/mysql/en/Column_types.html
 */
MySqlDriver::MySqlDriver(QObject *parent, const QStringList &args) :
  Driver(parent, args)
{
//	KexiDBDrvDbg << "MySqlDriver::MySqlDriver()" << endl;

  d->isFileDriver=false;
  d->features=IgnoreTransactions | CursorForward;

  beh->ROW_ID_FIELD_NAME="LAST_INSERT_ID()";
  beh->ROW_ID_FIELD_RETURNS_LAST_AUTOINCREMENTED_VALUE=true;
  beh->_1ST_ROW_READ_AHEAD_REQUIRED_TO_KNOW_IF_THE_RESULT_IS_EMPTY=false;
  beh->USING_DATABASE_REQUIRED_TO_CONNECT=false;
  beh->QUOTATION_MARKS_FOR_IDENTIFIER='`';
  initDriverSpecificKeywords(keywords);
  
  //predefined properties
#if MYSQL_VERSION_ID < 40000
  d->properties["client_library_version"] = MYSQL_SERVER_VERSION; //nothing better
  d->properties["default_server_encoding"] = MYSQL_CHARSET; //nothing better
#elif MYSQL_VERSION_ID < 50000
//OK?	d->properties["client_library_version"] = mysql_get_client_version();
#endif

  d->typeNames[Field::Byte]="TINYINT";
  d->typeNames[Field::ShortInteger]="SMALLINT";
  d->typeNames[Field::Integer]="INT";
  d->typeNames[Field::BigInteger]="BIGINT";
  // Can use BOOLEAN here, but BOOL has been in MySQL longer
  d->typeNames[Field::Boolean]="BOOL";
  d->typeNames[Field::Date]="DATE";
  d->typeNames[Field::DateTime]="DATETIME";
  d->typeNames[Field::Time]="TIME";
  d->typeNames[Field::Float]="FLOAT";
  d->typeNames[Field::Double]="DOUBLE";
  d->typeNames[Field::Text]="VARCHAR";
  d->typeNames[Field::LongText]="LONGTEXT";
  d->typeNames[Field::BLOB]="BLOB";
}

MySqlDriver::~MySqlDriver()
{
}

KexiDB::Connection*
MySqlDriver::drv_createConnection( ConnectionData &conn_data )
{
  return new MySqlConnection( this, conn_data );
}

bool MySqlDriver::isSystemDatabaseName(const QString &n) const
{
  return n.toLower()=="mysql" || Driver::isSystemObjectName(n);
}

bool MySqlDriver::drv_isSystemFieldName(const QString&) const {
  return false;
}

QString MySqlDriver::escapeString(const QString& str) const
{
  //escape as in http://dev.mysql.com/doc/refman/5.0/en/string-syntax.html
//! @todo support more characters, like %, _

  const int old_length = str.length();
  int i;
  for ( i = 0; i < old_length; i++ ) { //anything to escape?
    const unsigned int ch = str[i].unicode();
    if (ch == '\\' || ch == '\'' || ch == '"' || ch == '\n' || ch == '\r' || ch == '\t' || ch == '\b' || ch == '\0')
      break;
  }
  if (i >= old_length) { //no characters to escape
    return QString::fromLatin1("'") + str + QString::fromLatin1("'");
  }

  QChar *new_string = new QChar[ old_length * 3 + 1 ]; // a worst case approximation
//! @todo move new_string to Driver::m_new_string or so...
  int new_length = 0;
  new_string[new_length++] = '\''; //prepend '
  for ( i = 0; i < old_length; i++, new_length++ ) {
    const unsigned int ch = str[i].unicode();
    if (ch == '\\') {
      new_string[new_length++] = '\\';
      new_string[new_length] = '\\';
    }
    else if (ch <= '\'') {//check for speedup
      if (ch == '\'') {
        new_string[new_length++] = '\\';
        new_string[new_length] = '\'';
      }
      else if (ch == '"') {
        new_string[new_length++] = '\\';
        new_string[new_length] = '"';
      }
      else if (ch == '\n') {
        new_string[new_length++] = '\\';
        new_string[new_length] = 'n';
      }
      else if (ch == '\r') {
        new_string[new_length++] = '\\';
        new_string[new_length] = 'r';
      }
      else if (ch == '\t') {
        new_string[new_length++] = '\\';
        new_string[new_length] = 't';
      }
      else if (ch == '\b') {
        new_string[new_length++] = '\\';
        new_string[new_length] = 'b';
      }
      else if (ch == '\0') {
        new_string[new_length++] = '\\';
        new_string[new_length] = '0';
      }
      else
        new_string[new_length] = str[i];
    }
    else
      new_string[new_length] = str[i];
  }

  new_string[new_length++] = '\''; //append '
  QString result(new_string, new_length);
  delete [] new_string;
  return result;
}

QString MySqlDriver::escapeBLOB(const QByteArray& array) const
{
  return KexiDB::escapeBLOB(array, KexiDB::BLOBEscape0xHex);
}

QByteArray MySqlDriver::escapeString(const QByteArray& str) const
{
//! @todo optimize using mysql_real_escape_string()?
//! see http://dev.mysql.com/doc/refman/5.0/en/string-syntax.html

  return QByteArray("'")+QByteArray(str)
    .replace( '\\', "\\\\" )
    .replace( '\'', "\\''" )
    .replace( '"', "\\\"" )
    + QByteArray("'");
}

/*! Add back-ticks to an identifier, and replace any back-ticks within
 * the name with single quotes.
 */
QString MySqlDriver::drv_escapeIdentifier(const QString& str) const
{
  return QString(str).replace('`', "'");
}

QByteArray MySqlDriver::drv_escapeIdentifier(const QByteArray& str) const
{
  return QByteArray(str).replace('`', "'");
}

#include "mysqldriver.moc"

