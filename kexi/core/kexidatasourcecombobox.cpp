//THIS IS GOING TO GET AN LGPL HEADER (jowenn)

#include "kexidatasourcecombobox.h"
#include "kexidatasourcecombobox.moc"
#include <kexiproject.h>
#include <kexiprojecthandler.h>
#include <qptrlist.h>
#include <klocale.h>

KexiDataSourceComboBox::KexiDataSourceComboBox(QWidget *parent, const char* name,
	KexiProject *project) :KComboBox(parent,name),m_project(project) {

	fillList(project,m_list);
	init();
}

KexiDataSourceComboBox::KexiDataSourceComboBox(QWidget *parent, const char* name,
	const KexiDataSourceComboBox::ItemList &list) :KComboBox(parent,name),
	m_project(0) {

	m_list=list;
	init();
}

KexiDataSourceComboBox::~KexiDataSourceComboBox() {
}

void KexiDataSourceComboBox::init() {
	setEditable(false);
	clear();
	for (ItemList::const_iterator it=m_list.begin();it!=m_list.end();++it) {
		insertItem((*it).pixmap,(*it).name);
	}
}



void KexiDataSourceComboBox::fillList(KexiProject *proj,
	KexiDataSourceComboBox::ItemList &list) {

	list.clear();
	list.append(Item(i18n("<NONE>"),"","","",QPixmap()));
	ProviderList l=proj->providers("KexiDataProvider");
	for (KexiProjectHandler *h=l.first();h;h=l.next()) {
		QString sname=h->name()+"/%1";
		QString mime=h->mime();
		QPixmap pixmap=h->itemPixmap();
		for(KexiProjectHandler::ItemIterator it(*h->items()); it.current(); ++it)
		{
			list.append(Item(sname.arg((*it)->name()),
			(*it)->identifier(),mime,(*it)->shortIdentifier(),pixmap));
		}
	}

}

void KexiDataSourceComboBox::selectGlobalIdentifier(const QString& gid) {
//better with iterator and find
	for ( int i=0;i<m_list.count();i++) {
		if ((*m_list.at(i)).globalIdentifier==gid)
		{
			setCurrentItem(i);
			return;
		}
	}
	setCurrentItem(0);
}

QString KexiDataSourceComboBox::globalIdentifier() const {
	if (currentItem()<0) return QString();
	return (*m_list.at(currentItem())).globalIdentifier;
}

QString KexiDataSourceComboBox::mime() const {
	if (currentItem()<0) return QString();
	return (*m_list.at(currentItem())).mime;
}

QString KexiDataSourceComboBox::identifier() const{
	if (currentItem()<0) return QString();
	return (*m_list.at(currentItem())).identifier;
}
