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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KWDocument.h"
#include "KWMailMergeDataBase.h"
#include "KWMailMergeDataBase.moc"
#include <KoCustomVariablesDia.h>
#include "defs.h"
#include <Q3ListBox>
#include <QComboBox>
#include <klocale.h>
#include <kdebug.h>
#include <kstdaction.h>
#include <kaction.h>
#include <kmessagebox.h>
#include <kseparator.h>
#include <kservice.h>
#include <kmainwindow.h>

#include <qfile.h>

#include <qspinbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <Q3Frame>
#include <Q3VBoxLayout>
#include <kvbox.h>

/******************************************************************
 *
 * Class: KWMailMergeDataBase
 *
 ******************************************************************/

KWMailMergeDataBase::KWMailMergeDataBase( KWDocument *doc_ )
    : QObject(doc_,doc_->dcopObject()->objId()+".MailMergeDataBase"),
    KWordMailMergeDatabaseIface(DCOPCString(doc_->dcopObject()->objId()+".MailMergeDataBase")),
      m_version(0),
      doc( doc_ )
{
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
        kDebug()<<"Found mail merge plugin: "<< (*it)->name()<<endl;
    }
    return tmp;
}

bool KWMailMergeDataBase::isConfigDialogShown()
{
    return rejectdcopcall;
}

bool KWMailMergeDataBase::loadPlugin(const QString &name,const QString &command)
{
    if (rejectdcopcall)return false;
    QString constrain=QString("[X-KDE-InternalName] =='"+name+"'");
    kDebug()<<constrain<<endl;
    KTrader::OfferList pluginOffers=KTrader::self()->query(QString::fromLatin1("KWord/MailMergePlugin"),constrain);
    KService::Ptr it=pluginOffers.first();

    QVariant verProp=it->property("X-KDE-PluginVersion");
    int version=verProp.toInt();

    if (it)
    {
        KWMailMergeDataSource *tmp=loadPlugin(it->library());
        if (!tmp)
        {
            kDebug()<<"Couldn't load plugin"<<endl;
            return false; //Plugin couldn't be loaded
        }
        //Plugin found and loaded
        if (command=="silent") {
            return  askUserForConfirmationAndConfig(tmp,false,0,version);
        }
        else
        {
            if (command=="open") action=KWSLOpen;
            else if (command=="create") action=KWSLCreate;
            else action=KWSLUnspecified;
            return askUserForConfirmationAndConfig(tmp,true,0,version);
        }
    }
    else
    {
        kDebug()<<"No plugin found"<<endl;
        return false; //No plugin with specified name found
    }
}

KWMailMergeDataSource *KWMailMergeDataBase::openPluginFor(int type,int &version)
{
    version=0;
    KWMailMergeDataSource *ret=0;
    QString constrain=QString("'%1' in [X-KDE-Capabilities]").arg(((type==KWSLCreate)?KWSLCreate_text:KWSLOpen_text));
    kDebug()<<constrain<<endl;
    KTrader::OfferList pluginOffers=KTrader::self()->query(QString::fromLatin1("KWord/MailMergePlugin"),constrain);

    //Only for debugging
    for (KTrader::OfferList::Iterator it=pluginOffers.begin();*it;++it)
    {
        kDebug()<<"Found mail merge plugin: "<< (*it)->name()<<endl;
    }

    if (!pluginOffers.count())
    {
        //Sorry no suitable plugins found
        kDebug()<<"No plugins found"<<endl;
        KMessageBox::sorry(0,i18n("No plugins supporting the requested action were found."));
    }
    else
    {

        KWMailMergeChoosePluginDialog *dia=new KWMailMergeChoosePluginDialog(pluginOffers);
        if (dia->exec()==QDialog::Accepted)
        {
            KService::Ptr service = pluginOffers.at( dia->currentPlugin() );
            QVariant verProp = service->property("X-KDE-PluginVersion");
            version = verProp.toInt();

            ret = loadPlugin( service->library() );
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
      if (lib) {
          // get the create_ function
          QString factory=QString("create_%1").arg(name);
          void *create = lib->symbol(QFile::encodeName(factory));

          if (create)
          {
              // create the module
              KWMailMergeDataSource * (*func)(KInstance*,QObject*);
              func = (KWMailMergeDataSource* (*)(KInstance*,QObject*)) create;
              KWMailMergeDataSource *tmpsource =func(KWFactory::instance(),this);
              if (tmpsource)
              {
                  QDataStream tmpstream( &tmpsource->info,QIODevice::WriteOnly);
                  tmpstream.setVersion(QDataStream::Qt_3_1);
                  tmpstream<<name;
              }
              return tmpsource;
          }
      }
      kWarning() << "Couldn't load plugin " << name <<  endl;
  }
  else
      kWarning()<< "No plugin name specified" <<endl;
  return 0;
}

bool KWMailMergeDataBase::isSampleRecord() {
    return (0>doc->mailMergeRecord());
}

QString KWMailMergeDataBase::getValue( const QString &name, int record ) const
{
    if (plugin)
    {
        if (record==-1) record=doc->mailMergeRecord();
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


bool KWMailMergeDataBase::askUserForConfirmationAndConfig(KWMailMergeDataSource *tmpPlugin,bool config,QWidget *par,int version)
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
                if (KMessageBox::warningContinueCancel(
                        par,
                        i18n("Do you really want to replace the current datasource?"))
                    == KMessageBox::Cancel)
                {
                    delete tmpPlugin;
                    tmpPlugin=0;
                    return false;
                }
                delete plugin;
            }
            m_version=version;
            plugin=tmpPlugin;
        }
        else
        {
            delete tmpPlugin;
            tmpPlugin=0;
            return false;
        }
    }
    tmpPlugin->setObjId(DCOPCString(objId()+".MailMergePlugin"));
    return true;
}



