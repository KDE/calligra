/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   Modified by Joseph wenninger, 2001

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

#include <qlabel.h>
#include <qpushbutton.h>
#include <klocale.h>

#include <kdebug.h>

#include "kwdoc.h"
#include "mailmerge.h"
#include "mailmerge.moc"
#include <koVariableDlgs.h>
#include <kmainwindow.h>
#include "defs.h"
#include <qfile.h>
#include <kstdaction.h>
#include <kaction.h>
#include <kmessagebox.h>

/******************************************************************
 *
 * Class: KWMailMergeDataBase
 *
 ******************************************************************/

KWMailMergeDataBase::KWMailMergeDataBase( KWDocument *doc_ )
    : QObject(doc_,doc_->dcopObject()->objId()+".MailMergeDataBase"),
	KWordMailMergeDatabaseIface(QCString(doc_->dcopObject()->objId()+".MailMergeDataBase")),doc( doc_ ) {
   plugin=0; //loadPlugin("classic");
   rejectdcopcall=false;
}

QStringList KWMailMergeDataBase::availablePlugins()
{
	QStringList tmp;
	KTrader::OfferList pluginOffers=KTrader::self()->query(QString::fromLatin1("KWord/MailMergePlugin"),QString::null);
	for (KTrader::OfferList::Iterator it=pluginOffers.begin();*it;++it)
	{
		tmp.append((*it)->property("X-KDE-InternalName").toString());
		kdDebug()<<"Found mail merge plugin: "<< (*it)->name()<<endl;
	}
	return tmp;
}

bool KWMailMergeDataBase::isConfigDialogShown()
{
	return rejectdcopcall;
}

bool KWMailMergeDataBase::loadPlugin(QString name,QString command)
{
        if (rejectdcopcall)return false;
	QString constrain=QString("[X-KDE-InternalName] =='"+name+"'");
	kdDebug()<<constrain<<endl;
	KTrader::OfferList pluginOffers=KTrader::self()->query(QString::fromLatin1("KWord/MailMergePlugin"),constrain);
	KService::Ptr it=pluginOffers.first();
	if (it)
	{
		KWMailMergeDataSource *tmp=loadPlugin(it->library());
		if (!tmp)
		{
			kdDebug()<<"Couldn't load plugin"<<endl;
			return false; //Plugin couldn't be loaded
		}
		//Plugin found and loaded
		if (command=="silent") return askUserForConfirmationAndConfig(tmp,false,0);
		else
		{
			if (command=="open") action=KWSLOpen;
			else if (command=="create") action=KWSLCreate;
			else action=KWSLUnspecified;
			return askUserForConfirmationAndConfig(tmp,true,0);
		}
	}
	else
	{
		kdDebug()<<"No plugin found"<<endl;
		return false; //No plugin with specified name found
	}
}

KWMailMergeDataSource *KWMailMergeDataBase::openPluginFor(int type)
{
	KWMailMergeDataSource *ret=0;
	QString constrain=QString("'%1' in [X-KDE-Capabilities]").arg(((type==KWSLCreate)?KWSLCreate_text:KWSLOpen_text));
	kdDebug()<<constrain<<endl;
	KTrader::OfferList pluginOffers=KTrader::self()->query(QString::fromLatin1("KWord/MailMergePlugin"),constrain);

	//Only for debugging
	for (KTrader::OfferList::Iterator it=pluginOffers.begin();*it;++it)
	{
		kdDebug()<<"Found mail merge plugin: "<< (*it)->name()<<endl;
	}

	if (!pluginOffers.count())
	{
		//Sorry no suitable plugins found
		kdDebug()<<"No plugins found"<<endl;
		KMessageBox::sorry(0,i18n("No plugins supporting the requested action were found"));
	}
	else
	{
		KWMailMergeChoosePluginDialog *dia=new KWMailMergeChoosePluginDialog(pluginOffers);
		if (dia->exec()==QDialog::Accepted)
		{
			ret=loadPlugin((*(pluginOffers.at(dia->
			chooser->currentItem())))->library());
		}

	}
	return ret;
}

