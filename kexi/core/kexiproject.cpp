/***************************************************************************
                          kexipart.cpp  -  description
                             -------------------
    begin                : Sun Nov  17 23:30:00 CET 2002
    copyright            : (C) 2002 Joseph Wenninger
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
                                          Daniel Molkentin <molkentin@kde.org>

    email                : jowenn@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kexiproject.h"
#include "kexiproject.moc"
#include "kexi_factory.h"
#include "kexiview.h"
#include "kexicreateprojectiface.h"
#include "kexirelation.h"
#include "kexiprojecthandler.h"

#include <koStore.h>

#include <kdebug.h>
#include <kpassdlg.h>
#include <klocale.h>
#include <kparts/componentfactory.h>

#include <qpainter.h>
#include <koTemplateChooseDia.h>
#include "KexiProjectIface.h"

#include <kexiDB/kexidberror.h>

KexiProject::KexiProject( QWidget *parentWidget, const char *widgetName, QObject* parent,
         const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
	dcop = 0;
	setInstance( KexiFactory::global(), false );
	kdDebug()<<"creating KexDB instance"<<endl;
	m_db = new KexiDB(this);
//	m_formManager=new KexiFormManager(this);
	m_relationManager=new KexiRelation(this);
	
	m_parts = new PartList();

	if ( name )
		dcopObject();
}

KexiProject::~KexiProject()
{
	delete dcop;
}

KexiRelation *KexiProject::relationManager() { return m_relationManager;}

DCOPObject* KexiProject::dcopObject()
{
	if ( !dcop )
		dcop = new KexiProjectIface( this );
	
	return dcop;
}

bool KexiProject::completeSaving( KoStore* store )
{
	m_relationManager->storeRelations(store);
	return true;
}

bool KexiProject::completeLoading( KoStore* store )
{
	m_relationManager->loadRelations(store);
	return true;
}


bool KexiProject::initDoc()
{
	QString filename;
	KoTemplateChooseDia::ReturnType ret=KoTemplateChooseDia::choose(KexiFactory::global(),filename,"application/x-vnd.kde.kexi","*.kexi",
		i18n("Kexi"),KoTemplateChooseDia::Everything,"kexi_template");
	bool ok=false;
	if (ret==KoTemplateChooseDia::Empty) {
		clear();
		QObject *newDlg = KParts::ComponentFactory::createInstanceFromLibrary<QObject>( "kexiprojectwizard", this );
		ok=(static_cast<KexiCreateProjectIface*>(newDlg->qt_cast("KexiCreateProjectIface"))->execute())==QDialog::Accepted;
		delete newDlg;
	} else if (ret==KoTemplateChooseDia::File) {
		KURL url(filename);
		kdDebug()<<"kexi: opening file: "<<url.prettyURL()<<endl;
		ok=openURL(url);
	}
	return ok;
}

KoView* KexiProject::createViewInstance( QWidget* parent, const char* name )
{
	kdDebug() << "KoView* KexiProject::createViewInstance()" << endl;
    	KexiView *v= new KexiView( KexiView::MDIWindowMode,this, parent, name );
        for(KexiProjectHandler *part = m_parts->first(); part; part = m_parts->next())
		part->hookIntoView(v);
	return v;
}

void KexiProject::saveConnectionSettings(QDomDocument &domDoc)
{
	QDomElement connectionElement = domDoc.createElement("connectionSettings");
	domDoc.documentElement().appendChild(connectionElement);
	
//DB ENGINE
	QDomElement engineElement = domDoc.createElement("engine");
	connectionElement.appendChild(engineElement);
	
	QDomText tEngine = domDoc.createTextNode(m_cred.driver);
	engineElement.appendChild(tEngine);
	
//HOST
	QDomElement hostElement = domDoc.createElement("host");
	connectionElement.appendChild(hostElement);
	
	QDomText tHost = domDoc.createTextNode(m_cred.host);
	hostElement.appendChild(tHost);

//DATABASE NAME	
	QDomElement nameElement = domDoc.createElement("name");
	connectionElement.appendChild(nameElement);
	
	QDomText tName = domDoc.createTextNode(m_cred.database);
	nameElement.appendChild(tName);

//USER
	QDomElement userElement = domDoc.createElement("user");
	connectionElement.appendChild(userElement);
	
	QDomText tUser = domDoc.createTextNode(m_cred.user);
	userElement.appendChild(tUser);
	
//PASSWORD STUFF
	QDomElement passElement = domDoc.createElement("password");
	connectionElement.appendChild(passElement);

	QDomText tPass=domDoc.createTextNode(m_cred.savePassword?m_cred.password:"");
	passElement.appendChild(tPass);

	QDomElement savePassElement = domDoc.createElement("savePassword");
	connectionElement.appendChild(savePassElement);

	QDomText tSavePass = domDoc.createTextNode(boolToString(m_cred.savePassword));
	savePassElement.appendChild(tSavePass);

}


void KexiProject::saveReferences(QDomDocument &domDoc)
{
	QDomElement refs = domDoc.createElement("references");
	domDoc.documentElement().appendChild(refs);

	kdDebug() << "KexiProject::saveProject(): storing " << m_fileReferences.count() << " references" << endl;
	for(References::Iterator it = m_fileReferences.begin(); it != m_fileReferences.end(); it++)
	{
		FileReference ref(*it);

		QDomElement item = domDoc.createElement("item");
		item.setAttribute("name", ref.name);
		item.setAttribute("location", ref.location);

		if(m_refGroups.contains(ref.group))
		{
			kdDebug() << "KexiProject::saveProject(): using existing group: " << ref.group << endl;
			(&m_refGroups[ref.group])->appendChild(item);
		}
		else
		{
			kdDebug() << "KexiProject::saveProject(): creating group: " << ref.group << endl;
			
			QDomElement group = domDoc.createElement(ref.group);
			group.appendChild(item);
			
			m_refGroups.insert(ref.group, group);
		}
	}
	
	for(Groups::Iterator itG = m_refGroups.begin(); itG != m_refGroups.end(); itG++)
	{
		refs.appendChild(itG.data());
	}

}

QDomDocument KexiProject::saveXML()
{
	kdDebug()<<"KexiProject::saveXML()"<<endl;
	QDomDocument domDoc=createDomDocument( "KexiProject", "1.0" );
	saveConnectionSettings(domDoc);
	saveReferences(domDoc);
	for (KexiProjectHandler *hand=m_parts->first();hand;hand=m_parts->next())
		hand->saveXML(domDoc);
	setModified(false);
	return domDoc;
}


void KexiProject::loadConnectionSettings(QDomElement &rootElement)
{
	QDomElement engineElement = rootElement.namedItem("engine").toElement();
	QDomElement hostElement = rootElement.namedItem("host").toElement();
	QDomElement nameElement = rootElement.namedItem("name").toElement();
	QDomElement userElement = rootElement.namedItem("user").toElement();
	QDomElement passElement = rootElement.namedItem("password").toElement();
	QDomElement savePassElement = rootElement.namedItem("savePassword").toElement();

	Credentials parsedCred;
	parsedCred.driver   = engineElement.text();
	parsedCred.host     = hostElement.text();
	parsedCred.database = nameElement.text();
	parsedCred.user     = userElement.text();
	parsedCred.password = passElement.text();
	parsedCred.savePassword = stringToBool(savePassElement.text());
	bool mod = false;

	if(!parsedCred.savePassword)
	{
		QCString password;
		int keep = 1;
		int result = KPasswordDialog::getPassword(password, i18n("Password for %1 on %2").arg(parsedCred.user)
			.arg(parsedCred.host), &keep);

		if(result == KPasswordDialog::Accepted)
		{
			parsedCred.password = password;

			if(keep)
			{
				parsedCred.savePassword = true;
				mod = true;
			}
		}
	}

	initDbConnection(parsedCred);
        setModified( isModified() | mod );

}


void KexiProject::loadReferences(QDomElement &fileRefs)
{
	QDomNodeList reflist = fileRefs.childNodes();
	kdDebug() << "KexiProject::loadProject(): looking up references: " << reflist.count() << endl;

	for(int ci = 0; ci < reflist.count(); ci++)
	{
		QDomNode groups = reflist.item(ci);
		QDomNodeList groupList = groups.childNodes();
		QString groupName = groups.toElement().tagName();
		kdDebug() << "KexiProject::loadProject(): looking up groups: " << groupList.count() << " for " << groupName << endl;
		for(int gi = 0; gi < groupList.count(); gi++)
		{
			QDomElement item = groupList.item(gi).toElement();
			QString name = item.attribute("name");
			QString location = item.attribute("location");

			FileReference ref;
			ref.group = groupName;
			ref.name = name;
			ref.location = location;

				qDebug("KexiProject::openProject(): #ref %s:%s:%s\n",groupName.latin1(),name.latin1(),location.latin1());

			m_fileReferences.append(ref);
		}
	}

}

bool KexiProject::loadXML( QIODevice *, const QDomDocument &domDoc )
{
	setModified(false);
	kdDebug()<<"KexiProject::loadXML"<<endl;
	QDomElement prE=domDoc.documentElement();
	for (QDomElement el=prE.firstChild().toElement();!el.isNull();el=el.nextSibling().toElement())
	{
		QString tagname=el.tagName();
		//perhaps the if's should be moved lateron into the methods alone
		if (tagname=="connectionSettings") loadConnectionSettings(el);
		else if (tagname=="references") loadReferences(el);
	}
	for (KexiProjectHandler *hand=m_parts->first();hand;hand=m_parts->next())
		hand->loadXML(domDoc);
	return true;
}




void KexiProject::paintContent( QPainter& /*painter*/, const QRect& /*rect*/, bool /*transparent*/,
                                double /*zoomX*/, double /*zoomY*/)
{
	
}

