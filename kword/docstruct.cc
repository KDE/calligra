/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Document structure                                     */
/******************************************************************/

#include "kword_doc.h"
#include "kword_view.h"
#include "kword_page.h"
#include "parag.h"
#include "frame.h"
#include "docstruct.h"
#include "docstruct.moc"

#include <klocale.h>
#include <kiconloader.h>

#include <qframe.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qintdict.h>

/******************************************************************/
/* Class: KWDocStructParagItem                                    */
/******************************************************************/

/*================================================================*/
KWDocStructParagItem::KWDocStructParagItem(QListViewItem *_parent,QString _text,KWParag *_parag,KWordGUI*__parent)
	: QListViewItem(_parent,_text)
{
	parag = _parag;
	gui = __parent;
}

/*================================================================*/
KWDocStructParagItem::KWDocStructParagItem(QListViewItem *_parent,QListViewItem *_after,QString _text,KWParag *_parag,KWordGUI*__parent)
	: QListViewItem(_parent,_after,_text)
{
	parag = _parag;
	gui = __parent;
}

/*================================================================*/
void KWDocStructParagItem::slotDoubleClicked(QListViewItem *_item)
{
	if (_item == this)
		gui->getPaperWidget()->scrollToParag(parag);
}

/******************************************************************/
/* Class: KWDocStructFrameItem                                    */
/******************************************************************/

/*================================================================*/
KWDocStructFrameItem::KWDocStructFrameItem(QListViewItem *_parent,QString _text,KWFrameSet *_frameset,KWFrame *_frame,KWordGUI*__parent)
	: QListViewItem(_parent,_text)
{
	frame = _frame;
	frameset = _frameset;
	gui = __parent;
}

/*================================================================*/
void KWDocStructFrameItem::slotDoubleClicked(QListViewItem *_item)
{
	if (_item == this)
		gui->getPaperWidget()->scrollToOffset(frame->x(),frame->y(),*gui->getPaperWidget()->getCursor());
}

/******************************************************************/
/* Class: KWDocStructTableItem                                    */
/******************************************************************/

/*================================================================*/
KWDocStructTableItem::KWDocStructTableItem(QListViewItem *_parent,QString _text,KWGroupManager *_table,KWordGUI*__parent)
	: QListViewItem(_parent,_text)
{
	table = _table;
	gui = __parent;
}

/*================================================================*/
void KWDocStructTableItem::slotDoubleClicked(QListViewItem *_item)
{
	if (_item == this)
    {
		KWFrame *frame = table->getFrameSet(0,0)->getFrame(0);
		gui->getPaperWidget()->scrollToOffset(frame->x(),frame->y(),*gui->getPaperWidget()->getCursor());
    }
}

/******************************************************************/
/* Class: KWDocStructPictureItem                                  */
/******************************************************************/

/*================================================================*/
KWDocStructPictureItem::KWDocStructPictureItem(QListViewItem *_parent,QString _text,KWPictureFrameSet *_pic,KWordGUI*__parent)
	: QListViewItem(_parent,_text)
{
	pic = _pic;
	gui = __parent;
}

/*================================================================*/
void KWDocStructPictureItem::slotDoubleClicked(QListViewItem *_item)
{
	if (_item == this)
    {
		KWFrame *frame = pic->getFrame(0);
		gui->getPaperWidget()->scrollToOffset(frame->x(),frame->y(),*gui->getPaperWidget()->getCursor());
    }
}

/******************************************************************/
/* Class: KWDocStructPartItem                                     */
/******************************************************************/

/*================================================================*/
KWDocStructPartItem::KWDocStructPartItem(QListViewItem *_parent,QString _text,KWPartFrameSet *_part,KWordGUI*__parent)
	: QListViewItem(_parent,_text)
{
	part = _part;
	gui = __parent;
}

/*================================================================*/
void KWDocStructPartItem::slotDoubleClicked(QListViewItem *_item)
{
	if (_item == this)
    {
		KWFrame *frame = part->getFrame(0);
		gui->getPaperWidget()->scrollToOffset(frame->x(),frame->y(),*gui->getPaperWidget()->getCursor());
    }
}

