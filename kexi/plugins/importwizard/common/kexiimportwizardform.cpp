#include "kexiimportwizard.h"
#include "kexiimportwizardform.h"
#include "kexiimportwizardform.moc"
#include <kexiDB/kexidbtable.h>
#include <kexiDB/kexidb.h>
#include <kexiDB/kexidbrecordset.h>
#include <kexiDB/kexidbupdaterecord.h>
#include <core/kexiproject.h>
#include <core/filters/kexifilter.h>
#include <core/filters/kexifiltermanager.h>
#include <core/filters/kexitableimportsourceiface.h>
#include <kdebug.h>
#include <qradiobutton.h>
#include <kcombobox.h>
#include <klineedit.h>
#include <qstringlist.h>
#include <qtable.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <qobjectlist.h>
#include <qpushbutton.h>
#include <kpushbutton.h>
#include <qlayout.h>
#include <qmap.h>

KexiTableImportForm::KexiTableImportForm(KexiFilterManager *filterManager,KexiImportWizard *wiz,
		const QString& dialogFilter, QMap<QString,QString> mimePluginMapping):
	KexiImportWizardFormBase(),m_filterManager(filterManager),m_dialogFilter(dialogFilter),
	m_mimePluginMapping(mimePluginMapping), m_filter(0),m_initializing(true) {
	
	QVBoxLayout *layout=(new QVBoxLayout(openFilePage));
	layout->setAutoAdd(false);
	m_openWidget=wiz->openWidget(openFilePage,m_dialogFilter);
	connect (m_openWidget,SIGNAL(nextPage()),this,SLOT(loadPlugin()));
	connect (m_openWidget,SIGNAL(filterHasBeenLoaded(KexiFilter*,const KURL& )),this,SLOT(filterHasBeenLoaded(KexiFilter*,const KURL&)));
	layout->addWidget(m_openWidget);
	layout->activate();
	
	initDestChoice();
        setFinishEnabled(finishPage,true);
	connect(this,SIGNAL(selected(const QString&)),this,SLOT(pageSelected(const QString&)));
	connect(destination_newTable,SIGNAL(toggled(bool)),this,SLOT(changeDestinationType(bool)));
	m_previousPage=openFilePage;
	m_recursiveOpen=false;
	m_wiz=wiz;
	setAppropriate(dummySourcePage,false);
	m_initializing=false;
}

KexiTableImportForm::~KexiTableImportForm() {
	kdDebug()<<"KexiTableImportForm:~KexiTableIImportForm"<<endl;
}


void KexiTableImportForm::loadPlugin() {
	kdDebug()<<"KexiTableImportForm::loadPlugin: before recursive check"<<endl;

	if (m_recursiveOpen) return;
	kdDebug()<<"KexiTableImportForm::loadPlugin: after recursive check"<<endl;

	m_recursiveOpen=true;

	disconnect (m_openWidget,SIGNAL(nextPage()),this,SLOT(loadPlugin()));

	kdDebug()<<"KexiTableImportForm::loadPlugin"<<endl;

	delete m_filter;
	m_filter=0;
	
	nextButton()->setEnabled(false);
	backButton()->setEnabled(false);
	m_wiz->openPageLeft(m_openWidget, m_mimePluginMapping);
}

void KexiTableImportForm::filterHasBeenLoaded(KexiFilter *filter,const KURL &url) {
	nextButton()->setEnabled(true);
	backButton()->setEnabled(true);

	m_filter=filter;
	if (!m_filter)  {
		showPage(openFilePage);
		m_recursiveOpen=false;
		connect (m_openWidget,SIGNAL(nextPage()),this,SLOT(loadPlugin()));
		return;
	}


	kdDebug()<<"KexiTableImportForm::loadPlugin: plugin has been loaded"<<endl;

	if (!filter->prepareImport(KexiFilterManager::Data,url)) {
		delete m_filter;
		m_filter=0;
		showPage(openFilePage);
		m_recursiveOpen=false;
		connect (m_openWidget,SIGNAL(nextPage()),this,SLOT(loadPlugin()));
		return;
		
	}

	m_previousPage=0;	
	KexiTableImportSourceIface *source=static_cast<KexiTableImportSourceIface*>(
		m_filter->qt_cast("KexiTableImportSourceIface"));
	QPtrList<QWidget> wl=source->tableSourceWidgets(this);
	for (QWidget *w=wl.first();w;w=wl.next()) {
		insertPage(w,w->caption(),indexOf(dummySourcePage));
	}

	kdDebug()<<"KexiTableImportForm::loadPlugin: Pages have been added"<<endl;

	showPage(wl.first());

	kdDebug()<<"KexiTableImportForm::loadPlugin: show first Plugin Page"<<endl;
	
	connect (m_openWidget,SIGNAL(nextPage()),this,SLOT(loadPlugin()));
	m_recursiveOpen=false;

	kdDebug()<<"KexiTableImportForm::loadPlugin: Leaving"<<endl;
}

