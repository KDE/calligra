/**************************************************************************
		mdatabasereportengine.h  -  Kugar database report engine
			-------------------
	begin		: 2003-03-22 23:58:12
	copyright	: (C) 2003 by Joris Marcillac
	email		: joris@marcillac.com
**************************************************************************/
/**************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*	This program is distributed in the hope that it will be useful,	  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of	  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU *
*	Library General Public License for more details.		  *
*                                                                         *
*	You should have received a copy of the GNU Library General Public *
*	License along with this library; if not, write to the Free	  *
*	Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,	  *
*	MA 02111-1307, USA						  *
*                                                                         *
**************************************************************************/
#if !defined(MDATABASEREPORTENGINE_H)
#define MDATABASEREPORTENGINE_H

#include <qdom.h>
#include <qobject.h>
#include <qmap.h>
#include <qvariant.h>
#include <qstringlist.h>
#include <qintdict.h>

#include "sqlcursor.h"

class QSqlDatabase;
class QString;
class QTextStream;

/** this internal class link the field name of database recordset and level number used in kugar */
class CLinkFieldLevel
{
public:
    CLinkFieldLevel(): m_strFieldName(""), m_OldValue(""), m_nLevel(0) {}
    CLinkFieldLevel( const QString& strFieldName, const QString& OldValue, int nLevel )
        : m_strFieldName(strFieldName), m_OldValue(OldValue), m_nLevel(nLevel)
    { }

    QString fieldName() const { return m_strFieldName; }
    QVariant oldValue() const { return m_OldValue; }
    int level() const { return m_nLevel; }
    void setFieldName( const QString& strFieldName ) { m_strFieldName = strFieldName; }
    void setOldValue( const QVariant& OldValue ) { m_OldValue = OldValue; }
    void setLevel( int nLevel ) { m_nLevel = nLevel; }

private:
    QString m_strFieldName;	// the field name recordset
    QVariant m_OldValue;	// the old value of the field name
    int m_nLevel;		// level number from kugar
};

/** this internal class, using as possible the same MReportEngine kugar interface.
Some parts of code copyright	: (C) 1999 by Mutiny Bay Software (info@mutinybaysoftware.com)
Some parts of code copyright	: (C) 2002 Alexander Dymo (cloudtemple@mksat.net)
*/

class MDatabaseReportEngine :
	public QObject
{
	Q_OBJECT
public:
	MDatabaseReportEngine( QWidget* parent = 0, const char* name = 0 );
	MDatabaseReportEngine( const MDatabaseReportEngine& mReportEngine );
	MDatabaseReportEngine operator=( const MDatabaseReportEngine& mReportEngine );
	virtual ~MDatabaseReportEngine();

	bool setReportTemplate(const QString& );
	bool setReportTemplate( QIODevice* );
	void initDatabase();
	bool createReportDataFile( QIODevice* dev, const QString& templateFile );
	QString mergeReportDataFile( QIODevice* dev );

private:
	/** Report data document */
	QDomDocument rd;
	/** Report template document */
	QDomDocument rt;

	QString m_strSql;
	QString m_strOrderBy;
	QString	m_strDriverType;
	QString m_strDatabaseName;
	QString m_strUserName;
	QString m_strPassword;
	QString m_strHostName;
	QString m_strPort;

	QString m_strIndent;
	QTextStream *m_strDataBuffer;
	typedef QMap<int, CLinkFieldLevel> OldValueMap;
	OldValueMap m_mapOldValue;

	QIntDict<QStringList> details;

private:
	void initTemplate();
	void setDatabaseAttributes(QDomNode* report);
	void setDriverAttributes(QDomNode* report);
	void setSqlQueryAttributes(QDomNode* report);
	void setGroupByAttributes(QDomNode* report);
	void setSqlOrderByAttributes(QDomNode* report);
	void setDetailAttributes(QDomNode* report);
	bool setBufferFromDatabase( const QString& strTemplateFile = "", bool merge = false );
	void addDataRow( int level, QStringList* detail, CSqlCursor* cursor);
	void setHeaderDataFile();
	void setSQLQuery();
	void initDatabaseDlg();
};

#endif // !defined(MDATABASEREPORTENGINE_H)