QDomElement KWMailMergeDataBase::save(QDomDocument &doc) const
{
    kDebug()<<"KWMailMergeDataBase::save()"<<endl;
    QDomElement parentElem=doc.createElement("MAILMERGE");
    if (plugin)
    {
        kDebug()<<"KWMailMergeDataBase::save() There is really something to save"<<endl;
        QDomElement el=doc.createElement(QString::fromLatin1("PLUGIN"));

        QDataStream ds( &plugin->info,QIODevice::ReadOnly);
        ds.setVersion(QDataStream::Qt_3_1);
        QString libname;
        ds>>libname;
        el.setAttribute("library",libname);
        parentElem.appendChild(el);
        kDebug()<<"KWMailMergeDataBase::save() Calling datasource save()"<<endl;
        QDomElement el2=doc.createElement(QString::fromLatin1("DATASOURCE"));
        plugin->save(doc,el2);
        parentElem.appendChild(el2);

    }
    kDebug()<<"KWMailMergeDataBase::save() leaving now"<<endl;
    return parentElem;
    // if (plugin) plugin->save(parentElem); // Not completely sure, perhaps the database itself has to save something too (JoWenn)
}

void KWMailMergeDataBase::load( const QDomElement& parentElem )
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


int KWMailMergeDataBase::version() {
    kDebug()<<"KWMailMergeDataBase::version:"<<m_version<<endl;
    return m_version;
}


/******************************************************************
 *
 * Class: KWMailMerge ChoosePluginDialog
 *
 ******************************************************************/

KWMailMergeChoosePluginDialog::KWMailMergeChoosePluginDialog( KTrader::OfferList offers )
    : KDialogBase( Plain, i18n( "Mail Merge Setup" ), Ok | Cancel, Ok,
      /*parent*/ 0, "", true ), pluginOffers( offers )
{
  QWidget *back = plainPage();
  Q3VBoxLayout *layout = new Q3VBoxLayout( back, 0, spacingHint() );

  QLabel *label = new QLabel( i18n( "&Available sources:" ), back );
  chooser = new QComboBox( back );
  label->setBuddy( chooser );
  descriptionLabel = new QLabel( back );
  descriptionLabel->hide();
  descriptionLabel->setWordWrap( true );
  descriptionLabel->setFrameShape( Q3Frame::Box );
  descriptionLabel->setFrameShadow( Q3Frame::Sunken );

  QSize old_sizeHint;
  for ( KTrader::OfferList::Iterator it = pluginOffers.begin(); *it; ++it )
  {
    chooser->insertItem( (*it)->name() );
    old_sizeHint = descriptionLabel->sizeHint();
    descriptionLabel->setText( (*it)->comment() );
    if (descriptionLabel->sizeHint().width()*descriptionLabel->sizeHint().height() > old_sizeHint.width()*old_sizeHint.height())
        descriptionLabel->setMinimumSize(descriptionLabel->sizeHint() );
  }
  descriptionLabel->show();

  connect( chooser, SIGNAL( activated( int ) ),
           this, SLOT( pluginChanged( int ) ) );

  layout->addWidget( label );
  layout->addWidget( chooser );
  layout->addWidget( descriptionLabel );
  layout->addStretch( 1 );

  pluginChanged( 0 );
}

KWMailMergeChoosePluginDialog::~KWMailMergeChoosePluginDialog()
{
}

int KWMailMergeChoosePluginDialog::currentPlugin() const
{
  return chooser->currentItem();
}

void KWMailMergeChoosePluginDialog::pluginChanged( int pos )
{
  descriptionLabel->setText( pluginOffers.at( pos )->comment() );
}