KWMailMergeDataSource *KWMailMergeDataBase::loadPlugin(const QString& name)
{
  if (!name.isEmpty())
  {
      // get the library loader instance

      KLibLoader *loader = KLibLoader::self();

      // try to load the library
	QString libname=name;
//      QString libname("lib%1");
      KLibrary *lib = loader->library(QFile::encodeName(libname));
      if (lib)
	{
	  // get the create_ function
	  QString factory=QString("create_%1").arg(name);
	  void *create = lib->symbol(QFile::encodeName(factory));

	  if (create)
	    {
	      // create the module
	      KWMailMergeDataSource * (*func)(KInstance*,QObject*);
	      func = (KWMailMergeDataSource* (*)(KInstance*,QObject*)) create;
	      KWMailMergeDataSource *tmpsource =func(KWFactory::global(),this);
	      if (tmpsource)
	      {
		QDataStream tmpstream(tmpsource->info,IO_WriteOnly);
		tmpstream<<name;
	      }
	      return tmpsource;
	    }

       }
      kdWarning() << "Couldn't load plugin " << name <<  endl;
  }
  else
  	kdWarning()<< "No plugin name specified" <<endl;
  return 0;
}

QString KWMailMergeDataBase::getValue( const QString &name, int record ) const
{
	if (plugin)
	{
		if (record==-1) record=doc->getMailMergeRecord();
		return plugin->getValue(name,record);
	}
	else
		return QString("");
}


void KWMailMergeDataBase::refresh(bool force)
{
	if (plugin) plugin->refresh(force);
}

const QMap< QString, QString > &KWMailMergeDataBase::getRecordEntries() const
{
	if (plugin)
		return plugin->getRecordEntries();
	else
		return emptyMap;
}

int KWMailMergeDataBase::getNumRecords() const
{
	if (plugin)
		return plugin->getNumRecords();
	else
		return 0;

}


void KWMailMergeDataBase::showConfigDialog(QWidget *par)
{
	rejectdcopcall=true;
	KWMailMergeConfigDialog *dia=new KWMailMergeConfigDialog(par,this);
	dia->exec();
	delete dia;
	rejectdcopcall=false;
}


bool KWMailMergeDataBase::askUserForConfirmationAndConfig(KWMailMergeDataSource *tmpPlugin,bool config,QWidget *par)
{
	if (tmpPlugin)
	{
		bool replaceit=false;
		if (!config) replaceit=true;
		else
			replaceit=tmpPlugin->showConfigDialog(par,action);
		if (replaceit)
		{
			if (plugin)
			{
				if (KMessageBox::warningContinueCancel(par,
					i18n("Do you really want to replace the current datasource?"),
					QString::null,QString::null,QString::null,true)== KMessageBox::Cancel)
				{
					delete tmpPlugin;
					tmpPlugin=0;
					return false;
				}
				delete plugin;
			}
			plugin=tmpPlugin;
		}
		else
		{
			delete tmpPlugin;
			tmpPlugin=0;
			return false;
		}
	}
	tmpPlugin->setObjId(QCString(objId()+".MailMergePlugin"));
	return true;
}



QDomElement KWMailMergeDataBase::save(QDomDocument &doc) const
{
	kdDebug()<<"KWMailMergeDataBase::save()"<<endl;
	QDomElement parentElem=doc.createElement("MAILMERGE");
	if (plugin)
	{
		kdDebug()<<"KWMailMergeDataBase::save() There is really something to save"<<endl;
		QDomElement el=doc.createElement(QString::fromLatin1("PLUGIN"));

		QDataStream ds(plugin->info,IO_ReadOnly);
		QString libname;
		ds>>libname;
		el.setAttribute("library",libname);
 		parentElem.appendChild(el);
		kdDebug()<<"KWMailMergeDataBase::save() Calling datasource save()"<<endl;
		QDomElement el2=doc.createElement(QString::fromLatin1("DATASOURCE"));
		plugin->save(doc,el2);
		parentElem.appendChild(el2);

	}
	kdDebug()<<"KWMailMergeDataBase::save() leaving now"<<endl;
	return parentElem;
//	if (plugin) plugin->save(parentElem); // Not completely sure, perhaps the database itself has to save something too (JoWenn)
}

