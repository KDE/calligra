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
#include "kexicreateproject.h"

#include <koStore.h>

#include <kdebug.h>
#include <kpassdlg.h>
#include <klocale.h>

#include <qpainter.h>
#include <koTemplateChooseDia.h>
#include "KexiProjectIface.h"

KexiProject::KexiProject( QWidget *parentWidget, const char *widgetName, QObject* parent,
         const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
    dcop = 0;
    setInstance( KexiFactory::global(), false );
    kdDebug()<<"creating KexDB instance"<<endl;
    m_db = new KexiDB(this);
    m_formManager=new KexiFormManager(this);
    m_url = "";
    if ( name )
        dcopObject();

}

KexiProject::~KexiProject()
{
    delete dcop;
}

DCOPObject* KexiProject::dcopObject()
{
    if ( !dcop )
        dcop = new KexiProjectIface( this );
    return dcop;
}


bool KexiProject::initDoc()
{
	QString filename;
	KoTemplateChooseDia::ReturnType ret=KoTemplateChooseDia::choose(KexiFactory::global(),filename,"application/x-kexi","*.kex",
		i18n("Kexi"),KoTemplateChooseDia::Everything,"kexi_template");
	bool ok=false;
	if (ret==KoTemplateChooseDia::Empty) {
		clear();
		KexiCreateProject *newDlg = new KexiCreateProject(this,0);
		newDlg->exec();
		delete newDlg;
		ok=true;
	} else if (ret==KoTemplateChooseDia::File) {
		KURL url(filename);
		kdDebug()<<"kexi: opening file: "<<url.prettyURL()<<endl;
		ok=openURL(url);
	}
	return ok;
}

KoView* KexiProject::createViewInstance( QWidget* parent, const char* name )
{
    return new KexiView( KexiView::MDIWindowMode,this, parent, name );
}

bool KexiProject::loadXML( QIODevice *, const QDomDocument & )
{
    // TODO load the document from the QDomDocument
    return true;
}

QDomDocument KexiProject::saveXML()
{
    // TODO save the document into a QDomDocument
    return QDomDocument();
}

void KexiProject::paintContent( QPainter& /*painter*/, const QRect& /*rect*/, bool /*transparent*/,
                                double /*zoomX*/, double /*zoomY*/)
{
}


bool
KexiProject::saveProject()
{
	if(m_url.isEmpty())
		return false;

	KoStore* store = KoStore::createStore(m_url, KoStore::Write, "application/x-kexi");
	if(store)
	{
		emit saving(store);
	}

	QDomDocument domDoc("KexiProject");
	domDoc.appendChild(domDoc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));

	QDomElement projectElement = domDoc.createElement("KexiProject");
	domDoc.appendChild(projectElement);

	QDomElement engineElement = domDoc.createElement("engine");
	projectElement.appendChild(engineElement);

	QDomText tEngine = domDoc.createTextNode(m_cred.driver);
	engineElement.appendChild(tEngine);

	QDomElement hostElement = domDoc.createElement("host");
	projectElement.appendChild(hostElement);

	QDomText tHost = domDoc.createTextNode(m_cred.host);
	hostElement.appendChild(tHost);

	QDomElement nameElement = domDoc.createElement("name");
	projectElement.appendChild(nameElement);

	QDomText tName = domDoc.createTextNode(m_cred.database);
	nameElement.appendChild(tName);

	QDomElement userElement = domDoc.createElement("user");
	projectElement.appendChild(userElement);

	QDomText tUser = domDoc.createTextNode(m_cred.user);
	userElement.appendChild(tUser);

	QDomElement passElement = domDoc.createElement("password");
	projectElement.appendChild(passElement);

//	QDomText tPass = domDoc.createTextNode(m_cred.password);
//	passElement.appendChild(tPass);

	QDomText tPass;

	if(m_cred.savePassword)
	{
		tPass = domDoc.createTextNode(m_cred.password);
	}
	else
	{
		tPass = domDoc.createTextNode("");
	}

	passElement.appendChild(tPass);

	QDomElement savePassElement = domDoc.createElement("savePassword");
	projectElement.appendChild(savePassElement);

	QDomText tSavePass = domDoc.createTextNode(boolToString(m_cred.savePassword));
	savePassElement.appendChild(tSavePass);

	QDomElement refs = domDoc.createElement("references");
	projectElement.appendChild(refs);

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
/*
		QDomElement ref = domDoc.createElement("embedded");
		refs.appendChild(ref);

		QDomText tref = domDoc.createTextNode((*it));
		ref.appendChild(tref);
	}

*/
	QByteArray data = domDoc.toCString();
	data.resize(data.size()-1);

	if(store)
	{
		if(store->open("/project.xml"))
		{
			store->write(data);
			store->close();
		}

		delete store;
                setModified( false);
//		kexi->mainWindow()->slotProjectModified();
		return true;
	}

	return false;
}

bool
KexiProject::saveProjectAs(const QString& url)
{
	m_url = url;
	return saveProject();
}

bool
KexiProject::loadProject(const QString& url)
{
	m_url = url;
	KoStore* store = KoStore::createStore(m_url, KoStore::Read, "application/x-kexi");

	if(!store)
	{
		return false;
	}

	store->open("/project.xml");
	QDomDocument inBuf;

	//error reporting
	QString errorMsg;
	int errorLine;
	int errorCol;

	bool parsed = inBuf.setContent(store->device(), false, &errorMsg, &errorLine, &errorCol);
	store->close();
	delete store;

	if(!parsed)
	{
		kdDebug() << "coudn't parse:" << endl;
		kdDebug() << "error: " << errorMsg << " line: " << errorLine << " col: " << errorCol << endl;
		return false;
	}


	QDomElement projectData = inBuf.namedItem("KexiProject").toElement();

	QDomElement engineElement = projectData.namedItem("engine").toElement();
	QDomElement hostElement = projectData.namedItem("host").toElement();
	QDomElement nameElement = projectData.namedItem("name").toElement();
	QDomElement userElement = projectData.namedItem("user").toElement();
	QDomElement passElement = projectData.namedItem("password").toElement();
	QDomElement savePassElement = projectData.namedItem("savePassword").toElement();

	QDomElement fileRefs = projectData.namedItem("references").toElement();
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
        setModified( mod );

//	kexi->mainWindow()->slotProjectModified();

	kdDebug() << "File opened!" << endl;

	return true;
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
	if(m_db->connect(cred.host, cred.user, cred.password, cred.socket, cred.port, cred.database, create))
	{
		m_cred = cred;
		kdDebug() << "KexiProject::initDbConnection(): loading succeeded" << endl;
                setModified( false );
//		kexi->mainWindow()->slotProjectModified();
		emit dbAvaible();
		kdDebug() << "KexiProject::initDbConnection(): db is avaible now..." << endl;
		return true;
	}
	else
	{
		kdDebug() << "KexiProject::initDbConnection(): connection failed: #need to implement" /*m_db->lastError().databaseText() */ << endl;
		m_cred = cred;
		return false;
	}
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

void
KexiProject::clear()
{
	m_url = "";
        setModified( false);
//	kexi->mainWindow()->slotProjectModified();
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


