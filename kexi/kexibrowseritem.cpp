#include "kexibrowseritem.h"

KexiBrowserItem::KexiBrowserItem(ListType type, Content content, KListView *parent, QString caption)
	: KListViewItem(parent, caption,"")
{
	m_type = type;
	m_content = content;
}

KexiBrowserItem::KexiBrowserItem(ListType type, Content content, KListViewItem *parentItem, QString caption)
	: KListViewItem(parentItem, caption,"")
{
	m_type = type;
	m_content = content;
}


KexiBrowserItem::KexiBrowserItem(ListType type, Content content, KListViewItem *parentItem, QString caption, QString identifier)
	: KListViewItem(parentItem, caption,identifier)
{
	m_type = type;
	m_content = content;
}

/*ListType KexiBrowserItem::type()
{
	return m_type;
}

Content KexiBrowserItem::content()
{
	return m_content;
}
*/

#include "kexibrowseritem.moc"

