/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2002 Daniel Molkentin <molkentin@kde.org>

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

#ifndef KEXI_PART_H
#define KEXI_PART_H

#include <qobject.h>
#include <qptrlist.h>

#include <koDocument.h>
#include "../kexidb/drivermanager.h"
#include "../kexidb/driver.h"
#include "../kexidb/connection.h"

class KexiDoc;
class DCOPObject;
class KexiRelation;
class KexiProjectHandler;
class KexiProjectHandlerProxy;
class KexiFilterManager;
class KexiView;
class KexiProjectConnectionData;

typedef QPtrList<KexiProjectHandler> ProviderList;

class KEXICORE_EXPORT FileReference
{
public:
	FileReference():group(),name(),location(){;}
	FileReference(const FileReference &ref):group(ref.group),name(ref.name),location(ref.location){}
	FileReference(const QString &group_,const QString &name_,const QString &location_):
		group(group_),name(name_),location(location_){}
	~FileReference(){}
	QString group;
	QString name;
	QString location;
};


typedef QMap<QString, QDomElement> Groups;
typedef QValueList<FileReference> References;
typedef QMap<QString, FileReference> ReferencesM;
typedef QPtrList<KexiProjectHandler> PartList;

class KEXICORE_EXPORT KexiProject : public KoDocument
{
	Q_OBJECT
public:
	KexiProject( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* parent = 0,
		const char* name = 0, bool singleViewMode = false );
	~KexiProject();
	virtual DCOPObject* dcopObject();
	virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE,
		double zoomX = 1.0, double zoomY = 1.0 );

	virtual bool initDoc();

	virtual bool loadXML( QIODevice *, const QDomDocument & );
	virtual QDomDocument saveXML();


	void addFileReference(FileReference);
	void removeFileReference(const QString &location);
	void removeFileReference(FileReference);
	References fileReferences(const QString &group);

	bool initDBConnection(KexiProjectConnectionData *c, KoStore *store = 0);
	KexiDB::Connection *dbConnection() { return m_dbConnection; }
	KexiProjectConnectionData *dbConnectionData() { return m_dbConnectionData; }

	void clear();

	void registerProjectHandler(KexiProjectHandler *part);
	PartList *getParts();

	KexiDB::DriverManager* manager() {return m_dbDriverManager;}
	bool dbIsAvaible()const { return m_dbAvaible; }
	KexiFilterManager* const filterManager() {return m_filterManager;}
	QString boolToString(bool b);
	bool stringToBool(const QString &s);
	KexiRelation *relationManager();

	KexiProjectHandler *handlerForMime(const QString&);
	ProviderList providers(const QString &interfaceName);
	virtual bool completeLoading( KoStore* store );

	virtual bool openURL( const KURL & url );

public slots:
	void	slotImportFileData();
	void	slotImportServerData();

signals:
	void dbAvaible();
	void saving(KoStore *);
	void partListUpdated();

protected:
	virtual KoView* createViewInstance( QWidget* parent, const char* name );
	virtual bool completeSaving( KoStore* store );
	void setCurrentDB(){} ;
	bool saveProject();
	bool saveProjectAs(const QString&);
	bool loadProject(const QString&);

	void invokeStartupActions( KexiView *view );

private:
	void saveConnectionSettings(QDomDocument&);
	void saveReferences(QDomDocument&);
	void loadReferences(QDomElement&);
	void loadHandlers();
	KexiDoc*        m_settings;
	bool            m_dbAvaible;
	ReferencesM      m_fileReferences;
	Groups          m_refGroups;
	KexiRelation	*m_relationManager;
	PartList	*m_parts;
	DCOPObject	*dcop;
	bool		m_handlersLoaded;
	KexiDB::Connection *m_dbConnection;
	KexiDB::DriverManager *m_dbDriverManager;
	KexiProjectConnectionData *m_dbConnectionData;


	KexiFilterManager *m_filterManager;

	//! Flag used to ensure that KexiView::invokeActionsOnStartup()
	//! is invoked only once.
	bool m_invokeActionsOnStartup : 1;
};

#endif
