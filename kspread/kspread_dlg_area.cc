/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Montel Laurent <montell@club-internet.fr>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/



#include "kspread_dlg_area.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_tabbar.h"
#include "kspread_table.h"
#include "kspread_doc.h"
#include <kapp.h>
#include <klocale.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <kbuttonbox.h>
#include <qstrlist.h>
#include <qlist.h>
#include <kmessagebox.h>

KSpreadarea::KSpreadarea( KSpreadView* parent, const char* name,const QPoint &_marker )
	: QDialog( parent, name,TRUE )
{
  m_pView = parent;
  marker=_marker;

  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );

  areaName=new QLineEdit(this);
  lay1->addWidget( areaName );

  setCaption( i18n("Area Name") );

  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  m_pOk = bb->addButton( i18n("OK") );
  m_pOk->setDefault( TRUE );
  m_pClose = bb->addButton( i18n( "Close" ) );
  bb->layout();
  lay1->addWidget( bb );
  areaName->setFocus();
  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
}



void KSpreadarea::slotOk()
{
  if( !areaName->text().isEmpty())
  {
        QRect rect( m_pView->activeTable()->selectionRect() );
        if ( rect.left() == 0 || rect.top() == 0 ||
                rect.right() == 0 || rect.bottom() == 0 )
                {
                rect.setCoords( marker.x(), marker.y(), marker.x(),marker.y() );
                }
        bool newName=true;
        QValueList<Reference>::Iterator it;
        QValueList<Reference> area=m_pView->doc()->listArea();
        for ( it = area.begin(); it != area.end(); ++it )
    	        {
    	        if(areaName->text()==(*it).ref_name)
                        newName=false;
    	        }
        if(newName)
                {
                m_pView->doc()->addAreaName(rect,areaName->text(),m_pView->activeTable()->name());
                accept();
                }
        else
                KMessageBox::error( this, i18n("This name is already used."));
  }
  else
  {
        KMessageBox::error( this, i18n("Area text is empty!") );
  }
}
#include <math.h>
void KSpreadarea::slotClose()
{
  cout <<"14.1567\n";
  double x=14.1567;
  int n=2;
  double preci=pow(10,-3);
  /*double q1=1;
  bool cont=true;
  double p1=0;
  double q=0;
  double d=0;
  cout <<"0.4 :"<<rint(0.4)<<endl;
  cout <<"0.5 :"<<rint(0.5)<<endl;
  cout <<"0.6 :"<<rint(0.6)<<endl;
  cout <<"10^n :"<<pow(10,n)<<endl;
  while(cont)
  {
  p1=rint(q1*x);
  d=fabs(x-p1/q1);
  cout <<"d :"<<d <<endl;
  if(d!=0)
        {
        q=1;
        while( (fabs(q*x-rint(q*x))>=fabs(q1*x-rint(q1*x))&& q<pow(10,n)))
                {
                q++;
                cout <<"entrer :"<<q<<endl;
                }
        cout <<"q :"<<q<<" q1 :"<<q1<<endl;
        if(q<(pow(10,n)))
                {
                q1=q;
                }
        else
                cont=false;
        }
  else
        cont=false;
  }

  */
  cout <<"preci :"<<preci<<endl;
  double y=x;
//> partie entière (x) -> c
double c=rint(x);

//> Partie entière (x) -> a
double a=rint(x);
double e=1;
double d=1;
double f=0;
//> 1 -> e
//> 1 -> d
//> 0 -> f
double p,q;
while(fabs(c/d-x)>preci)
{
y=(1/(y-a));
a=rint(y);
p= a*c + e;
q= a*d + f;
e=c;
f=d;
c=p;
d=q;
}
/*> Tant que abs (c/d-x) > E faire
> ....(1/(y-a)) -> y
> ....Partie entière (y) -> a
> ....a*c + e -> p
> ....a*d + f -> q
> ....c -> e
> ....d -> f
> ....p -> c
> ....q -> d
> Fin Tant que
> return c/d*/
  cout <<" c :"<<c<<" d :"<<d<<endl;


  reject();
}


#include "kspread_dlg_area.moc"
