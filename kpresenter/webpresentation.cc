/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1999              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: KPWebPresentation                                      */
/******************************************************************/

#include "webpresentation.h"
#include "webpresentation.moc"

#include "kpresenter_doc.h"
#include "kpresenter_view.h"
#include "page.h"

#include <stdlib.h>

#include <qfile.h>
#include <qtextstream.h>

#include <klocale.h>

/******************************************************************/
/* Class: KPWebPresentation                                       */
/******************************************************************/

/*================================================================*/
KPWebPresentation::KPWebPresentation(KPresenterDoc *_doc,KPresenterView *_view)
  : config(QString::null)
{
  doc = _doc;
  view = _view;
  init();
}

/*================================================================*/
KPWebPresentation::KPWebPresentation(const QString &_config,KPresenterDoc *_doc,KPresenterView *_view)
  : config(_config)
{
  doc = _doc;
  view = _view;
  init();
  loadConfig();
}

/*================================================================*/
void KPWebPresentation::writeSlideImage(const QString &filename,int slide)
{
}

/*================================================================*/
void KPWebPresentation::loadConfig()
{
}

/*================================================================*/
void KPWebPresentation::saveConfig()
{
}

/*================================================================*/
void KPWebPresentation::init()
{
  detectAuthor();
  title = "Slideshow";
  
  for (unsigned int i = 0;i < doc->getPageNums();i++)
    slideTitles.append(QString("Slide %1").arg(i));
  
  backColor = Qt::white;
  textColor = Qt::black;
  titleColor = Qt::red;
  imgFormat = PNG;
  
  path = getenv("HOME");
  path += "/kpresenter-webpresentation";

  zoom = 100;
}

/*================================================================*/
void KPWebPresentation::detectAuthor()
{
  QString user = getenv("USER");
  QString cmd = QString("grep %1 /etc/passwd | cut -d: -f5 > /tmp/kpname").arg(user);
  
  system(cmd);
  
  QFile f("/tmp/kpname");
  f.open(IO_ReadOnly);
  QTextStream t(&f);
  
  author = t.readLine();
  
  system("rm -rf /tmp/kpname");

  f.close();
}

/******************************************************************/
/* Class: KPWebPresentationWizard                                 */
/******************************************************************/

/*================================================================*/
KPWebPresentationWizard::KPWebPresentationWizard(const QString &_config,KPresenterDoc *_doc,KPresenterView *_view)
  : QWizard(_view,"",true), config(_config), webPres(config,_doc,_view)
{
  doc = _doc;
  view = _view;

  addPage(new QWidget(this),"");
}
  
/*================================================================*/
void KPWebPresentationWizard::createWebPresentation(const QString &_config,KPresenterDoc *_doc,KPresenterView *_view)
{
  KPWebPresentationWizard *dlg = new KPWebPresentationWizard(_config,_doc,_view);

  dlg->setCaption(i18n("Create Web-Presentation (HTML Slideshow)"));
  dlg->exec();
}

/*================================================================*/
void KPWebPresentationWizard::finish()
{
}
