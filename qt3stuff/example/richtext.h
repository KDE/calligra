/****************************************************************************
** $Id$
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef RICHTEXT_H
#define RICHTEXT_H

#include <qvbox.h>

namespace Qt3 { 
class QTextView;
}
class QPushButton;

class MyRichText : public QVBox
{
    Q_OBJECT

public:
    MyRichText( QWidget *parent = 0, const char *name = 0 );

protected:
    Qt3::QTextView *view;
    QPushButton *bClose, *bNext, *bPrev;
    int num;

protected slots:
    void prev();
    void next();

};

#endif
