/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Page Structure Viewer                                  */
/******************************************************************/

#include "kpresenter_doc.h"
#include "presstructview.h"
#include "presstructview.moc"

/******************************************************************/
/* Class: PresStructViewer                                        */
/******************************************************************/

/*================================================================*/
PresStructViewer::PresStructViewer(QWidget *parent,const char *name,KPresenterDocument_impl *_doc)
  : QDialog(parent,name,false)
{
  doc = _doc;
  pageList.setAutoDelete(true);
  objList.setAutoDelete(true);

  panner = new KNewPanner(this,"panner",KNewPanner::Vertical,KNewPanner::Percent,30);

  setupTreeView();

  list = new KTabListBox(panner,"",2);
  list->setTableFlags(list->tableFlags() | Tbl_smoothHScrolling | Tbl_smoothVScrolling);
  list->setColumn(0,i18n("Description"),200);
  list->setColumn(1,i18n("Value"),200);

  panner->activate(treelist,list);

  resize(600,400);
  panner->move(0,0);
  panner->resize(600,400);
}

/*================================================================*/
void PresStructViewer::itemSelected(int _index)
{
  KTreeListItem *item = treelist->itemAt(_index);
  QString text(item->getText());

  // must be a page
  if (text.contains("Page",false) > 0)
    {
      ItemInfo *info = 0;
      for (unsigned int i = 0;i < pageList.count();i++)
	{
	  info = pageList.at(i);
	  if (info->item == item) fillWithPageInfo(doc->backgroundList()->at(info->num),info->num);
	}
    }

  // must be an object
  else
    {
    }
}

/*================================================================*/
void PresStructViewer::resizeEvent(QResizeEvent *e)
{
  QDialog::resizeEvent(e);
  panner->resize(width(),height());
}

/*================================================================*/
void PresStructViewer::closeEvent(QCloseEvent *e)
{
  QDialog::closeEvent(e);
  emit presStructViewClosed();
}

/*================================================================*/
void PresStructViewer::setupTreeView()
{
  treelist = new KTreeList(this,"");

  KTreeListItem *item = 0;
  ItemInfo *info = 0;

  QString pixdir = kapp->kde_toolbardir();
  QString page_name,obj_name;
  int pgnum;

  for (unsigned int i = 0;i < doc->backgroundList()->count();i++)
    {
      page_name.sprintf("%d. Page",i + 1);
      item = new KTreeListItem(page_name.data(),new QPixmap(pixdir + "/filenew.xpm"));
      treelist->insertItem(item,-1,false);
      info = new ItemInfo;
      info->num = i;
      info->item = item;
      pageList.append(info);
    }

  pixdir = kapp->kde_datadir();
  KPObject *kpobject = 0;
  for (unsigned int i = 0;i < doc->objectList()->count();i++)
    {
      kpobject = doc->objectList()->at(i);
      obj_name.sprintf("%s (%d)",i18n(ObjName[static_cast<int>(kpobject->getType())]),i);
      item = new KTreeListItem(obj_name.data(),new QPixmap(pixdir + "/kpresenter/toolbar/dot.xpm"));
      pgnum = doc->getPageOfObj(i,0,0);
      if (pgnum != -1)
	{
	  pgnum--;
	  treelist->addChildItem(item,treelist->itemIndex(pageList.at(pgnum)->item));;
	  info = new ItemInfo;
	  info->num = i;
	  info->item = item;
	  objList.append(info);
	}
    }

  connect(treelist,SIGNAL(highlighted(int)),this,SLOT(itemSelected(int)));
}

/*================================================================*/
void PresStructViewer::fillWithPageInfo(KPBackGround *_page,int _num)
{
  QString str;
  int r,g,b;
  QColor c;

  list->setNumRows(0);
  str.sprintf("%d",_num + 1);
  list->appendItem(i18n("Number")); 
  list->changeItemPart(str,list->count() - 1,1);

  list->appendItem(i18n("Back Type")); 
  list->changeItemPart(i18n(BackTypeName[static_cast<int>(_page->getBackType())]),
		       list->count() - 1,1);

  list->appendItem(i18n("Back View")); 
  list->changeItemPart(i18n(BackViewName[static_cast<int>(_page->getBackView())]),
		       list->count() - 1,1);

  c = _page->getBackColor1();
  c.rgb(&r,&g,&b);
  str.sprintf("#%02X%02X%02X",r,g,b);
  list->appendItem(i18n("Color1")); 
  list->changeItemPart(str,list->count() - 1,1);

  c = _page->getBackColor2();
  c.rgb(&r,&g,&b);
  str.sprintf("#%02X%02X%02X",r,g,b);
  list->appendItem(i18n("Color2")); 
  list->changeItemPart(str,list->count() - 1,1);

  list->appendItem(i18n("Picture Filename")); 
  list->changeItemPart(_page->getBackPixFilename(),list->count() - 1,1);

  list->appendItem(i18n("Clipart Filename")); 
  list->changeItemPart(_page->getBackClipFilename(),list->count() - 1,1);

  list->appendItem(i18n("Effect for changing to next page")); 
  list->changeItemPart(i18n(PageEffectName[static_cast<int>(_page->getPageEffect())]),
		       list->count() - 1,1);
}