void KWMailMergeDataBase::load( QDomElement& parentElem )
{
	QDomNode dn=parentElem.namedItem("PLUGIN");
	if (dn.isNull()) return;
	QDomElement el=dn.toElement();
	plugin=loadPlugin(el.attribute("library"));

	dn=parentElem.namedItem("DATASOURCE");
	if (dn.isNull()) return;
	el=dn.toElement();
	if (plugin) plugin->load(el);
}


/******************************************************************
 *
 * Class: KWMailMerge ChoosePluginDialog
 *
 ******************************************************************/

KWMailMergeChoosePluginDialog::KWMailMergeChoosePluginDialog(KTrader::OfferList pluginOffers)
    : KDialogBase(Plain, i18n( "Mail Merge - Configuration" ), Ok|Cancel, Ok, /*parent*/ 0, "", true )
{
	QWidget *back = plainPage();
	QVBoxLayout *layout=new QVBoxLayout(back);
	layout->setSpacing( KDialog::spacingHint() );
	layout->setMargin( KDialog::marginHint() );
	layout->setAutoAdd(true);
	QLabel *l = new QLabel( i18n( "&Available sources:" ),back );
   //l->setMaximumHeight( l->sizeHint().height() );
	chooser=new QComboBox(back);
	l->setBuddy(chooser);
	chooser->setEditable(false);
	for (KTrader::OfferList::Iterator it=pluginOffers.begin();*it;++it)
	{
		chooser->insertItem((*it)->name());
	}
	l=new QLabel((*pluginOffers.at(0))->comment(),back);
	l->setAlignment( WordBreak );
	l->setFrameShape( QFrame::Box );
	l->setFrameShadow( QFrame::Sunken );
	l->setMinimumSize(l->sizeHint());
   layout->addStretch();

}

KWMailMergeChoosePluginDialog::~KWMailMergeChoosePluginDialog()
{
}

/******************************************************************
 *
 * Class: KWMailMergeConfigDialog
 *
 ******************************************************************/

KWMailMergeConfigDialog::KWMailMergeConfigDialog(QWidget *parent,KWMailMergeDataBase *db)
    : KDialogBase(Plain, i18n( "Mail Merge - Configuration" ), Close, Close, parent, "", true )
{
    db_=db;
    QWidget *back = plainPage();
    QVBoxLayout *layout=new QVBoxLayout(back);
//    QVBox *back = new QVBox( page );
    layout->setSpacing( KDialog::spacingHint() );
    layout->setMargin( KDialog::spacingHint() );

//    QVBox *row1 = new QVBox( back );
//    row1->setSpacing( 5 );

    QLabel *l = new QLabel( i18n( "Datasource:" ),back );
//    l->setMaximumHeight( l->sizeHint().height() );
    layout->addWidget(l);

    QHBox *row1=new QHBox(back);
    layout->addWidget(row1);
    row1->setSpacing( KDialog::spacingHint() );
    edit=new QPushButton(i18n("Edit Current..."),row1);
    create=new QPushButton(i18n("Create New..."),row1);
    open=new QPushButton(i18n("Open Existing..."),row1);
    QFrame *Line1 = new QFrame( back, "Line1" );
    layout->addWidget(Line1);
    Line1->setFrameShape( QFrame::HLine );
    Line1->setFrameShadow( QFrame::Sunken );
    l = new QLabel( i18n( "Merging:" ),back );
	 layout->addWidget(l);
//  l->setMaximumHeight( l->sizeHint().height() );
    QHBox *row2=new QHBox(back);
    layout->addWidget(row2);
    row2->setSpacing( KDialog::spacingHint() );
    preview=new QPushButton(i18n("Print Preview..."),row2);
    document=new QPushButton(i18n("Create New Document"),row2);
    layout->addStretch();

    enableDisableEdit();

    connect(edit,SIGNAL(clicked()), this, SLOT(slotEditClicked()));
    connect(create,SIGNAL(clicked()),this,SLOT(slotCreateClicked()));
    connect(open,SIGNAL(clicked()),this,SLOT(slotOpenClicked()));
    connect(preview,SIGNAL(clicked()),this,SLOT(slotPreviewClicked()));
    connect(document,SIGNAL(clicked()),this,SLOT(slotDocumentClicked()));
}

