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
#include <kexiDB/kexidb.h>
#include <kexiDB/kexidberror.h>

class KexiDoc;
class DCOPObject;
class KexiRelation;
class KexiProjectHandler;
class KexiDBConnection;

typedef QPtrList<KexiProjectHandler> ProviderList;

class FileReference
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

/*
struct Credentials
{
	KexiDB::DBType type;
	QString host,
		database,
		port,
		driver,
		user,
		password,
		socket;
	bool savePassword;
};
*/

typedef QMap<QString, QDomElement> Groups;
typedef QValueList<FileReference> References;
typedef QMap<QString, FileReference> ReferencesM;
typedef QPtrList<KexiProjectHandler> PartList;

class KexiProject : public KoDocument
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

	bool initDBConnection(KexiDBConnection *c, KoStore *store = 0);

	void clear();

	void registerProjectHandler(KexiProjectHandler *part);
	PartList *getParts();

	KexiDB* db()const { return m_db; };
	bool dbIsAvaible()const { return m_dbAvaible; }
	QString boolToString(bool b);
	bool stringToBool(const QString &s);
	KexiRelation *relationManager();

	KexiProjectHandler *handlerForMime(const QString&);
	ProviderList providers(const QString &interfaceName);
signals:
	void dbAvaible();
	void saving(KoStore *);
	void partListUpdated();

protected:
	virtual KoView* createViewInstance( QWidget* parent, const char* name );
	virtual bool completeSaving( KoStore* store );
	virtual bool completeLoading( KoStore* store );
	void setCurrentDB(){} ;
	bool saveProject();
	bool saveProjectAs(const QString&);
	bool loadProject(const QString&);

private:
	void saveConnectionSettings(QDomDocument&);
	void saveReferences(QDomDocument&);
	void loadReferences(QDomElement&);
	void loadHandlers();
	KexiDoc*        m_settings;
	KexiDB*         m_db;
//	Credentials     m_cred;
	bool            m_dbAvaible;
	ReferencesM      m_fileReferences;
	Groups          m_refGroups;
	KexiRelation	*m_relationManager;
	PartList	*m_parts;
	DCOPObject	*dcop;
	bool		m_handlersLoaded;
	KexiDBConnection *m_dbconnection;
};

#endif
