/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
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
/* Module: Dialog for skipping to a page during a presentation    */
/******************************************************************/

#include "gotopage.h"
#include "gotopage.moc"

#include <qlabel.h>
#include <qspinbox.h>
#include <qvalidator.h>
#include <qevent.h>

#include <kapp.h>
#include <klocale.h>

/******************************************************************/
/* class KPGotoPage                                               */
/******************************************************************/

/*================================================================*/
KPGotoPage::KPGotoPage(int minValue,int maxValue,int start,
                       QWidget *parent, const char *name, WFlags f )
    : QFrame( parent, name, f ), _default( start ), page( start )
{
    label = new QLabel( i18n( "Goto Page:" ), this );
    label->resize( label->sizeHint() );

    spinbox = new QSpinBox(minValue,maxValue,1,this);
    spinbox->setValue( page );
    spinbox->resize( spinbox->sizeHint() );

    spinbox->installEventFilter( this );
    label->installEventFilter( this );

    setFrameStyle( QFrame::WinPanel | QFrame:: Raised );

    spinbox->grabMouse();
    spinbox->grabKeyboard();
    setFocusProxy( spinbox );
    setFocusPolicy( QWidget::StrongFocus );
    spinbox->setFocus();

    resize( spinbox->width() + label->width() + 16,
            QMAX( spinbox->height(), label->height() ) + 10 );

    move( ( kapp->desktop()->width() - width() ) / 2,
          ( kapp->desktop()->height() - height() ) / 2 );

    show();
}

/*================================================================*/
int KPGotoPage::gotoPage(int minValue,int maxValue,int start,QWidget *parent)
{
    KPGotoPage dia(minValue,maxValue,start,parent,0L,
                   Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WStyle_Tool | Qt::WType_Popup );

    kapp->enter_loop();

    int page = dia.getPage();

    return page;
}

/*================================================================*/
bool KPGotoPage::eventFilter( QObject *obj, QEvent *e )
{
#undef KeyPress

    if ( e->type() == QEvent::KeyPress )
    {
        QKeyEvent *ke = dynamic_cast<QKeyEvent*>( e );
        if ( ke->key() == Key_Enter || ke->key() == Key_Return )
        {
            page = spinbox->value();
    
            spinbox->releaseMouse();
            spinbox->releaseKeyboard();
    
            kapp->exit_loop();
            hide();
    
            return true;
        }
        else if ( ke->key() == Key_Escape )
        {
            page = _default;

            spinbox->releaseMouse();
            spinbox->releaseKeyboard();

            kapp->exit_loop();
            hide();
    
            return true;
        }
        else if ( ke->key() == Key_Up )
        {
            spinbox->stepUp();
            return true;
        }
        else if ( ke->key() == Key_Down )
        {
            spinbox->stepDown();
            return true;
        }
    }
    return false;
}

/*================================================================*/
void KPGotoPage::resizeEvent( QResizeEvent *e )
{
    spinbox->resize( spinbox->sizeHint() );
    label->resize( label->sizeHint() );
    label->resize( label->width(), QMAX( label->height(), spinbox->height() ) );

    label->move( 5, 5 );
    spinbox->move( label->x() + label->width() + 5, 5 );
}