void KWMailMergeConfigDialog::enableDisableEdit()
{
    if (!db_->plugin)
    	{
		preview->setEnabled(false);
		document->setEnabled(false);
		edit->setEnabled(false);
	}
	else
	{
		preview->setEnabled(true);
		document->setEnabled(true);
		edit->setEnabled(true);
	}
}

void KWMailMergeConfigDialog::slotEditClicked()
{db_->action=KWSLEdit;
 if (db_->plugin) db_->plugin->showConfigDialog((QWidget*)parent(),KWSLEdit);
}

void KWMailMergeConfigDialog::slotCreateClicked()
{
	db_->action=KWSLCreate;
	doNewActions();
//done(QDialog::Accepted);
}

void KWMailMergeConfigDialog::doNewActions()
{
	KWMailMergeDataSource *tmpPlugin=db_->openPluginFor(db_->action);
	if (tmpPlugin)
	{
		db_->askUserForConfirmationAndConfig(tmpPlugin,true,this);
		enableDisableEdit();
	}
}


void KWMailMergeConfigDialog::slotOpenClicked()
{
	db_->action=KWSLOpen;
	doNewActions();
}

void KWMailMergeConfigDialog::slotPreviewClicked()
{
	db_->action=KWSLMergePreview;
	KMainWindow *mw=dynamic_cast<KMainWindow*>(((QWidget *)parent())->topLevelWidget());
	if (mw)
	{
		KAction *ac=mw->actionCollection()->action(KStdAction::stdName(KStdAction::PrintPreview));
		if (ac) ac->activate();
		else kdWarning()<<"Toplevel doesn't provide a print preview action"<<endl;
	}
	else
		kdWarning()<<"Toplevel is no KMainWindow->no preview"<<endl;
}

void KWMailMergeConfigDialog::slotDocumentClicked()
{db_->action=KWSLMergeDocument;done(QDialog::Accepted);}

KWMailMergeConfigDialog::~KWMailMergeConfigDialog()
{
}

/******************************************************************
 *
 * Class: KWMailMergeVariableInsertDia
 *
 ******************************************************************/

KWMailMergeVariableInsertDia::KWMailMergeVariableInsertDia( QWidget *parent, KWMailMergeDataBase *db )
    : KDialogBase(Plain, i18n( "Mail Merge - Variable Name" ), Ok | Cancel, Ok, parent, "", true )
{
    QWidget *page = plainPage();

    back = new QVBox( page );
    back->setSpacing( KDialog::spacingHint() );
    back->setMargin( KDialog::marginHint() );

    QVBox *row1 = new QVBox( back );
    row1->setSpacing( KDialog::spacingHint() );

    QLabel *l = new QLabel( i18n( "Name:" ), row1 );
    l->setMaximumHeight( l->sizeHint().height() );
    names = new QListBox( row1 );

    QMap< QString, QString >::ConstIterator it = db->getRecordEntries().begin();
    for ( ; it != db->getRecordEntries().end(); ++it )
        names->insertItem( it.key(), -1 );

    setInitialSize( QSize( 350, 400 ) );
    connect(names,SIGNAL(selectionChanged () ),this,SLOT(slotSelectionChanged()));
    connect(names,SIGNAL(doubleClicked ( QListBoxItem * )),this,SLOT(slotOk()));
    setFocus();
    enableButtonOK(names->currentItem ()!=-1);
}

void KWMailMergeVariableInsertDia::slotSelectionChanged()
{
    enableButtonOK(names->currentItem ()!=-1);
}

QString KWMailMergeVariableInsertDia::getName() const
{
    return names->text( names->currentItem() );
}

void KWMailMergeVariableInsertDia::resizeEvent( QResizeEvent *e )
{
    QDialog::resizeEvent( e );
    back->resize( size() );
}
