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

#include "docstruct.h"
#include "docstruct.moc"

/******************************************************************/
/* Class: KWDocStructRootItem                                     */
/******************************************************************/

/*================================================================*/
KWDocStructRootItem::KWDocStructRootItem(QListView *_parent,KWordDocument *_doc,Type _type)
  : QListViewItem(_parent)
{
  doc = _doc;
  type = _type;

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

  QIntDict<QListViewItem> parags;
  parags.setAutoDelete(false);

  KWFrameSet *frameset = 0L;
  KWParag *parag = 0L;
  KWParagLayout *pLayout = 0L;
  QListViewItem *item = 0L;
  QString _name;
  for (int i = doc->getNumFrameSets() - 1;i >= 0;i--)
    {
      frameset = doc->getFrameSet(i);
      if (frameset->getFrameType() == FT_TEXT && frameset->getFrameInfo() == FI_BODY && !frameset->getGroupManager())
	{
	  _name.sprintf(i18n("Frameset %d"),i);
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
			parags.replace(_depth,new QListViewItem(item,QString(parag->getCounterText() + "  " +
									     parag->getKWString()->toString(0,parag->getKWString()->size()))));
		      else
			parags.replace(_depth,new QListViewItem(item,parags[_depth],
								QString(parag->getCounterText() + "  " +
									parag->getKWString()->toString(0,parag->getKWString()->size()))));
		    }
		  else
		    {
		      if (parags[_depth - 1]->childCount() == 0)
			parags.replace(_depth,new QListViewItem(parags[_depth - 1],
								QString(parag->getCounterText() + "  " +
									parag->getKWString()->toString(0,parag->getKWString()->size()))));
		      else
			parags.replace(_depth,new QListViewItem(parags[_depth - 1],parags[_depth],
								QString(parag->getCounterText() + "  " +
									parag->getKWString()->toString(0,parag->getKWString()->size()))));
		    }
		}
	      parag = parag->getNext();
	    }
	}
    } 
}

/*================================================================*/
void KWDocStructRootItem::setupTextFrames()
{
}

/*================================================================*/
void KWDocStructRootItem::setupTables()
{
}

/*================================================================*/
void KWDocStructRootItem::setupPictures()
{
}

/*================================================================*/
void KWDocStructRootItem::setupCliparts()
{
}

/*================================================================*/
void KWDocStructRootItem::setupEmbedded()
{
}

/******************************************************************/
/* Class: KWDocStructTree                                         */
/******************************************************************/

/*================================================================*/
KWDocStructTree::KWDocStructTree(QWidget *_parent,KWordDocument *_doc)
  : QListView(_parent)
{
  doc = _doc;
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

  embedded = new KWDocStructRootItem(this,doc,KWDocStructRootItem::Embedded);
  QListViewItem *item = new QListViewItem(embedded,"Test5"); 

  cliparts = new KWDocStructRootItem(this,doc,KWDocStructRootItem::Cliparts);
  item = new QListViewItem(cliparts,"Test4");

  pictures = new KWDocStructRootItem(this,doc,KWDocStructRootItem::Pictures);
  item = new QListViewItem(pictures,"Test3");

  tables = new KWDocStructRootItem(this,doc,KWDocStructRootItem::Tables);
  item = new QListViewItem(tables,"Test2");

  textfrms = new KWDocStructRootItem(this,doc,KWDocStructRootItem::TextFrames);
  item = new QListViewItem(textfrms,"Test1");

  arrangement = new KWDocStructRootItem(this,doc,KWDocStructRootItem::Arrangement);
  item = new QListViewItem(arrangement,i18n("Nothing"));
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

  tree = new KWDocStructTree(this,doc);
  tree->resize(tree->sizeHint());
  layout->addWidget(tree,0,0);
  layout->addColSpacing(0,0);
  layout->addRowSpacing(0,tree->width());
  layout->setColStretch(0,1);
  layout->setRowStretch(0,1);
  tree->setup();

  layout->activate();
}



