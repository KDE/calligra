/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <gotopage.h>

#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qpushbutton.h>

#include <klocale.h>

#include <kpresenter_doc.h>

/******************************************************************/
/* class KPGotoPage						  */
/******************************************************************/

/*================================================================*/
KPGotoPage::KPGotoPage( KPresenterDoc *doc, float fakt, const QValueList<int> &slides, int start,
			QWidget *parent, const char *name, WFlags f )
    : QDialog( parent, name, f ), p(parent), oldPage(start)
{
    setCaption(i18n("Goto Page..."));

    QGridLayout *ml=new QGridLayout(this, 2, 2, 5, 5);
    label = new QLabel( i18n( "Goto Page:" ), this );
    ml->addWidget(label, 0,0);
    spinbox = new QComboBox( false, this );
    ml->addWidget(spinbox, 0,1);

    QHBoxLayout *box=new QHBoxLayout(ml);
    QPushButton *button=new QPushButton(i18n("OK"), this);
    connect(button, SIGNAL(clicked()), this, SLOT(accept()));
    box->addWidget(button);
    button=new QPushButton(i18n("Cancel"), this);
    connect(button, SIGNAL(clicked()), this, SLOT(reject()));
    box->addWidget(button);
    ml->addMultiCellLayout(box, 1, 1, 0, 1);

    QValueList<int>::ConstIterator it = slides.begin();
    unsigned int i = 0;
    for ( unsigned int j = 0; it != slides.end(); ++it, ++j ) {
        QString t(doc->getPageTitle( *it - 1, i18n( "Slide %1" ).arg( *it ), fakt ));
        // cut ultra long titles...
        if(t.length() > 30) {
            t.truncate(30);
            t+=i18n("...");
        }
        spinbox->insertItem( QString( "%1 - %2" ).arg( *it ).arg( t ), -1 );
        if ( *it == start )
            i = j;
    }
    spinbox->setCurrentItem( i );

    setFocusProxy( spinbox );
    setFocusPolicy( QWidget::StrongFocus );
    spinbox->setFocus();

    if(p)
        p->setCursor(Qt::forbiddenCursor);
}

/*================================================================*/
int KPGotoPage::gotoPage( KPresenterDoc *doc, float fakt, const QValueList<int> &slides, int start, QWidget *parent)
{
    KPGotoPage dia( doc, fakt, slides, start,parent, 0L,
                    Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WStyle_Tool | Qt::WType_Popup );
    dia.exec();
    dia.resetCursor();
    return dia.getPage();
}

int KPGotoPage::getPage() {
    if(result()==QDialog::Accepted)
        return spinbox->currentItem()+1;
    return oldPage;
}

void KPGotoPage::resetCursor() {
    if(p)
        p->setCursor(Qt::blankCursor);
}
