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

  infoWidget = new QWidget(this,"");

  panner->activate(treelist,infoWidget);

  resize(600,500);
  panner->move(0,0);
  panner->resize(600,500);
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

  QString pixdir = kapp->kde_toolbardir();
  QString page_name,obj_name;
  int pgnum;

  for (unsigned int i = 0;i < doc->backgroundList()->count();i++)
    {
      page_name.sprintf("%d. Page",i + 1);
      item = new KTreeListItem(page_name.data(),new QPixmap(pixdir + "/filenew.xpm"));
      treelist->insertItem(item,-1,false);
      pageList.append(item);
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
	  treelist->addChildItem(item,treelist->itemIndex(pageList.at(pgnum)));;
	  objList.append(item);
	}
    }
}


























