/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#include "kchartParameterConfigPage.h"

#include "kchartParameterConfigPage.moc"

#include <kapp.h>
#include <klocale.h>


#include <qlayout.h>
#include <qlabel.h>


KChartParameterConfigPage::KChartParameterConfigPage(KChartParameters* params,QWidget* parent ) :
    QWidget( parent ),_params( params )
{
    QVBoxLayout* toplevel = new QVBoxLayout( this, 10 );
    
    QGridLayout* layout = new QGridLayout( 6, 3 );
    toplevel->addLayout( layout );
	
    grid = new QCheckBox( i18n( "Grid" ), this );
    layout->addWidget(grid,0,0);
    
    border = new QCheckBox( i18n( "Border" ), this );
    layout->addWidget(border,1,0);
    
    xaxis = new QCheckBox( i18n( "X-Axis" ), this );
    layout->addWidget(xaxis,2,0);
    
    yaxis = new QCheckBox( i18n( "Y-Axis" ), this );
    layout->addWidget(yaxis,3,0);
    
    shelf = new QCheckBox( i18n( "Shelf grid" ), this );
    layout->addWidget(shelf,4,0);
    
    yaxis2 = new QCheckBox( i18n( "Y-Axis2" ), this );
    layout->addWidget(yaxis2,5,0);
    
    QLabel *tmpLabel = new QLabel( i18n( "Title" ), this );
    tmpLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(tmpLabel,0,1);
    
    title= new QLineEdit( this );
    title->setMaximumWidth(130);
    layout->addWidget(title,1,1);
    
    
    tmpLabel = new QLabel( i18n( "Y-Title" ), this );
    tmpLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(tmpLabel,2,1);
    
    ytitle= new QLineEdit( this );
    ytitle->setMaximumWidth(130);
    layout->addWidget(ytitle,3,1);
    
    tmpLabel = new QLabel( i18n( "X-Title" ), this );
    tmpLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(tmpLabel,4,1);
    
    xtitle= new QLineEdit( this );
    xtitle->setMaximumWidth(130);
    layout->addWidget(xtitle,5,1);  
  
    tmpLabel = new QLabel( i18n( "Y-Label format" ), this );
    tmpLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(tmpLabel,0,2);
    
    ylabel_fmt= new QLineEdit( this );
    ylabel_fmt->setMaximumWidth(130);
    layout->addWidget(ylabel_fmt,1,2);  
  
    tmpLabel = new QLabel( i18n( "Y-Title 2" ), this );
    tmpLabel->setAlignment(Qt::AlignCenter);    
    layout->addWidget(tmpLabel,2,2);
    
    ytitle2= new QLineEdit( this );
    ytitle2->setMaximumWidth(130);
    layout->addWidget(ytitle2,3,2);  
  
    tmpLabel = new QLabel( i18n( "Y-Label format 2" ), this );
    tmpLabel->setAlignment(Qt::AlignCenter);    
    layout->addWidget(tmpLabel,4,2);
    
    ylabel2_fmt= new QLineEdit( this );
    ylabel2_fmt->setMaximumWidth(130);
    layout->addWidget(ylabel2_fmt,5,2);  
    
    
    connect( grid, SIGNAL( toggled( bool ) ),
  		   this, SLOT( changeState( bool ) ) );
}

void KChartParameterConfigPage::changeState(bool state)
{
if(state)
   shelf->setEnabled(true);
else
   shelf->setEnabled(false);

}

void KChartParameterConfigPage::init()
{
    grid->setChecked(_params->grid);
    border->setChecked(_params->border);
    xaxis->setChecked(_params->xaxis);
    yaxis->setChecked(_params->yaxis);
    if(_params->has_yaxis2())
    	{
    	yaxis2->setChecked(_params->yaxis2);
    	if(!_params->ylabel2_fmt.isEmpty())
    		{
    		int len=_params->ylabel2_fmt.length();
         	ylabel2_fmt->setText(_params->ylabel2_fmt.right(len-3));
         	}
    	ytitle2->setText(_params->ytitle2);
    	}
    else
    	{
    	yaxis2->setEnabled(false);
    	ylabel2_fmt->setEnabled(false);
    	ytitle2->setEnabled(false);
    	}
    if(_params->grid)
    	shelf->setEnabled(true);
    else
    	shelf->setEnabled(false);
    title->setText(_params->title);
    xtitle->setText(_params->xtitle);
    ytitle->setText(_params->ytitle);
    if(!_params->ylabel_fmt.isEmpty())
    		{
    		int len=_params->ylabel_fmt.length();
         	ylabel_fmt->setText(_params->ylabel_fmt.right(len-3));
         	}

}
void KChartParameterConfigPage::apply()
{
    _params->grid=grid->isChecked();
    _params->border=border->isChecked();
    _params->xaxis=xaxis->isChecked();
    _params->yaxis=yaxis->isChecked(); 
    
    if(_params->has_yaxis2())
    	{
    	_params->yaxis2=yaxis2->isChecked(); 
    	if(!ylabel2_fmt->text().isEmpty())
    		_params->ylabel2_fmt="%g "+ylabel2_fmt->text();
    	_params->ytitle2=ytitle2->text();
    	}
    
    
    if(grid->isChecked())
    	_params->shelf=shelf->isEnabled();
    _params->title=title->text();
    _params->xtitle=xtitle->text();
    _params->ytitle=ytitle->text();
    if(!ylabel_fmt->text().isEmpty())
    	_params->ylabel_fmt="%g "+ylabel_fmt->text();
      
}
