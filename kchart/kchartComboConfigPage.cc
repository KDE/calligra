/* This file is part of the KDE project
   Copyright (C) 2001,2002,2003,2004 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kchartComboConfigPage.h"
#include "kchartComboConfigPage.moc"

#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>
#include <kdialog.h>
#include <QLayout>
#include <QLabel>
#include <qradiobutton.h>
#include <q3buttongroup.h>
//Added by qt3to4:
#include <Q3GridLayout>

#include "kchart_params.h"

namespace KChart
{

KChartComboPage::KChartComboPage( KChartParams* params,
                                  QWidget* parent ) :
    QWidget( parent ),_params( params )
{
    //QVBoxLayout* toplevel = new QVBoxLayout( this, 10 );

    Q3GridLayout* layout = new Q3GridLayout(this, 2, 2,KDialog::marginHint(), KDialog::spacingHint() );
    //toplevel->addLayout( layout );
    Q3ButtonGroup* gb = new Q3ButtonGroup( i18n("HLC Style"), this );
    Q3GridLayout *grid1 = new Q3GridLayout(gb,7,1,KDialog::marginHint(), KDialog::spacingHint());
    layout->addWidget(gb,0,0);

    diamond=new QRadioButton( i18n("Diamond"), gb ); ;
    grid1->addWidget(diamond,0,0);
    closeconnected=new QRadioButton( i18n("Close connected"), gb );
    grid1->addWidget(closeconnected,1,0);
    connecting=new QRadioButton( i18n("Connecting"), gb );
    grid1->addWidget(connecting,2,0);
    icap=new QRadioButton( i18n("Icap"), gb );
    grid1->addWidget(icap,3,0);
    gb->setAlignment(Qt::AlignLeft);
    grid1->addItem( new QSpacerItem(diamond->width(), 0 ), 0, 0 );
    grid1->addItem( new QSpacerItem(closeconnected->width(), 0 ), 0, 0 );
    grid1->addItem( new QSpacerItem(connecting->width(), 0 ), 0, 0 );
    grid1->addItem( new QSpacerItem(icap->width(), 0 ), 0, 0 );
    grid1->setColStretch(0,1);
    grid1->activate();
    //it's not good but I don't know how
    //to reduce space
    layout->addItem( new QSpacerItem(300, 0 ), 0, 1 );
}

void KChartComboPage::init()
{
    qDebug( "Sorry, not implemented: KChartComboPage::init()" );
#ifdef __GNUC__
#warning Put back in
#endif
#ifdef K
    switch((int)_params->hlc_style)
        {
        case (int)KCHARTHLCSTYLE_DIAMOND:
            {
                diamond->setChecked(true);
                break;
            }
        case (int)KCHARTHLCSTYLE_CLOSECONNECTED:
            {
                closeconnected->setChecked(true);
                break;
            }
        case (int)KCHARTHLCSTYLE_CONNECTING:
            {
                connecting->setChecked(true);
                break;
            }
        case (int)KCHARTHLCSTYLE_ICAP:
            {
                icap->setChecked(true);
                break;
            }
        default:
            {
                kDebug(35001)<<"Error in hlc_style\n";
                break;
            }
        }
#endif
}

void KChartComboPage::apply()
{
    qDebug( "Sorry, not implemented: KChartComboPage::apply()" );
#ifdef __GNUC__
#warning Put back in
#endif
#ifdef K
    if(diamond->isChecked())
        {
            _params->hlc_style = KCHARTHLCSTYLE_DIAMOND;
        }
    else if(closeconnected->isChecked())
        {
            _params->hlc_style = KCHARTHLCSTYLE_CLOSECONNECTED;
        }
    else if(connecting->isChecked())
        {
            _params->hlc_style = KCHARTHLCSTYLE_CONNECTING;
        }
    else if(icap->isChecked())
        {
            _params->hlc_style = KCHARTHLCSTYLE_ICAP;
        }
    else
        {
            kDebug(35001)<<"Error in groupbutton\n";
        }
#endif
}

}  //KChart namespace
