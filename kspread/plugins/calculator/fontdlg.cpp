/*

 $Id$

 KCalc

 Copyright (C) Bernd Johannes Wuebben
               wuebben@math.cornell.edu
	       wuebben@kde.org

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 */

#include "fontdlg.h"

#include <qfontdialog.h>
#include <klocale.h>
#include <qgrid.h>
#include <qlayout.h>


FontDlg::FontDlg(QWidget *parent, const char *name, DefStruct *defstruct)
  : QDialog(parent, name)
{
  defst = defstruct;
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );

  box = new QGroupBox(this, "box");
  box->setTitle(i18n("Set Default Font"));

  QGridLayout *grid1 = new QGridLayout(box,8,2,15,7);

  button = new QPushButton(box);
  grid1->addWidget(button,5,1);
  button->setText(i18n("Change"));

  connect(button,SIGNAL(clicked()),this,SLOT(setFont()));

  familylabel = new QLabel(box);
  familylabel->setText(i18n("Family:"));
  grid1->addWidget(familylabel,0,0);


  familylabeldisp = new QLabel(box);
  grid1->addWidget(familylabeldisp,0,1);
  familylabeldisp->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  familylabeldisp->setText(defst->font.family());
//  familylabeldisp->setBackgroundColor(white);

  sizelabel = new QLabel(box);
  sizelabel->setText(i18n("Point Size:"));
  grid1->addWidget(sizelabel,1,0);

  sizelabeldisp = new QLabel(box);
  sizelabeldisp->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  grid1->addWidget(sizelabeldisp,1,1);
//  sizelabeldisp->setBackgroundColor(white);
  QString size;
  size.setNum(defst->font.pointSize());
  sizelabeldisp->setText(size);


  stylelabel = new QLabel(box);
  grid1->addWidget(stylelabel,3,0);
  stylelabel->setText(i18n("Style:"));


  stylelabeldisp = new QLabel(box);
  stylelabeldisp->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  grid1->addWidget(stylelabeldisp,3,1);
//  stylelabeldisp->setBackgroundColor(white);

  if(defst->font.italic())
    stylelabeldisp->setText(i18n("Italic"));
  else
    stylelabeldisp->setText(i18n("Roman"));

  weightlabel = new QLabel(box);
  grid1->addWidget(weightlabel,2,0);
  weightlabel->setText(i18n("Weight:"));



  weightlabeldisp = new QLabel(box);
  grid1->addWidget(weightlabeldisp,2,1);
  weightlabeldisp->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
//  weightlabeldisp->setBackgroundColor(white);

  if(defst->font.bold())
    weightlabeldisp->setText(i18n("Bold"));
  else
    weightlabeldisp->setText(i18n("Normal"));

  connect(parent,SIGNAL(applyButtonPressed()),SLOT(okButton()));
  lay1->addWidget(box);
}


void FontDlg::help(){

}

void FontDlg::okButton(){



}
void FontDlg::cancelbutton() {
  reject();
}


void FontDlg::setFont(){

    bool ok = TRUE;
    QFont f = QFontDialog::getFont( &ok, defst->font, this );
    if ( ok )
    {
	defst->font = f;

	familylabeldisp->setText(defst->font.family());

	if(defst->font.bold())
	    weightlabeldisp->setText(i18n("Bold"));
	else
	    weightlabeldisp->setText(i18n("Normal"));

	if(defst->font.italic())
	    stylelabeldisp->setText(i18n("Italic"));
	else
	    stylelabeldisp->setText(i18n("Roman"));

	QString size;
	size.setNum(defst->font.pointSize());
	sizelabeldisp->setText(size);
    }
}

#include "fontdlg.moc"
