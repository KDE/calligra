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

#include <qpainter.h>
#include <qdir.h>

#include <kdebug.h>
#include <kpassdlg.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kparts/componentfactory.h>
#include <kstandarddirs.h>

#include <koStore.h>
#include <koTemplateChooseDia.h>

#include <kexiDB/kexidberror.h>

#include "KexiProjectIface.h"
#include "kexiproject.h"
#include "kexiproject.moc"
#include "kexi_factory.h"
#include "kexiview.h"
#include "kexicreateprojectiface.h"
#include "kexirelation.h"
#include "kexiprojecthandler.h"
#include "kexidbconnection.h"

KexiProject::KexiProject( QWidget *parentWidget, const char *widgetName, QObject* parent,
         const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode ),m_handlersLoaded(false)
{
	dcop = 0;
	setInstance( KexiFactory::global(), false );
	m_db = new KexiDB(this, "db");
	m_dbconnection = new KexiDBConnection();
	m_relationManager=new KexiRelation(this);

	m_parts = new PartList();

	if ( name )
		dcopObject();
}

KexiProject::~KexiProject()
{
	m_dbconnection->clean();
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
	m_dbconnection->flush(store);
	m_relationManager->storeRelations(store);
	for (KexiProjectHandler *hand=m_parts->first();hand;hand=m_parts->next())
		hand->store(store);
	return true;
}

bool KexiProject::completeLoading( KoStore* store )
{
	if(!initDBConnection(m_dbconnection, store))
		return false;

	m_relationManager->loadRelations(store);
	for (KexiProjectHandler *hand=m_parts->first();hand;hand=m_parts->next())
		hand->load(store);
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
		loadHandlers();
		QObject *newDlg = KParts::ComponentFactory::createInstanceFromLibrary<QObject>( "kexiprojectwizard", this );
		ok=(static_cast<KexiCreateProjectIface*>(newDlg->qt_cast("KexiCreateProjectIface"))->execute())==QDialog::Accepted;
		delete newDlg;
	} else if (ret==KoTemplateChooseDia::File) {
		loadHandlers();
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
/*
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
*/
}


void KexiProject::saveReferences(QDomDocument &domDoc)
{
	QDomElement refs = domDoc.createElement("references");
	domDoc.documentElement().appendChild(refs);

	kdDebug() << "KexiProject::saveProject(): storing " << m_fileReferences.count() << " references" << endl;
	for(ReferencesM::Iterator it = m_fileReferences.begin(); it != m_fileReferences.end(); it++)
	{
		FileReference ref = it.data();

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
	m_dbconnection->writeInfo(domDoc);
	saveReferences(domDoc);
	for (KexiProjectHandler *hand=m_parts->first();hand;hand=m_parts->next())
		hand->saveXML(domDoc);
	setModified(false);
	return domDoc;
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

			m_fileReferences.insert(ref.location, ref);
		}
	}

}

bool KexiProject::loadXML( QIODevice *, const QDomDocument &domDoc )
{
	loadHandlers();
	setModified(false);
	kdDebug()<<"*********KexiProject::loadXML**********"<<endl;
	QDomElement prE=domDoc.documentElement();
	for (QDomElement el=prE.firstChild().toElement();!el.isNull();el=el.nextSibling().toElement())
	{
		QString tagname=el.tagName();
		//perhaps the if's should be moved lateron into the methods alone
		if (tagname=="connectionSettings")
			m_dbconnection = KexiDBConnection::loadInfo(el);
		else if (tagname=="references")
			loadReferences(el);
		else {
			kdDebug()<<"Trying to find a part capable of handling node \""<<tagname<<"\""<<endl;
			for (KexiProjectHandler *hand=m_parts->first();hand;hand=m_parts->next())
			hand->loadXML(domDoc,el);
		}
	}
	return true;
}

void KexiProject::paintContent( QPainter& /*painter*/, const QRect& /*rect*/, bool /*transparent*/,
                                double /*zoomX*/, double /*zoomY*/)
{

}

bool
KexiProject::initDBConnection(KexiDBConnection *connection, KoStore *store)
{
	if(!connection)
		return;

	m_dbconnection = connection;
	m_db = connection->connectDB(m_db, store);
	kdDebug() << "KexiProject::initDBConnection()" << endl;
	if(m_db)
	{
		kdDebug() << "KexiProject::initDBConnection(): succeeded" << endl;
		setModified( false );
		emit dbAvaible();
		m_dbAvaible = true;
		loadHandlers();

		return true;
	}
	else
	{
		kdDebug() << "KexiProject::initDBConnection(): faild" << endl;
//		KMessageBox::error(0, i18n("connection to database faild"), i18n("Database Connection"));
		return false;
	}
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

KexiProjectHandler *
KexiProject::handlerForMime(const QString& mime)
{
	for (KexiProjectHandler *h=m_parts->first();h;h=m_parts->next())
	{
		if (h->mime()==mime) return h;
	}
	return 0;
}

ProviderList
KexiProject::providers(const QString &interfaceName)
{
	ProviderList l;
	for (KexiProjectHandler *h=m_parts->first();h;h=m_parts->next())
    {
        if (h->qt_cast(interfaceName.latin1())) l.append(h);
    }
	return l;
}



PartList*
KexiProject::getParts()
{
	return m_parts;
}

void
KexiProject::addFileReference(FileReference fileref)
{
//	if(m_fileReferences.findIndex(fileref) != -1)
		m_fileReferences.insert(fileref.location, fileref);
}

void
KexiProject::removeFileReference(const QString &location)
{
//	if(m_fileReferences.findIndex(fileref) != -1)
	m_fileReferences.remove(location);
}

void
KexiProject::removeFileReference(FileReference fileref)
{
//	if(m_fileReferences.findIndex(fileref) != -1)
	m_fileReferences.remove(fileref.location);
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
	for(ReferencesM::Iterator it = m_fileReferences.begin(); it != m_fileReferences.end(); it++)
	{
		if(it.data().group == group)
		{
			kdDebug() << "KexiProject::fileReferences() found a matching item: " << group << endl;
			refs.append(it.data());
		}
	}
	return refs;
}


void KexiProject::loadHandlers()
{
	if (m_handlersLoaded) return;
	m_handlersLoaded=true;
	kdDebug()<<"***********************Trying to load handlers"<<endl;
	KTrader::OfferList ol=KTrader::self()->query("Kexi/Handler");
	for (KTrader::OfferList::ConstIterator it=ol.begin(); it!=ol.end(); ++it)
	{
		(void) KParts::ComponentFactory::createInstanceFromService<KexiProjectHandler>(
			*it,this);
	}
}

