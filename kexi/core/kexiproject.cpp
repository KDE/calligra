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
#include <qptrvector.h>

#include <kdebug.h>
#include <kpassdlg.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kparts/componentfactory.h>
#include <kstandarddirs.h>
#include <kconfig.h>
#include <kcmdlineargs.h>

#include <koStore.h>
#include <koTemplateChooseDia.h>

#include <kexidb/drivermanager.h>
#include "KexiProjectIface.h"
#include "kexiproject.h"
#include "kexiproject.moc"
#include "kexi_factory.h"
#include "kexiview.h"
#include "kexicreateprojectiface.h"
#include "kexirelation.h"
#include "kexiprojecthandler.h"
#include "kexiprojecthandlerproxy.h"
#include "kexidb/connection.h"
#include "filters/kexifiltermanager.h"
#include "kexiprojectconnectiondata.h"

#include "koApplication.h"
#include "kexi_global.h"
#include "kexi_utils.h"
#include "kexiworkspaceMDI.h"



KexiProject::KexiProject( QWidget *parentWidget, const char *widgetName, QObject* parent,
         const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode ),
      m_handlersLoaded(false),
      m_dbConnection(0)
{
	kdDebug()<<"KexiProject::KexiProject()"<<endl;
	dcop = 0;
	setInstance( KexiFactory::global(), false );
	m_dbDriverManager = new KexiDB::DriverManager;
	m_filterManager=new KexiFilterManager(this);
	m_relationManager=new KexiRelation(this);
	m_invokeActionsOnStartup = true;

	m_parts = new PartList();

	if ( name )
		dcopObject();
}

KexiProject::~KexiProject()
{
	delete m_dbConnection;
	delete dcop;
	delete m_dbDriverManager;
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
#ifndef Q_WS_WIN
#warning FIXME
#endif
//	m_dbconnection->flush(store);
	m_relationManager->storeRelations(store);
	for (KexiProjectHandler *hand=m_parts->first();hand;hand=m_parts->next())
		hand->store(store);
	return true;
}

bool KexiProject::completeLoading( KoStore* store )
{
	if(!initDBConnection(m_dbConnectionData, store))
		return false;

	m_relationManager->loadRelations(store);
	for (KexiProjectHandler *hand=m_parts->first();hand;hand=m_parts->next())
		hand->load(store);
	return true;
}