/******************************************************************/
/* Class: KWDocStructRootItem                                     */
/******************************************************************/

/*================================================================*/
KWDocStructRootItem::KWDocStructRootItem(QListView *_parent,KWordDocument *_doc,Type _type,KWordGUI*__parent)
	: QListViewItem(_parent)
{
	doc = _doc;
	type = _type;
	gui = __parent;

	switch (type)
    {
    case Arrangement:
	{
		setText(0,i18n("Arrangement"));
		setPixmap(0,ICON("tree_arrange.xpm"));
	} break;
    case TextFrames:
	{
		setText(0,i18n("Text Frames/Frame Sets"));
		setPixmap(0,ICON("tree_textframes.xpm"));
	} break;
    case Tables:
	{
		setText(0,i18n("Tables"));
		setPixmap(0,ICON("tree_table.xpm"));
	} break;
    case Pictures:
	{
		setText(0,i18n("Pictures"));
		setPixmap(0,ICON("tree_picture.xpm"));
	} break;
    case Cliparts:
	{
		setText(0,i18n("Cliparts"));
		setPixmap(0,ICON("tree_clipart.xpm"));
	} break;
    case Embedded:
	{
		setText(0,i18n("Embedded Objects"));
		setPixmap(0,ICON("tree_embedded.xpm"));
	} break;
    }
}

/*================================================================*/
void KWDocStructRootItem::setOpen(bool o)
{
	if (o)
    {
		switch (type)
		{
		case Arrangement:
			setupArrangement();
			break;
		case TextFrames:
			setupTextFrames();
			break;
		case Tables:
			setupTables();
			break;
		case Pictures:
			setupPictures();
			break;
		case Cliparts:
			setupCliparts();
			break;
		case Embedded:
			setupEmbedded();
			break;
		}
    }
	QListViewItem::setOpen(o);
}

/*================================================================*/
void KWDocStructRootItem::setupArrangement()
{
	if (childCount() > 0)
    {
		QListViewItem *child = firstChild(),*delChild;

		while(child)
		{
			delChild = child;
			child = child->nextSibling();
			delete delChild;
		}
    }

	QIntDict<KWDocStructParagItem> parags;
	parags.setAutoDelete(false);

	KWFrameSet *frameset = 0L;
	KWParag *parag = 0L;
	KWParagLayout *pLayout = 0L;
	QListViewItem *item = 0L;
	QString _name;

	int j = 0;
	for (int i = doc->getNumFrameSets() - 1;i >= 0;i--)
    {
		frameset = doc->getFrameSet(i);
		if (frameset->getFrameType() == FT_TEXT && frameset->getFrameInfo() == FI_BODY && !frameset->getGroupManager())
		{
			_name.sprintf(i18n("Frameset %d"),++j);
			item = new QListViewItem(this,_name);

			parag = dynamic_cast<KWTextFrameSet*>(frameset)->getFirstParag();
			while (parag)
			{
				pLayout = parag->getParagLayout();
				if (pLayout->getCounterType() != KWParagLayout::CT_NONE && pLayout->getNumberingType() == KWParagLayout::NT_CHAPTER)
				{
					int _depth = pLayout->getCounterDepth();
					if (_depth == 0)
					{
						if (item->childCount() == 0)
							parags.replace(_depth,new KWDocStructParagItem(item,
																		   QString(parag->getCounterText() + "  " +
																				   parag->getKWString()->toString(0,parag->getKWString()->size())),
																		   parag,gui));
						else
							parags.replace(_depth,new KWDocStructParagItem(item,parags[_depth],
																		   QString(parag->getCounterText() + "  " +
																				   parag->getKWString()->toString(0,parag->getKWString()->size())),
																		   parag,gui));
					}
					else
					{
						if (parags[_depth - 1]->childCount() == 0)
							parags.replace(_depth,new KWDocStructParagItem(parags[_depth - 1],
																		   QString(parag->getCounterText() + "  " +
																				   parag->getKWString()->toString(0,parag->getKWString()->size())),
																		   parag,gui));
						else
							parags.replace(_depth,new KWDocStructParagItem(parags[_depth - 1],parags[_depth],
																		   QString(parag->getCounterText() + "  " +
																				   parag->getKWString()->toString(0,parag->getKWString()->size())),
																		   parag,gui));
					}
					QObject::connect(listView(),SIGNAL(doubleClicked(QListViewItem*)),parags[_depth],SLOT(slotDoubleClicked(QListViewItem*)));
				}
				parag = parag->getNext();
			}
		}
    }

	if (childCount() == 0)
		(void)new QListViewItem(this,i18n("Empty"));
}

