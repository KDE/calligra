/****************************************************************************
** $Id$
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "richtext.h"

#include <qhbox.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qtextview.h>
#include <qbrush.h>
#include <qapplication.h>

static const char* sayings[] = {
    "<b>Saying 1:</b><br>"
    "<hr><br><br>"
    "<big>Evil is that which one believes of others.  It is a sin to believe evil "
    "of others, but it is seldom a mistake.</big><br><br>"
    "<center><i>-- H.L. Mencken</i></center>",

    "<b>Saying 2:</b><br>"
    "<hr><br><br>"
    "<big>A well-used door needs no oil on its hinges.<br>"
    "A swift-flowing steam does not grow stagnant.<br>"
    "Neither sound nor thoughts can travel through a vacuum.<br>"
    "Software rots if not used.<br><br>"
    "These are great mysteries.</big><br><br>"
    "<center><i>-- Geoffrey James, \"The Tao of Programming\"</i></center>",

    "<b>Saying 3:</b><br>"
    "<hr><br><br>"
    "<big>Show business is just like high school, except you get paid.</big><br><br>"
    "<center><i>-- Martin Mull</i></center>",

    "<b>Saying 4:</b><br>"
    "<hr><br><br>"
    "<big><b>The Least Successful Executions</b><br>"
    "<twocolumn><p>      History has furnished us with two executioners worthy of attention. "
    "The first performed in Sydney in Australia.  In 1803 three attempts were "
    "made to hang a Mr. Joseph Samuels.  On the first two of these the rope "
    "snapped, while on the third Mr. Samuels just hung there peacefully until he "
    "and everyone else got bored.  Since he had proved unsusceptible to capital "
    "punishment, he was reprieved.</p>"
    "<p>        The most important British executioner was Mr. James Berry who "
    "tried three times in 1885 to hang Mr. John Lee at Exeter Jail, but on each "
    "occasion failed to get the trap door open.<!p>"
    "<p>        In recognition of this achievement, the Home Secretary commuted "
    "Lee's sentence to \"life\" imprisonment.  He was released in 1917, emigrated "
    "to America and lived until 1933.</p></twocolumn></big><br><br>"
    "<center><i>-- Stephen Pile, \"The Book of Heroic Failures\"</i></center>",

    "<b>Saying 5:</b><br>"
    "<hr><br><br>"
    "<big>If you can, help others.  If you can't, at least don't hurt others.</big><br><br>"
    "<center><i>-- the Dalai Lama</i></center>",

    "<b>Saying 6:</b><br>"
    "<hr><br><br>"
    "<big>Television has brought back murder into the home -- where it belongs.</big><br><br>"
    "<center><i>-- Alfred Hitchcock</i></center>",

    "<b>Saying 7:</b><br>"
    "<hr><br><br>"
    "<big>I don't know who my grandfather was; I am much more concerned to know "
    "what his grandson will be.</big><br><br>"
    "<center><i>-- Abraham Lincoln</i></center>",

    0
};


MyRichText::MyRichText( QWidget *parent, const char *name )
    : QVBox( parent, name )
{
    setMargin( 5 );

    view = new QTextView( this );
    view->setText( "This is a <b>Test</b> with <i>italic</i> <u>stuff</u>" );
    QBrush paper;
    paper.setPixmap( QPixmap( "marble.png" ) );
    view->setPaper( paper );

    view->setText( sayings[0] );
    view->setMinimumSize( 450, 250 );

    QHBox *buttons = new QHBox( this );
    buttons->setMargin( 5 );

    bClose = new QPushButton( "&Close", buttons );
    bPrev = new QPushButton( "<< &Prev", buttons );
    bNext = new QPushButton( "&Next >>", buttons );

    bPrev->setEnabled( FALSE );

    connect( bClose, SIGNAL( clicked() ), qApp, SLOT( quit() ) );
    connect( bPrev, SIGNAL( clicked() ), this, SLOT( prev() ) );
    connect( bNext, SIGNAL( clicked() ), this, SLOT( next() ) );

    num = 0;
}

void MyRichText::prev()
{
    if ( num <= 0 )
        return;

    num--;

    view->setText( sayings[num] );

    if ( num == 0 )
        bPrev->setEnabled( FALSE );

    bNext->setEnabled( TRUE );
}

void MyRichText::next()
{
    if ( !sayings[++num] )
        return;

    view->setText( sayings[num] );

    if ( !sayings[num + 1] )
        bNext->setEnabled( FALSE );

    bPrev->setEnabled( TRUE );
}