bool KexiProject::initDoc()
{
	QString filename;

	KoTemplateChooseDia::DialogType dlgtype;

#ifndef OOPL_VERSION
	if (initDocFlags() != KoDocument::InitDocFileNew)
        	dlgtype = KoTemplateChooseDia::Everything;
	else
        	dlgtype = KoTemplateChooseDia::OnlyTemplates;
#else //for ancient kofficelibs
        	dlgtype = KoTemplateChooseDia::Everything;
#endif
	
	KoTemplateChooseDia::ReturnType ret=KoTemplateChooseDia::choose(
		KexiFactory::global(),filename,"application/x-vnd.kde.kexi","*.kexi",
		KEXI_APP_NAME,dlgtype,"kexi_template");

	bool ok=false;
	if (ret==KoTemplateChooseDia::Empty) {
		clear();
		loadHandlers();


		QObject *newDlg = KParts::ComponentFactory::
			createInstanceFromLibrary<QObject>( "kexiprojectwizard", this );
		if (newDlg) {
		ok=(static_cast<KexiCreateProjectIface*>(newDlg->
			qt_cast("KexiCreateProjectIface"))->execute())==QDialog::Accepted;
		QString newProjectFileName=(static_cast<KexiCreateProjectIface*>(newDlg->
			qt_cast("KexiCreateProjectIface")))->projectFileName();
		delete newDlg;
		}

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
	KexiView *v= new KexiView( KexiView::DefaultMode,this, parent, name );
	for(KexiProjectHandler *part = m_parts->first(); part; part = m_parts->next())
		part->hookIntoView(v);
	v->finalizeInit();
	//invoke startup actions if document is not empty (ie. is loaded)
//	if (!isEmpty() && m_invokeActionsOnStartup) {
//		m_invokeActionsOnStartup = false; //only once
//		v->invokeStartupActions();
//	}
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
	m_dbConnectionData->writeInfo(domDoc);
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

	for(int ci = 0; ci < int(reflist.count()); ci++)
	{
		QDomNode groups = reflist.item(ci);
		QDomNodeList groupList = groups.childNodes();
		QString groupName = groups.toElement().tagName();
		kdDebug() << "KexiProject::loadProject(): looking up groups: " << groupList.count() << " for " << groupName << endl;
		for(int gi = 0; gi < int(groupList.count()); gi++)
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
			m_dbConnectionData = KexiProjectConnectionData::loadInfo(el);
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
KexiProject::initDBConnection(KexiProjectConnectionData *connection, KoStore *store)
{
#ifndef Q_WS_WIN
#warning FIXME
#endif
	return false;
/*
	if(!connection)
		return false;

	m_dbconnection = connection;
	m_db = connection->connectDB(m_dbInterfaceManager, store);
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
//		KMessageBox::error(0, i18n("Connection to database failed."), i18n("Database Connection"));
		return false;
	}
*/
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
	
	KConfig conf("kexirc", true);
	conf.setGroup("Parts");
	QStringList sl_order = QStringList::split( ",", conf.readEntry("Order") );
	
	KTrader::OfferList ol=KTrader::self()->query("Kexi/Handler");
	
	const int size = QMAX( ol.count(), sl_order.count() );
	int offset = size;
	QPtrVector<KService> ordered( size*2 );
	
	for (KTrader::OfferList::ConstIterator it=ol.begin(); it!=ol.end(); ++it)
	{
		KService::Ptr ptr = *it;
		kdDebug() << "loadHandlers(): library=" << ptr->library() << endl;
		int idx = sl_order.findIndex( ptr->library() );
		if (idx!=-1)
			ordered.insert(idx, (KService*)ptr);
		else //add to end
			ordered.insert(offset++, (KService*)ptr);
	}
	for (int i = 0; i< (int)ordered.size(); i++) {
		KService::Ptr ptr = ordered[i];
		if (ptr) {
			kdDebug() << "loadHandlers(): adding library=" << ptr->library() << endl;
			(void) KParts::ComponentFactory::createInstanceFromService<KexiProjectHandler>(
				ptr,this);
		}
	}
}

void KexiProject::slotImportFileData()
{
#ifndef KEXI_NO_UNFINISHED

	m_filterManager->import(KexiFilterManager::File,KexiFilterManager::AllEntries);
#else
	KEXI_UNFINISHED(i18n("Import Data"));
#endif
}

void KexiProject::slotImportServerData()
{
	m_filterManager->import(KexiFilterManager::Server,KexiFilterManager::AllEntries);
}

/*! Here we try to invoke actions defined to be automatically
	invoked on application startup. Now these are specified with 
	--open command line option (see main.cpp).
	Actions in this method are invoked only once.
*/
void KexiProject::invokeStartupActions( KexiView *view )
{
	if (isEmpty() || !m_invokeActionsOnStartup)
		return;
	m_invokeActionsOnStartup = false; //only once

	KCmdLineArgs *args = KCmdLineArgs::parsedArgs(0);
	if (!args)
		return;
	QString not_found_msg;
	QCStringList list = args->getOptionList("open");
	QCStringList::const_iterator it;
	for ( it = list.begin(); it!=list.end(); ++it) {
		QString type_name, obj_name, item=*it;
		int idx;
		//option with " " (type: default)
		if (item.left(1)=="\"" && item.right(1)=="\"") {
			obj_name = item.mid(1, item.length()-2);
			type_name = "table";
		}
		//option with type name specified:
		else if ((idx = item.find(':'))!=-1) {
			type_name = item.left(idx).lower();
			obj_name = item.mid(idx+1);
			//optional: remove ""
			if (obj_name.left(1)=="\"" && obj_name.right(1)=="\"")
				obj_name = obj_name.mid(1, obj_name.length()-2);
		}
		//just obj. name: type name is "table" by default
		else {
			obj_name = item;
			type_name = "table";
		}
		if (type_name.isEmpty() || obj_name.isEmpty())
			continue;
		//ok, now open this object
		QString obj_mime = QString("kexi/") + type_name;
		QString obj_identifier = obj_mime + "/" + obj_name;
		KexiProjectHandler *hd = handlerForMime(obj_mime);
		KexiProjectHandlerProxy *pr = hd ? hd->proxy(view) : 0;
		if (!pr || !pr->executeItem(obj_identifier)) {
			if (!not_found_msg.isEmpty())
				not_found_msg += ",<br>";
			not_found_msg += (pr ? pr->part()->name() : i18n("Unknown object")) + " \"" + obj_name + "\"";
		}
	}
	if (!not_found_msg.isEmpty())
		KMessageBox::sorry(0, "<p><b>" + i18n("Requested objects cannot be opened:") + "</b><p>" + not_found_msg );
}

bool KexiProject::openURL( const KURL & url )
{
	bool ret = KoDocument::openURL( url );
	if (!ret)
		return false;
	QPtrListIterator<KoView> it(views());
	KoView *view = it.current();
	if (view) {
		invokeStartupActions( static_cast<KexiView*>(view) );
	}
	return true;
}