/*================================================================*/
void KWDocStructRootItem::setupTextFrames()
{
	if (childCount() > 0)
    {
		QListViewItem *child = firstChild(),*delChild;

		while(child)
		{
			delChild = child;
			child = child->nextSibling();
			delete delChild;
		}
    }

	KWFrameSet *frameset = 0L;
	QListViewItem *item = 0L;
	QString _name;
	KWDocStructFrameItem *child;

	int k = 0;
	for (int i = doc->getNumFrameSets() - 1;i >= 0;i--)
    {
		frameset = doc->getFrameSet(i);
		if (frameset->getFrameType() == FT_TEXT && frameset->getFrameInfo() == FI_BODY && !frameset->getGroupManager())
		{
			_name.sprintf(i18n("Frameset %d"),++k);
			item = new QListViewItem(this,_name);
	
			for (int j = frameset->getNumFrames() - 1;j >= 0;j--)
			{
				if (i == 0 && doc->getProcessingType() == KWordDocument::WP)
				{
					if (doc->getColumns() == 1)
						_name.sprintf(i18n("Page %d"),j + 1);
					else
						_name.sprintf(i18n("Column %d"),j + 1);
				}
				else
					_name.sprintf(i18n("Text Frame %d"),j + 1);
				child = new KWDocStructFrameItem(item,_name,frameset,frameset->getFrame(j),gui);
				QObject::connect(listView(),SIGNAL(doubleClicked(QListViewItem*)),child,SLOT(slotDoubleClicked(QListViewItem*)));
			}
		}
    }

	if (childCount() == 0)
		(void)new QListViewItem(this,i18n("Empty"));
}

/*================================================================*/
void KWDocStructRootItem::setupTables()
{
	if (childCount() > 0)
    {
		QListViewItem *child = firstChild(),*delChild;

		while(child)
		{
			delChild = child;
			child = child->nextSibling();
			delete delChild;
		}
    }

	QString _name;
	KWDocStructTableItem *child;

	for (int i = doc->getNumGroupManagers() - 1;i >= 0;i--)
    {
		if (!doc->getGroupManager(i)->isActive()) continue;

		_name.sprintf(i18n("Table %d"),i + 1);
		child = new KWDocStructTableItem(this,_name,doc->getGroupManager(i),gui);
		QObject::connect(listView(),SIGNAL(doubleClicked(QListViewItem*)),child,SLOT(slotDoubleClicked(QListViewItem*)));
    }

	if (childCount() == 0)
		(void)new QListViewItem(this,i18n("Empty"));
}

/*================================================================*/
void KWDocStructRootItem::setupPictures()
{
	if (childCount() > 0)
    {
		QListViewItem *child = firstChild(),*delChild;

		while(child)
		{
			delChild = child;
			child = child->nextSibling();
			delete delChild;
		}
    }

	KWFrameSet *frameset = 0L;
	QString _name;
	KWDocStructPictureItem *child;

	int j = 0;
	for (int i = doc->getNumFrameSets() - 1;i >= 0;i--)
    {
		frameset = doc->getFrameSet(i);
		if (frameset->getFrameType() == FT_PICTURE)
		{
			_name.sprintf(i18n("Picture (%s) %d"),dynamic_cast<KWPictureFrameSet*>(frameset)->getFileName().data(),++j);
			child = new KWDocStructPictureItem(this,_name,dynamic_cast<KWPictureFrameSet*>(frameset),gui);
			QObject::connect(listView(),SIGNAL(doubleClicked(QListViewItem*)),child,SLOT(slotDoubleClicked(QListViewItem*)));
		}
    }

	if (childCount() == 0)
		(void)new QListViewItem(this,i18n("Empty"));
}

