/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Presentation Structure Viewer (header)                 */
/******************************************************************/

#ifndef presstructview_h
#define presstructview_h

#include <qdialog.h>
#include <qlistview.h>

class KPresenterDoc;
class KPresenterView;
class QSplitter;
class QResizeEvent;
class KPBackGround;
class KPObject;

/******************************************************************
 *
 * Class: KPPresStructObjectItem
 *
 ******************************************************************/

class KPPresStructObjectItem : public QListViewItem
{
public:
    KPPresStructObjectItem( QListView *parent );
    KPPresStructObjectItem( QListViewItem *parent );
    
    void setPage( KPBackGround *p );
    void setObject( KPObject *o );
    KPBackGround *getPage();
    KPObject *getObject();
    
protected:
    KPBackGround *page;
    KPObject *object;
    
};

/******************************************************************
 *
 * Class: KPPresStructView
 *
 ******************************************************************/

class KPPresStructView : public QDialog
{
    Q_OBJECT

public:
    KPPresStructView( QWidget *parent, const char *name,
                      KPresenterDoc *_doc, KPresenterView *_view );

protected:
    void setupSlideList();
    void resizeEvent( QResizeEvent *e );
    
    KPresenterDoc *doc;
    KPresenterView *view;
    QListView *slides;
    QSplitter *hsplit;
    
signals:
    void presStructViewClosed();

};

#endif