bool KexiProject::initDbConnection(const Credentials &cred, const bool create)
{
	kdDebug() << "KexiProject::initDbConnection()" << endl;

	kdDebug() << "KexiProject::initDbConnection(): engine:" << cred.driver << endl;
	kdDebug() << "KexiProject::initDbConnection(): host:" << cred.host << endl;
	kdDebug() << "KexiProject::initDbConnection(): user:" << cred.user << endl;
	kdDebug() << "KexiProject::initDbConnection(): database:" << cred.database << endl;


	if(m_db->driverName() != cred.driver)
	{
		kdDebug() << "KexiProject::initDBConnection(): abroating" << endl;
		initHostConnection(cred);
	}

	kdDebug() << "KexiProject::initDBConnection(): using simple method\n  because current driver is: " << m_db->driverName() << endl;

	try
	{
		m_db->connect(cred.host, cred.user, cred.password, cred.socket, cred.port, cred.database, create);
	}
	catch(KexiDBError *err)
	{
		kdDebug() << "KexiProject::initDbConnection(): connection failed: #need to implement" /*m_db->lastError().databaseText() */ << endl;
		err->toUser(0);
		m_cred = cred;
		return false;
	}

	m_cred = cred;
	kdDebug() << "KexiProject::initDbConnection(): loading succeeded" << endl;
	setModified( false );
	emit dbAvaible();
//		emit updateBrowsers();
	m_dbAvaible = true;
	loadHandlers();
//		new KexiTablePart(this);
//		new KexiQueryPart(this);
	kdDebug() << "KexiProject::initDbConnection(): db is avaible now..." << endl;
	return true;
}