/******************************************************************
 *
 * Class: KWMailMergeConfigDialog
 *
 ******************************************************************/

KWMailMergeConfigDialog::KWMailMergeConfigDialog(QWidget *parent,KWMailMergeDataBase *db)
    : KDialogBase(Plain, i18n( "Mail Merge Setup" ), Close, Close, parent, "", true )
{
    db_=db;
    QWidget *back = plainPage();
    Q3VBoxLayout *layout=new Q3VBoxLayout(back);
//    QVBox *back = new QVBox( page );
    layout->setSpacing( KDialog::spacingHint() );

//    QVBox *row1 = new QVBox( back );
//    row1->setSpacing( KDialog::spacingHint() );

    QLabel *l = new QLabel( i18n( "Datasource:" ),back );
//    l->setMaximumHeight( l->sizeHint().height() );
    layout->addWidget(l);

    KHBox *row1=new KHBox(back);
    layout->addWidget(row1);
    row1->setSpacing( KDialog::spacingHint() );
    edit=new QPushButton(i18n("Edit Current..."),row1);
    create=new QPushButton(i18n("Create New..."),row1);
    open=new QPushButton(i18n("Open Existing..."),row1);

    KSeparator *separator1 = new KSeparator(back);
    layout->addWidget(separator1);

    l = new QLabel( i18n( "Merging:" ),back );
    layout->addWidget(l);
//  l->setMaximumHeight( l->sizeHint().height() );
    KHBox *row2=new KHBox(back);
    layout->addWidget(row2);
    row2->setSpacing( KDialog::spacingHint() );
    preview=new QPushButton(i18n("Print Preview..."),row2);
    document=new QPushButton(i18n("Create New Document"),row2);
    document->hide();
    (void) new QWidget(row2);
    layout->addStretch();

    KSeparator *separator2 = new KSeparator(back);
    layout->addWidget(separator2);

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
    int tmpVersion;
    KWMailMergeDataSource *tmpPlugin=db_->openPluginFor(db_->action,tmpVersion);
    if (tmpPlugin)
    {
        if (db_->askUserForConfirmationAndConfig(tmpPlugin,true,this,tmpVersion))

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
        if (ac) ac->trigger();
        else kWarning()<<"Toplevel doesn't provide a print preview action"<<endl;
    }
    else
        kWarning()<<"Toplevel is no KMainWindow->no preview"<<endl;
}

void KWMailMergeConfigDialog::slotDocumentClicked()
{
    db_->action=KWSLMergeDocument;
    done(QDialog::Accepted);
}

KWMailMergeConfigDialog::~KWMailMergeConfigDialog()
{
}

/******************************************************************
 *
 * Class: KWMailMergeVariableInsertDia
 *
 ******************************************************************/

KWMailMergeVariableInsertDia::KWMailMergeVariableInsertDia( QWidget *parent, KWMailMergeDataBase *db )
  : KDialogBase( Plain, i18n( "Mail Merge - Variable Name" ),
                 Ok | Cancel, Ok, parent, "", true )
{
  m_db=db;
  QWidget *page = plainPage();

  Q3VBoxLayout *layout = new Q3VBoxLayout( page, marginHint(), spacingHint() );
  layout->setAutoAdd( true );

  QLabel *l = new QLabel( i18n( "Name:" ), page );
  l->setMaximumHeight( l->sizeHint().height() );
  names = new Q3ListBox( page );

  QMap< QString, QString >::ConstIterator it = db->getRecordEntries().begin();
  for ( ; it != db->getRecordEntries().end(); ++it )
    names->insertItem( m_db->version() ?it.data():it.key(), -1 );

  setInitialSize( QSize( 350, 400 ) );
  connect( names, SIGNAL( selectionChanged() ),
           this, SLOT( slotSelectionChanged() ) );
  connect( names, SIGNAL( doubleClicked( Q3ListBoxItem* ) ),
           this, SLOT( slotOk() ) );

  setFocus();
  enableButtonOK( names->currentItem() != -1 );
}

void KWMailMergeVariableInsertDia::slotSelectionChanged()
{
  enableButtonOK( names->currentItem() != -1 );
}

QString KWMailMergeVariableInsertDia::getName() const
{
     if (m_db->version()>=1) {
           QString description=names->text(names->currentItem());
           QMap< QString, QString >::ConstIterator it = m_db->getRecordEntries().begin();
           for ( ; it != m_db->getRecordEntries().end(); ++it )
                   if (description==it.data()) {
                           return it.key();
                   }
       Q_ASSERT(0);
         return ""; // can't happen
     }
       else
           return names->text( names->currentItem() );
}
