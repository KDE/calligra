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
#include <qbuttongroup.h>
#include <kfontdialog.h>

KChartParameterConfigPage::KChartParameterConfigPage(KChartParameters* params,QWidget* parent ) :
    QWidget( parent ),_params( params )
{
    QVBoxLayout* toplevel = new QVBoxLayout( this, 10 );

    QGridLayout* layout = new QGridLayout( 1, 3 );
    toplevel->addLayout( layout );

    QButtonGroup* gb1 = new QButtonGroup( i18n("Parameters"), this );
    QGridLayout *grid1 = new QGridLayout(gb1,9,1,15,8);

    grid = new QCheckBox( i18n( "Grid" ), gb1 );
    grid1->addWidget(grid,0,0);

    border = new QCheckBox( i18n( "Border" ), gb1 );
    grid1->addWidget(border,1,0);

    xaxis = new QCheckBox( i18n( "X-Axis" ), gb1 );
    grid1->addWidget(xaxis,2,0);

    yaxis = new QCheckBox( i18n( "Y-Axis" ), gb1);
    grid1->addWidget(yaxis,3,0);

    shelf = new QCheckBox( i18n( "Shelf grid" ), gb1 );
    grid1->addWidget(shelf,4,0);

    xlabel = new QCheckBox( i18n( "Has X-Label" ), gb1 );
    grid1->addWidget(xlabel,5,0);

    yaxis2 = new QCheckBox( i18n( "Y-Axis2" ), gb1 );
    grid1->addWidget(yaxis2,6,0);

    cross = new QCheckBox( i18n( "Cross" ), gb1 );
    grid1->addWidget(cross,7,0);

    llabel = new QCheckBox( i18n( "Legend" ), gb1 );
    grid1->addWidget(llabel,8,0);

    QButtonGroup* gb2 = new QButtonGroup( i18n("Title"), this );
    QGridLayout *grid2 = new QGridLayout(gb2,8,2,15,8);
    QLabel *tmpLabel = new QLabel( i18n( "Title" ), gb2 );
    tmpLabel->setAlignment(Qt::AlignCenter);
    grid2->addWidget(tmpLabel,0,0);

    title= new QLineEdit( gb2 );
    title->setMaximumWidth(130);
    grid2->addWidget(title,1,0);


    tmpLabel = new QLabel( i18n( "Y-Title" ), gb2 );
    tmpLabel->setAlignment(Qt::AlignCenter);
    grid2->addWidget(tmpLabel,2,0);

    ytitle= new QLineEdit( gb2 );
    ytitle->setMaximumWidth(130);
    grid2->addWidget(ytitle,3,0);

    tmpLabel = new QLabel( i18n( "X-Title" ), gb2 );
    tmpLabel->setAlignment(Qt::AlignCenter);
    grid2->addWidget(tmpLabel,4,0);

    xtitle= new QLineEdit( gb2 );
    xtitle->setMaximumWidth(130);
    grid2->addWidget(xtitle,5,0);

    tmpLabel = new QLabel( i18n( "Y-Label format" ), gb2 );
    tmpLabel->setAlignment(Qt::AlignCenter);
    grid2->addWidget(tmpLabel,0,1);

    ylabel_fmt= new QLineEdit( gb2 );
    ylabel_fmt->setMaximumWidth(130);
    grid2->addWidget(ylabel_fmt,1,1);

    tmpLabel = new QLabel( i18n( "Y-Title 2" ), gb2 );
    tmpLabel->setAlignment(Qt::AlignCenter);
    grid2->addWidget(tmpLabel,2,1);

    ytitle2= new QLineEdit( gb2 );
    ytitle2->setMaximumWidth(130);
    grid2->addWidget(ytitle2,3,1);

    tmpLabel = new QLabel( i18n( "Y-Label format 2" ), gb2 );
    tmpLabel->setAlignment(Qt::AlignCenter);
    grid2->addWidget(tmpLabel,4,1);

    ylabel2_fmt= new QLineEdit( gb2 );
    ylabel2_fmt->setMaximumWidth(130);
    grid2->addWidget(ylabel2_fmt,5,1);

    QButtonGroup* gb3 = new QButtonGroup( i18n("Annotation"), this );
    QGridLayout *grid3 = new QGridLayout(gb3,8,1,15,8);

    tmpLabel = new QLabel( i18n( "Annotation" ), gb3 );
    tmpLabel->setAlignment(Qt::AlignCenter);
    grid3->addWidget(tmpLabel,0,0);

    annotation=new QLineEdit(gb3);
    annotation->setMaximumWidth(130);
    grid3->addWidget(annotation,1,0);

    tmpLabel = new QLabel( i18n( "Position" ), gb3 );
    tmpLabel->setAlignment(Qt::AlignCenter);
    grid3->addWidget(tmpLabel,2,0);

    //element = new KIntNumInput(1,gb3,10);
    element=new QSpinBox(1,_params->xlbl.count() , 1, gb3);
    //element->setRange(1,3,1);//_params->xlbl.count());
    //element->setLabel(i18n( "Element" ));
    grid3->addWidget(element,3,0);

    tmpLabel = new QLabel( i18n( "Color" ), gb3 );
    tmpLabel->setAlignment(Qt::AlignCenter);
    grid3->addWidget(tmpLabel,4,0);

    color=new KColorButton( gb3 );
    grid3->addWidget( color, 5, 0 );

    font = new QPushButton( gb3);
    font->setText(i18n("Font"));
    grid3->addWidget( font,6,0);

    layout->addWidget(gb1,0,0);
    layout->addWidget(gb2,0,1);
    layout->addWidget(gb3,0,2);
    grid1->activate();
    grid2->activate();
    grid3->activate();

    connect(font,SIGNAL(clicked()),this,SLOT(changeFont()));
    connect( grid, SIGNAL( toggled( bool ) ),
  		   this, SLOT( changeState( bool ) ) );
    connect( xaxis, SIGNAL( toggled( bool ) ),
  		   this, SLOT( changeXaxisState( bool ) ) );
}