bool
KexiProject::initHostConnection(const Credentials &cred)
{
	kdDebug() << "KexiProject::initHostConnection" << endl;
	KexiDB *addDB = m_db->add(cred.driver);
	if(addDB)
	{
		m_db = addDB;
	}
	else
	{
		return false;
	}

	if(!m_db->connect(cred.host, cred.user, cred.password, cred.socket, cred.port))
	{
		m_cred = cred;
		setModified( true );
		return false;
	}
	else
	{
		m_cred = cred;
		return true;
	}
}

bool
KexiProject::initFileConnection(const QString driver, const QString file)
{
	kdDebug() << "KexiProject::initFileConnection()" << endl;

	KexiDB *addDB = m_db->add(driver);
	if(addDB)
		m_db = addDB;
	else
		return false;

//	if(m_db->load(file))
//	{
	try
	{
		m_db->load(file);
	}
	catch(KexiDBError *err)
	{
		err->toUser(0);
		return false;
	}

	setModified( false );
	emit dbAvaible();
//	emit updateBrowsers();
	m_dbAvaible = true;
	loadHandlers();
	return true;
}

void
KexiProject::clear()
{
        setModified( false);
//	kexi->mainWindow()->slotProjectModified();
}

void
KexiProject::registerProjectHandler(KexiProjectHandler *part)
{
	kdDebug() << "KexiProject::registerProjectHandler()" << endl;
	m_parts->append(part);
	emit partListUpdated();
}

PartList*
KexiProject::getParts()
{
	return m_parts;
}

void
KexiProject::addFileReference(FileReference fileref)
{
	m_fileReferences.append(fileref);
}

QString
KexiProject::boolToString(bool b)
{
	if(b)
	{
		return QString("TRUE");
	}
	else
	{
		return QString("FALSE");
	}
}

bool KexiProject::stringToBool(const QString &s)
{
    return s == "TRUE" ? true : false ;
}

References
KexiProject::fileReferences(const QString &group)
{
	kdDebug() << "KexiProject::fileReferences(" << group << ")" << endl;
	References refs;
	for(References::Iterator it = m_fileReferences.begin(); it != m_fileReferences.end(); it++)
	{
		if((*it).group == group)
		{
			kdDebug() << "KexiProject::fileReferences() found a matching item: " << group << endl;
			refs.append(*it);
		}
	}
	return refs;
}


void KexiProject::loadHandlers()
{
	kdDebug()<<"***********************Trying to load handlers"<<endl;
	KTrader::OfferList ol=KTrader::self()->query("Kexi/Handler");
	for (KTrader::OfferList::ConstIterator it=ol.begin(); it!=ol.end(); ++it)
	{
		(void) KParts::ComponentFactory::createInstanceFromService<KexiProjectHandler>(
			*it,this);	
	}
}
