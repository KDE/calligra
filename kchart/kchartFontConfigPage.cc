/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#include "kchartFontConfigPage.h"

#include "kchartFontConfigPage.moc"

#include <kapp.h>
#include <klocale.h>


#include <qlayout.h>
#include <qlabel.h>
#include <kfontdialog.h>

KChartFontConfigPage::KChartFontConfigPage(KChartParameters* params,QWidget* parent ) :
    QWidget( parent ),_params( params )
{
   QGridLayout *grid = new QGridLayout(this,5,4,15,7);

   list=new QListBox(this);
   list->resize( list->sizeHint() );
   //grid->addWidget( list,0,0);
   grid->addMultiCellWidget(list,0,4,0,0);
   fontButton = new QPushButton( this);
   fontButton->setText(i18n("Font"));
 
   fontButton->resize( fontButton->sizeHint() );
   grid->addWidget( fontButton,2,1);
   grid->addColSpacing(0,list->width());
   grid->addColSpacing(2,list->width());
   grid->addColSpacing(3,list->width());
      
   initList(); 
   connect(fontButton,SIGNAL(clicked()),this,SLOT(changeLabelFont()));   
  
}

void KChartFontConfigPage::initList()
{
  list->insertItem(i18n("Title"));
  if(!_params->isPie())
  	{
  	list->insertItem(i18n("X-Title"));
  	list->insertItem(i18n("Y-Title"));
  	list->insertItem(i18n("X-Axis"));
  	list->insertItem(i18n("Y-Axis"));
  	}
  list->insertItem(i18n("Label"));
  list->setCurrentItem(0);
}

void KChartFontConfigPage::changeLabelFont()
{
if(list->currentText()==i18n("Title"))
	{
	if (KFontDialog::getFont( title,true,this ) == QDialog::Rejected )
	      return;
	}
else if(list->currentText()==i18n("X-Title"))
	{
	if (KFontDialog::getFont( xtitle,true,this ) == QDialog::Rejected )
	      return;
	}
else if(list->currentText()==i18n("Y-Title"))
	{
	if (KFontDialog::getFont( ytitle,true,this ) == QDialog::Rejected )
	      return;
	}
else if(list->currentText()==i18n("X-Axis"))
	{
	if (KFontDialog::getFont( xaxis,true,this ) == QDialog::Rejected )
	      return;
	}
else if(list->currentText()==i18n("Y-Axis"))
	{
	if (KFontDialog::getFont( yaxis,true,this ) == QDialog::Rejected )
	      return;
	}
else if(list->currentText()==i18n("Label"))
	{
	if (KFontDialog::getFont( label,true,this ) == QDialog::Rejected )
	      return;
	}
else
	{
	cout <<"Pb in listBox\n";
	}


}


void KChartFontConfigPage::init()
{

  title=_params->titleFont();
  xtitle=_params->xTitleFont();
  ytitle=_params->yTitleFont();
  xaxis=_params->xAxisFont();
  yaxis=_params->yAxisFont();
  label=_params->labelFont();
}
void KChartFontConfigPage::apply()
{
  _params->setLabelFont(label);
  if(!_params->isPie())
  	{
	_params->setXAxisFont(xaxis);
  	_params->setYAxisFont(yaxis);
  	_params->setXTitleFont(xtitle);
  	_params->setYTitleFont(ytitle);
  	}
  _params->setTitleFont(title);
  }