void KexiTableImportForm::initDestChoice() {
	KexiDB *db=m_filterManager->project()->db();
	destination_newTable->setChecked(true);
	name_newTable->setText("New Table1");
	name_existingTable->setEnabled(false);
	name_existingTable->insertStringList(db->tableNames());
	setAppropriate(newTablePage,true);
	setAppropriate(existingTablePage,false);


}

void KexiTableImportForm::changeDestinationType(bool newTable) {
	if (newTable) {
		name_existingTable->setEnabled(false);
		name_newTable->setEnabled(true);
		setAppropriate(newTablePage,true);
		setAppropriate(existingTablePage,false);
	} else {
		name_existingTable->setEnabled(true);
		name_newTable->setEnabled(false);
		setAppropriate(newTablePage,false);
		setAppropriate(existingTablePage,true);
	}
}

void KexiTableImportForm::pageSelected(const QString &) {

	if (currentPage()==newTablePage) buildNewTablePage();
	if (currentPage()==existingTablePage) buildExistingTablePage();

	if (m_previousPage==openFilePage) {
		if (!((currentPage()==openFilePage) || m_initializing)) {
			loadPlugin();
		}
		m_previousPage=currentPage();
		return;
	}
	m_previousPage=currentPage();

	if (currentPage()==openFilePage) setAppropriate(loadingImportPluginPage,true);
	if ((currentPage()!=openFilePage) && (currentPage()!=loadingImportPluginPage))
		setAppropriate(loadingImportPluginPage,false);

}


void KexiTableImportForm::buildExistingTablePage() {
	KexiDB *db=m_filterManager->project()->db();
	KexiDBTable const *table=db->table(name_existingTable->currentText());
	if (!table) {/*this can't happen, but who knows*/ return; }

	KexiTableImportSourceIface *src=KEXITABLEIMPORTSOURCEIFACE(m_filter);
	if (!src) {
		//error
		return;
	}

	m_sourceTable=src->tableStructure();

	QStringList fieldList;
	QComboTableItem *cbti;

	fieldList<<i18n("<DEFAULT>")<<i18n("<NULL>");
        for (int i=0;i<m_sourceTable.fieldCount();i++) {
                fieldList<<m_sourceTable.field(i).name();
                m_destinationTable.addField(m_sourceTable.field(i));
        }

	existingTable_mapping->setNumRows(0);
	existingTable_mapping->setNumRows(table->fieldCount());
	int s=(fieldList.count()>2)?2:0;
        for (int i=0;i<table->fieldCount();i++) {

                existingTable_mapping->setText(i,0,table->field(i).name());
                existingTable_mapping->setItem(i,1,cbti=new 
		QComboTableItem(newTable_mapping,fieldList));
                cbti->setCurrentItem(*(fieldList.at(s)));
		if (s!=0) {
			s++;
			if (s>=fieldList.count()) s=0;
		}

        }


	
}

void KexiTableImportForm::buildNewTablePage() {
	newTable_fields->setNumRows(0);
	newTable_mapping->setNumRows(0);

	KexiTableImportSourceIface *src=KEXITABLEIMPORTSOURCEIFACE(m_filter);
	if (!src) {
		//error
		return;
	}

	m_sourceTable=src->tableStructure();

        QStringList fieldTypes;

        for(int i = 1; i < KexiDBField::SQLLastType; i++)
        {
                fieldTypes.append(KexiDBField::typeName(static_cast<KexiDBField::ColumnType>(i)));
        }

	
	int offset=0;
	m_idColumn=false;
	QComboTableItem *cbti;
	QCheckTableItem *cti;
	QStringList fieldList;

	m_destinationTable=KexiDBTable(name_newTable->text());

	kdDebug()<<"Source table has "<<m_sourceTable.primaryKeys().count()<<" primarykey(s)"<<endl;
	if (m_sourceTable.primaryKeys().count()==0) {
		newTable_fields->setNumRows(1);
		newTable_fields->setText(0,0,"ID");
		newTable_fields->setItem(0,1,cbti=new QComboTableItem(newTable_fields,fieldTypes));
		newTable_fields->setText(0,2,"");
		newTable_fields->setItem(0,3,cti=new QCheckTableItem(newTable_fields,""));
		cti->setChecked(true);
		newTable_fields->setText(0,4,"N");
		newTable_mapping->setNumRows(1);
		newTable_mapping->setText(0,0,"ID");

		offset=1;
		m_idColumn=true;
		m_destinationTable.addField(KexiDBField("ID",KexiDBField::SQLBigInt,
			KexiDBField::CCNotNull | KexiDBField::CCPrimaryKey | KexiDBField::CCAutoInc));
		m_destinationTable.addPrimaryKey("ID");
	}

	fieldList<<i18n("<DEFAULT>")<<i18n("<NULL>");
	for (int i=0;i<m_sourceTable.fieldCount();i++) {
		fieldList<<m_sourceTable.field(i).name();
		m_destinationTable.addField(m_sourceTable.field(i));
	}

	for (int i=0;i<m_sourceTable.fieldCount();i++) {
	
		newTable_fields->setNumRows(i+offset+1);
		
		newTable_fields->setText(i+offset,0,m_sourceTable.field(i).name());
		newTable_fields->setItem(i+offset,1,cbti=new QComboTableItem(newTable_fields,fieldTypes));
		cbti->setCurrentItem(m_sourceTable.field(i).sqlType()-1);
		newTable_fields->setText(i+offset,2,"");

		newTable_fields->setItem(i+offset,3,cti=new QCheckTableItem(newTable_fields,""));
		cti->setChecked(false);
		newTable_fields->setText(i+offset,4,"N");

		newTable_mapping->setNumRows(i+offset+1);
		newTable_mapping->setText(i+offset,0,m_sourceTable.field(i).name());
		newTable_mapping->setItem(i+offset,1,cbti=new QComboTableItem(newTable_mapping,fieldList));
		cbti->setCurrentItem(m_sourceTable.field(i).name());

	}


}

