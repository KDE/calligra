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

#ifndef webpresentation_h
#define webpresentation_h

#include <qstring.h>
#include <qstringlist.h>
#include <qcolor.h>
#include <qwizard.h>

class KPresenterDoc;
class KPresenterView;
class Page;

/******************************************************************/
/* Class: KPWebPresentation                                       */
/******************************************************************/

class KPWebPresentation
{
public:
  enum ImageFormat {BMP = 0,PNG};
  
  KPWebPresentation(KPresenterDoc *_doc,KPresenterView *_view);
  KPWebPresentation(const QString &_config,KPresenterDoc *_doc,KPresenterView *_view);
  
  void setAuthor(const QString &_author)
  { author = _author; }
  void setTitle(const QString &_title)
  { title = _title; }
  void setSlideTitles(const QStringList &_slideTitles)
  { slideTitles = _slideTitles; }
  void setBackColor(const QColor &_backColor)
  { backColor = _backColor; }
  void setTitleColor(const QColor &_titleColor)
  { titleColor = _titleColor; }
  void setTextColor(const QColor &_textColor)
  { textColor = _textColor; }
  void setImageFormat(ImageFormat _imgFormat)
  { imgFormat = _imgFormat; }
  void setPath(const QString &_path)
  { path = _path; }
  void setZoom(int _zoom)
  { zoom = _zoom; }
  
  QString getAuthor()
  { return author; }
  QString getTitle()
  { return title; }
  QStringList &getSlideTitles()
  { return slideTitles; }
  QColor getBackColor()
  { return backColor; }
  QColor getTitleColor()
  { return titleColor; }
  QColor getTextColor()
  { return textColor; }
  ImageFormat getImageFormat()
  { return imgFormat; }
  QString getPath()
  { return path; }
  int getZoom()
  { return zoom; }
  
  void writeSlideImage(const QString &filename,int slide);

  void setConfig(const QString &_config)
  { config = _config; }
  QString getConfig()
  { return config; }
  
  void loadConfig();
  void saveConfig();
  
protected:
  void init();
  void detectAuthor();
		
  KPresenterDoc *doc;
  KPresenterView *view;
  QString config;
  
  QString author,title;
  QStringList slideTitles;
  QColor backColor,titleColor,textColor;
  QString path;
  ImageFormat imgFormat;
  int zoom;
  
};

/******************************************************************/
/* Class: KPWebPresentationWizard                                 */
/******************************************************************/

class KPWebPresentationWizard : public QWizard
{
  Q_OBJECT
  
public:
  KPWebPresentationWizard(const QString &_config,KPresenterDoc *_doc,KPresenterView *_view);
  
  static void createWebPresentation(const QString &_config,KPresenterDoc *_doc,KPresenterView *_view);

protected:
  QString config;
  KPresenterDoc *doc;
  KPresenterView *view;
  KPWebPresentation webPres;
  
protected slots:
  virtual void finish();
  
};

#endif
