#ifndef KEXIBROWSERITEM_H
#define KEXIBROWSERITEM_H

#include <klistview.h>
#include <qstring.h>

class KexiBrowserItem : public KListViewItem
{
//	Q_OBJECT
	
	public:
			
		enum ListType
		{
			Parent,
			Child
		};

		enum Content
		{
			Form,
			Query,
			Report,
			Table
		};

		
		KexiBrowserItem(ListType type, Content content, KListView *parent, QString caption);
		KexiBrowserItem(ListType type, Content content, KListViewItem *parentItem, QString capiton);
		KexiBrowserItem(ListType type, Content content, KListViewItem *parentItem, QString capiton, QString identifier);	

		~KexiBrowserItem() {};

		ListType	type() { return m_type; } ;
		Content		content() { return m_content; };
		QString		identifier() {return text(1);};		
	protected:

		ListType	m_type;
		Content		m_content;
};

#endif