void KChartParameterConfigPage::changeXaxisState(bool state)
{
if(state)
   xlabel->setEnabled(true);
else
   xlabel->setEnabled(false);

}

void KChartParameterConfigPage::changeState(bool state)
{
if(state)
   shelf->setEnabled(true);
else
   shelf->setEnabled(false);

}

void KChartParameterConfigPage::changeFont()
{
if (KFontDialog::getFont( annotationFont,false,this ) == QDialog::Rejected )
        return;
}

void KChartParameterConfigPage::init()
{
    grid->setChecked(_params->grid);
    border->setChecked(_params->border);
    xaxis->setChecked(_params->xaxis);
    yaxis->setChecked(_params->yaxis);
    xlabel->setChecked(_params->hasxlabel);
    shelf->setChecked(_params->shelf);
    llabel->setChecked(_params->llabel);
    if(_params->type==KCHARTTYPE_LINE)
        {
         cross->setEnabled(true);
         cross->setChecked(_params->cross);
        }
    else
        {
         cross->setEnabled(false);
         cross->setChecked(false);
        }
    if(_params->has_yaxis2())
    	{
    	yaxis2->setChecked(_params->yaxis2);
    	int len=_params->ylabel2_fmt.length();
        ylabel2_fmt->setText(_params->ylabel2_fmt.right(len-3));

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
    int len=_params->ylabel_fmt.length();
    ylabel_fmt->setText(_params->ylabel_fmt.right(len-3));

    if(_params->xaxis)
    	xlabel->setEnabled(true);
    else
    	xlabel->setEnabled(false);

    annotationFont=_params->annotationFont();
    if(_params->annotation)
        {
        annotation->setText(_params->annotation->note);
        color->setColor(_params->annotation->color);
        element->setValue((int)_params->annotation->point+1);
        }
    else
        {
        color->setColor(Qt::black);
        annotation->setText("");
        element->setValue(1);
        }

}
void KChartParameterConfigPage::apply()
{
    _params->grid=grid->isChecked();
    _params->border=border->isChecked();
    _params->xaxis=xaxis->isChecked();
    _params->yaxis=yaxis->isChecked();
    _params->llabel=llabel->isChecked();
    if(xaxis->isChecked())
    	_params->hasxlabel=xlabel->isChecked();
    if(_params->has_yaxis2())
    	{
    	_params->yaxis2=yaxis2->isChecked();
    	if(!ylabel2_fmt->text().isEmpty())
    		_params->ylabel2_fmt="%g "+ylabel2_fmt->text();
    	_params->ytitle2=ytitle2->text();
    	}


    if(grid->isChecked())
    	_params->shelf=shelf->isChecked();
    _params->title=title->text();
    _params->xtitle=xtitle->text();
    _params->ytitle=ytitle->text();
    if(!ylabel_fmt->text().isEmpty())
    	_params->ylabel_fmt="%g "+ylabel_fmt->text();

    if(annotation->text().isEmpty())
        {
        if(_params->annotation)
                {
                delete _params->annotation;
                _params->annotation=0;
                }
        }
    else
        {
        if(!_params->annotation)
                _params->annotation=new KChartAnnotationType;
        _params->annotation->note=annotation->text();
        _params->annotation->color=color->color();
        _params->annotation->point=element->value()-1;
        _params->setAnnotationFont( annotationFont );
        }
   if(_params->type==KCHARTTYPE_LINE)
        {
         _params->cross=cross->isChecked();
        }
}
