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

#include "presstructview.h"
#include "presstructview.moc"

/******************************************************************
 *
 * Class: KPPresStructTreeView
 *
 ******************************************************************/

KPPresStructTreeView::KPPresStructTreeView( QWidget *parent, const char *name )
    : QListView( parent, name )
{
}
    
/******************************************************************
 *
 * Class: KPPresStructTableView
 *
 ******************************************************************/
KPPresStructTableView::KPPresStructTableView( QWidget *parent, const char *name )
    : QListView( parent, name )
{
}

/******************************************************************
 *
 * Class: KPPresStructView
 *
 ******************************************************************/

KPPresStructView::KPPresStructView( QWidget *parent, const char *name, 
                                    KPresenterDoc *_doc, KPresenterView *_view )
    : QDialog( parent, name, FALSE ), doc( _doc ), view( _view )
{
}