bool KexiTableImportForm::createTable() {
	int cnt=m_destinationTable.fieldCount();
	if (cnt==0) {
		KMessageBox::sorry(this,i18n("A table with no fields can't be created"));
		return false;
	}

	if (m_destinationTable.primaryKeys().count()==0) {
		KMessageBox::sorry(this,i18n("A table has to have a primary key"));
		return false;
	}

	kdDebug()<<"New table contains "<<m_destinationTable.fieldCount()<<" fields"<<endl;
	KexiDB *db=m_filterManager->project()->db();
	if (!db->createTable(m_destinationTable)) {
		KexiDBError *err;
		db->latestError(&err);
		if (err && (err->kexiErrnoFunction()!=0)) KMessageBox::sorry(this,err->message());
		else KMessageBox::sorry(this,i18n("An unknown error has been encountered while creating the destination table"));
		return false;
	}
			
	return true;
}


bool KexiTableImportForm::importValues(const QString& tableName,int mapLen, int mapping[]) {
	KexiTableImportSourceIface *src=KEXITABLEIMPORTSOURCEIFACE(m_filter);
	if (!src) {
		//error
		return;
	}

	KexiDB *db=m_filterManager->project()->db();
	KexiDBRecordSet *rs=db->queryRecord("select * from "+tableName);

#warning FIXME either create an db->recordSetForInsert(); or automatically close the recordset on writeout
	while (rs->next()); //Q&D Hack(tm)


	for (bool rowAvailable=src->firstTableRow();rowAvailable;
		rowAvailable=src->nextTableRow()) {
		KexiDBUpdateRecord *ur=rs->insert(false);
		for (int i=0;i<mapLen;i++) {
			if (mapping[i]>=0) {
				ur->setValue(i,src->tableValue(mapping[i]));
			}
		}
	}
	if (rs->writeOut()) kdDebug()<<"ERROR while inserting data values"<<endl; //ERRORHANDLING
	delete rs;
	return true;
}

void KexiTableImportForm::accept() {
	QString tableName;
	int *mapping;
	int mapLen;
	if (destination_newTable->isChecked()) {
		tableName=name_newTable->text();
		if (!createTable()) return;
		mapLen=newTable_mapping->numRows();
		mapping=new int[mapLen];
		for (int i=0;i<mapLen;i++) {
			if (i==0) mapping[0]=-3; else  {//temporary hack 
				mapping[i]=((QComboTableItem*)newTable_mapping->item(i,1))->currentItem()-2;
				kdDebug()<<"MAPPING: "<<mapping[i]<<endl;
			}
		}
	} else {
		tableName=name_existingTable->currentText();
		mapLen=existingTable_mapping->numRows();
		mapping=new int[mapLen];
		for (int i=0;i<mapLen;i++) {
			mapping[i]=((QComboTableItem*)existingTable_mapping->item(i,1))->currentItem()-2;
		}
		
	}

	bool ok=importValues(tableName,mapLen,mapping);

	delete mapping;
	if (!ok) return;

	QWizard::accept();
}

void KexiTableImportForm::setMode(unsigned long mode) {
	if ((mode == KexiFilterManager::Data) || (mode == KexiFilterManager::Data)) {
		setAppropriate(destinationTablePage,true);
		setAppropriate(newTablePage,true);
		setAppropriate(existingTablePage,true);
		setAppropriate(finishPage,true);
	}
}
