//THIS IS GOING TO GET AN LGPL HEADER (jowenn)

#ifndef KEXI_DATASOURCE_COMBO_H_
#define KEXI_DATASOURCE_COMBO_H_

#include <kcombobox.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qpixmap.h>

class KexiProject;

class KexiDataSourceComboBox: public KComboBox {
	Q_OBJECT
public:
	class Item {
	public:
		Item(){}
		Item(const QString &name_, const QString &globID_,
			const QString &mime_, const QString &identifier_,const QPixmap& pixm) {

			name=name_;
			globalIdentifier=globID_;
			mime=mime_;
			identifier=identifier_;
			pixmap=pixm;
		}
		QString name;
		QString globalIdentifier;
		QString mime;
		QString identifier;
		QPixmap pixmap;
	};
	typedef QValueList<Item> ItemList;
	KexiDataSourceComboBox(QWidget *parent, const char* name, KexiProject *proj);
	KexiDataSourceComboBox(QWidget *parent, const char* name, const ItemList &list);
	virtual ~KexiDataSourceComboBox();
	QString globalIdentifier();
	QString mime();
	QString identifier();
	static void fillList(KexiProject*,ItemList&);
private:
	ItemList m_list;
	KexiProject *m_project;
	void init();
};

#endif