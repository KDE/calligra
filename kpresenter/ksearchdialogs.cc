/******************************************************************/
/* KSearchDialogs - (c) by Reginald Stadlbauer 1998               */
/* Version: 0.0.4                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* KCharSelect is under GNU GPL                                   */
/******************************************************************/
/* Module: Search Dialogs                                         */
/******************************************************************/

#include "ksearchdialogs.h"
#include "ksearchdialogs.moc"

#include <qpushbt.h>
#include <qchkbox.h>
#include <qlined.h>
#include <qgrpbox.h>
#include <qlabel.h>

#include <kapp.h>
#include <klocale.h>

/******************************************************************/
/* Class: KSearchDialog                                           */
/******************************************************************/

KSearchDialog::KSearchDialog( QWidget *parent=0, const char *name=0 )
    : QDialog( parent, name, true )
{
    setCaption( i18n( "Find" ) );

    frame = new QGroupBox( i18n( "Find" ), this, "frame" );

    value = new QLineEdit( frame, "value" );
    value->setFocus();
    connect( value, SIGNAL( returnPressed() ), this, SLOT( search() ) );

    sensitive = new QCheckBox( i18n( "Case Sensitive" ), frame, "case" );
    direction = new QCheckBox( i18n( "Find Backwards" ), frame, "direction" );

    ok = new QPushButton( i18n( "Find" ), this, "find" );
    connect( ok, SIGNAL( clicked() ), this, SLOT( search() ) );

    cancel = new QPushButton( i18n( "Done" ), this, "cancel" );
    connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    value->resize( 400, value->sizeHint().height() );
    value->move( 15, 20 );
    frame->move( 10, 10 );
    sensitive->move( value->x(), value->y() + value->height() + 10 );
    sensitive->resize( sensitive->sizeHint() );
    direction->resize( direction->sizeHint() );
    direction->move( value->x() + value->width() - direction->width(), sensitive->y() );
    frame->resize( direction->x() + direction->width() + 15, direction->y() + direction->height() + 15 );
    cancel->resize( cancel->sizeHint() );
    ok->resize( ok->sizeHint() );
    cancel->move( frame->x() + frame->width() - cancel->width(), frame->y() + frame->height() + 15 );
    ok->move( cancel->x() - cancel->width() - 5, cancel->y() );

    resize( cancel->x() + cancel->width() + 10, cancel->y() + cancel->height() + 10 );
}

/******************************************************************/
/* Class: KSearchReplaceDialog                                    */
/******************************************************************/

KSearchReplaceDialog::KSearchReplaceDialog( QWidget *parent=0, const char *name=0 )
    : QDialog( parent, name, true )
{
    setCaption( i18n( "Find & Replace" ) );

    frame = new QGroupBox( i18n( "Find" ), this, "frame" );

    search = new QLineEdit( frame, "search" );
    search->setFocus();
    connect( search, SIGNAL( returnPressed() ), this, SLOT( gotoReplace() ) );

    replace = new QLineEdit( frame, "replace" );
    connect( replace, SIGNAL( returnPressed() ), this, SLOT( searchReplace() ) );

    label = new QLabel( "Replace:", frame );

    sensitive = new QCheckBox( i18n( "Case Sensitive" ), frame, "case" );
    direction = new QCheckBox( i18n( "Find Backwards" ), frame, "direction" );

    replace_all = new QPushButton( i18n( "Replace all" ), this, "find" );
    connect( replace_all, SIGNAL( clicked() ), this, SLOT( searchReplaceAll() ) );

    ok = new QPushButton( i18n( "Replace" ), this, "find" );
    connect( ok, SIGNAL( clicked() ), this, SLOT( searchReplace() ) );

    cancel = new QPushButton( i18n( "Done" ), this, "cancel" );
    connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    search->resize( 400, search->sizeHint().height() );
    search->move( 15, 20 );
    label->resize( label->sizeHint() );
    label->move( search->x(), search->y() + search->height() + 10 );
    replace->resize( 400, replace->sizeHint().height() );
    replace->move( search->x(), label->y() + label->height() + 5 );
    frame->move( 10, 10 );
    sensitive->move( replace->x(), replace->y() + replace->height() + 10 );
    sensitive->resize( sensitive->sizeHint() );
    direction->resize( direction->sizeHint() );
    direction->move( replace->x() + replace->width() - direction->width(), sensitive->y() );
    frame->resize( direction->x() + direction->width() + 15, direction->y() + direction->height() + 15 );
    cancel->resize( cancel->sizeHint() );
    ok->resize( ok->sizeHint() );
    replace_all->resize( ok->sizeHint() );
    cancel->move( frame->x() + frame->width() - cancel->width(), frame->y() + frame->height() + 15 );
    ok->move( cancel->x() - ok->width() - 5, cancel->y() );
    replace_all->move( ok->x() - replace_all->width() - 5, cancel->y() );

    resize( cancel->x() + cancel->width() + 10, cancel->y() + cancel->height() + 10 );
}