/*================================================================*/
void KWDocStructRootItem::setupCliparts()
{
}

/*================================================================*/
void KWDocStructRootItem::setupEmbedded()
{
	if (childCount() > 0)
    {
		QListViewItem *child = firstChild(),*delChild;

		while(child)
		{
			delChild = child;
			child = child->nextSibling();
			delete delChild;
		}
    }

	KWFrameSet *frameset = 0L;
	QString _name;
	KWDocStructPartItem *child;

	int j = 0;
	for (int i = doc->getNumFrameSets() - 1;i >= 0;i--)
    {
		frameset = doc->getFrameSet(i);
		if (frameset->getFrameType() == FT_PART)
		{
			_name.sprintf(i18n("Embedded Object %d"),++j);
			child = new KWDocStructPartItem(this,_name,dynamic_cast<KWPartFrameSet*>(frameset),gui);
			QObject::connect(listView(),SIGNAL(doubleClicked(QListViewItem*)),child,SLOT(slotDoubleClicked(QListViewItem*)));
		}
    }

	if (childCount() == 0)
		(void)new QListViewItem(this,i18n("Empty"));
}

/******************************************************************/
/* Class: KWDocStructTree                                         */
/******************************************************************/

/*================================================================*/
KWDocStructTree::KWDocStructTree(QWidget *_parent,KWordDocument *_doc,KWordGUI*__parent)
	: QListView(_parent)
{
	doc = _doc;
	gui = __parent;

	addColumn(i18n("Document Structure"));
	//addColumn(i18n("Additional Info"));
	//setColumnWidthMode(0,Manual);
	//setColumnWidthMode(1,Manual);
}

/*================================================================*/
void KWDocStructTree::setup()
{
	setRootIsDecorated(true);
	setSorting(-1);

	embedded = new KWDocStructRootItem(this,doc,KWDocStructRootItem::Embedded,gui);
	QListViewItem *item = new QListViewItem(embedded,"Empty");

	cliparts = new KWDocStructRootItem(this,doc,KWDocStructRootItem::Cliparts,gui);
	item = new QListViewItem(cliparts,"Empty");

	pictures = new KWDocStructRootItem(this,doc,KWDocStructRootItem::Pictures,gui);
	item = new QListViewItem(pictures,"Empty");

	tables = new KWDocStructRootItem(this,doc,KWDocStructRootItem::Tables,gui);
	item = new QListViewItem(tables,"Empty");

	textfrms = new KWDocStructRootItem(this,doc,KWDocStructRootItem::TextFrames,gui);
	item = new QListViewItem(textfrms,"Empty");

	arrangement = new KWDocStructRootItem(this,doc,KWDocStructRootItem::Arrangement,gui);
	item = new QListViewItem(arrangement,i18n("Empty"));
}

/******************************************************************/
/* Class: KWDocStruct                                             */
/******************************************************************/

/*================================================================*/
KWDocStruct::KWDocStruct(QWidget *_parent,KWordDocument *_doc,KWordGUI*__parent)
	: QWidget(_parent)
{
	doc = _doc;
	parent = __parent;

	layout = new QGridLayout(this,1,1,0,0);

	tree = new KWDocStructTree(this,doc,__parent);
	tree->resize(tree->sizeHint());
	layout->addWidget(tree,0,0);
	layout->addColSpacing(0,0);
	layout->addRowSpacing(0,tree->width());
	layout->setColStretch(0,1);
	layout->setRowStretch(0,1);
	tree->setup();

	layout->activate();
}



