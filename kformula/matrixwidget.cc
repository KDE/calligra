
/*
 *
 * KFormula - Formula Editor
 *
 * This code is free. Do with it what you want.
 *
 * (c) KDE Project
 * Andrea Rizzi
 * bilibao@ouverture.it
 */



#include <kapp.h>

#include <kdebug.h>

#include <stdlib.h>

#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlayout.h>

#include "matrixwidget.h"

/*
Matrix Setup Dialog
*/

MatrixSetupWidget::MatrixSetupWidget(QWidget* parent,const char* name)
        :KDialogBase(KDialogBase::Tabbed, i18n("KFormula - Matrix Element Setup"), Ok|Cancel, Ok, parent, name)
{

    QFrame *page1 = addPage(i18n("General"));
    QVBoxLayout *lay1 = new QVBoxLayout( page1 );
    lay1->setMargin( 5 );
    lay1->setSpacing( 10 );


    QButtonGroup* tmpQButtonGroup;
    tmpQButtonGroup = new QButtonGroup( page1, "ButtonGroup_1" );
    tmpQButtonGroup->setFrameStyle( 49 );
    tmpQButtonGroup->setTitle( i18n("Size & Space:") );
    QGridLayout *grid1 = new QGridLayout(tmpQButtonGroup,2,4,15,7);

    QLabel* tmpQLabel;
    tmpQLabel = new QLabel( tmpQButtonGroup, "Label_1" );
    tmpQLabel->setText(i18n("Columns:" ));
    grid1->addWidget(tmpQLabel,0,0);

    spb[1] = new QSpinBox( 1, 999, 1, tmpQButtonGroup, "SpinX" );
    grid1->addWidget(spb[1],0,1);

    tmpQLabel = new QLabel( tmpQButtonGroup, "Label_2" );
    tmpQLabel->setText(i18n("Rows" ));
    grid1->addWidget(tmpQLabel,0,2);


    spb[2] = new QSpinBox( 1, 999, 1, tmpQButtonGroup, "SpinY" );
    grid1->addWidget(spb[2],0,3);

    connect(spb[2],SIGNAL(valueChanged(int)),this, SLOT(valueChanged(int)));

    tmpQLabel = new QLabel( tmpQButtonGroup, "Label_2" );
    tmpQLabel->setText(i18n("Space:" ));
    grid1->addWidget(tmpQLabel,1,0);

    spb[3] = new QSpinBox( 1, 999, 1, tmpQButtonGroup, "SpinSpace" );
    grid1->addWidget(spb[3],1,1);
    lay1->addWidget(tmpQButtonGroup);



    tmpQButtonGroup = new QButtonGroup( page1, "ButtonGroup_2" );
    grid1 = new QGridLayout(tmpQButtonGroup,6,2,15,7);

    tmpQButtonGroup->setFrameStyle( 49 );
    tmpQButtonGroup->setTitle( i18n("Borders:") );

    co[0] = new QComboBox(tmpQButtonGroup,"Combo1");
    co[0]->insertItem(i18n("No-border"));
    co[0]->insertItem(i18n("Single Line"));
    co[0]->insertItem(i18n("Double Line"));
    grid1->addWidget(co[0],0,1);

    co[1] = new QComboBox(tmpQButtonGroup,"Combo2");
    co[1]->insertItem(i18n("No-border"));
    co[1]->insertItem(i18n("Single Line"));
    co[1]->insertItem(i18n("Double Line"));
    grid1->addWidget(co[1],1,1);

    co[2] = new QComboBox(tmpQButtonGroup,"Combo3");
    co[2]->insertItem(i18n("No-border"));
    co[2]->insertItem(i18n("Single Line"));
    co[2]->insertItem(i18n("Double Line"));
    grid1->addWidget(co[2],2,1);

    co[3] = new QComboBox(tmpQButtonGroup,"Combo4");
    co[3]->insertItem(i18n("No-border"));
    co[3]->insertItem(i18n("Single Line"));
    co[3]->insertItem(i18n("Double Line"));
    grid1->addWidget(co[3],3,1);

    co[4] = new QComboBox(tmpQButtonGroup,"Combo5");
    co[4]->insertItem(i18n("No-border"));
    co[4]->insertItem(i18n("Single Line"));
    co[4]->insertItem(i18n("Double Line"));
    grid1->addWidget(co[4],4,1);

    co[5] = new QComboBox(tmpQButtonGroup,"Combo6");
    co[5]->insertItem(i18n("No-border"));
    co[5]->insertItem(i18n("Single Line"));
    co[5]->insertItem(i18n("Double Line"));
    grid1->addWidget(co[5],5,1);

    tmpQLabel = new QLabel( tmpQButtonGroup, "Label_8" );
    grid1->addWidget(tmpQLabel,0,0);
    tmpQLabel->setText(i18n("Internal Horizontal" ));

    tmpQLabel = new QLabel( tmpQButtonGroup, "Label_3" );
    grid1->addWidget(tmpQLabel,1,0);
    tmpQLabel->setText(i18n("Internal Vertical" ));

    tmpQLabel = new QLabel( tmpQButtonGroup, "Label_4" );
    grid1->addWidget(tmpQLabel,2,0);
    tmpQLabel->setText(i18n("Top" ));

    tmpQLabel = new QLabel( tmpQButtonGroup, "Label_5" );
    grid1->addWidget(tmpQLabel,3,0);
    tmpQLabel->setText(i18n("Bottom" ));

    tmpQLabel = new QLabel( tmpQButtonGroup, "Label_6" );
    grid1->addWidget(tmpQLabel,4,0);
    tmpQLabel->setText(i18n("Left" ));

    tmpQLabel = new QLabel( tmpQButtonGroup, "Label_7" );
    grid1->addWidget(tmpQLabel,5,0);
    tmpQLabel->setText(i18n("Right" ));

    lay1->addWidget(tmpQButtonGroup);


    QFrame *page2 = addPage(i18n("Align"));

    lay1 = new QVBoxLayout( page2 );
    lay1->setMargin( 5 );
    lay1->setSpacing( 10 );

    tmpQButtonGroup = new QButtonGroup( page2, "ButtonGroup_3" );

    grid1 = new QGridLayout(tmpQButtonGroup,1,2,15,7);
    tmpQButtonGroup->setFrameStyle( 49 );
    tmpQButtonGroup->setTitle( i18n("Vertical:") );

    QButtonGroup *tmpQButtonGroup1 = new QButtonGroup( tmpQButtonGroup, "ButtonGroup_31" );
    tmpQButtonGroup1->setFrameStyle( 0 );
    QGridLayout *grid2 = new QGridLayout(tmpQButtonGroup1,2,2,15,7);
    cb[0] = new QRadioButton( tmpQButtonGroup1, "RadioButton_1" );
    cb[0]->setText( i18n("Fixed Row") );
    grid2->addWidget(cb[0],0,0);

    spb[0] = new QSpinBox( tmpQButtonGroup1, "FixRow" );
    grid2->addWidget(spb[0],0,1);

    cb[1] = new QRadioButton( tmpQButtonGroup1, "RadioButton_2" );
    grid2->addWidget(cb[1],1,0);
    cb[1]->setText( i18n("Half Matrix") );
    tmpQButtonGroup1->setRadioButtonExclusive( TRUE );
    grid1->addWidget(tmpQButtonGroup1,0,0);


    QButtonGroup *tmpQButtonGroup2 = new QButtonGroup( tmpQButtonGroup, "ButtonGroup_31" );
    tmpQButtonGroup2->setFrameStyle( 0 );
    grid2 = new QGridLayout(tmpQButtonGroup2,3,1,15,7);
    cb[2] = new QRadioButton( tmpQButtonGroup2, "RadioButton_1" );
    cb[2]->setText( i18n("Row Midline") );
    grid2->addWidget(cb[2],0,0);
    cb[3] = new QRadioButton( tmpQButtonGroup2, "RadioButton_1" );
    cb[3]->setText( i18n("Over the row" ));
    grid2->addWidget(cb[3],1,0);
    cb[4] = new QRadioButton( tmpQButtonGroup2, "RadioButton_2" );
    cb[4]->setText( i18n("Under the row" ));
    grid2->addWidget(cb[4],2,0);
    grid1->addWidget(tmpQButtonGroup2,0,1);
    tmpQButtonGroup2->setRadioButtonExclusive( TRUE );

    lay1->addWidget(tmpQButtonGroup);

    tmpQButtonGroup = new QButtonGroup( page2, "ButtonGroup_4" );
    grid1 = new QGridLayout(tmpQButtonGroup,1,1,15,7);

    tmpQButtonGroup->setFrameStyle( 49 );
    tmpQButtonGroup->setTitle( i18n("Horizontal:") );
    co[6] = new QComboBox(tmpQButtonGroup,"Combo6");
    grid1->addWidget(co[6],0,0);
    co[6]->insertItem(i18n("Center "));
    co[6]->insertItem(i18n("Left side"));
    co[6]->insertItem(i18n("Right side"));
    lay1->addWidget(tmpQButtonGroup);

    connect( this, SIGNAL(okClicked()), SLOT(OkPressed()) );

}


MatrixSetupWidget::~MatrixSetupWidget()
{
}

void MatrixSetupWidget::OkPressed()
{
QString ret;
char vch='C';  //default is centred

if ( cb[1]->isChecked() )
  vch='C';

if ( cb[0]->isChecked() )
  {
    kdDebug(39001) <<"CB1 checked\n";
     if (cb[2]->isChecked())
      vch='M';
     if (cb[3]->isChecked())
      vch='U';
     if (cb[4]->isChecked())
      vch='D';
  }

ret.sprintf("M%cC%03d%03d%03d%03dLLLLLL",vch,spb[2]->value(),spb[1]->value(),spb[0]->value()-1,spb[3]->value());
for (int k=0;k<6;k++)
 {
  char bch='N';
  if(co[k]->currentItem()==0) bch='N';
  if(co[k]->currentItem()==1) bch='L';
  if(co[k]->currentItem()==2) bch='D';
  ret[15+k]=bch;
 }
  if(co[6]->currentItem()==0) ret[2]='C';
  if(co[6]->currentItem()==1) ret[2]='L';
  if(co[6]->currentItem()==2) ret[2]='R';


  kdDebug(39001) <<ret<<endl;
  emit returnString(ret);
}

void MatrixSetupWidget::valueChanged(int)
{
int x=spb[2]->value();
spb[0]->setRange(1,x);
if(spb[0]->value()>x)
  spb[0]->setValue(x);
}

void MatrixSetupWidget::setString(QString str)
{
spb[2]->setValue(str.mid(3,3).toInt());
spb[1]->setValue(str.mid(6,3).toInt());
spb[0]->setValue(str.mid(9,3).toInt()+1);
spb[3]->setValue(str.mid(12,3).toInt());

spb[0]->setRange(1,spb[2]->value());
cb[0]->setChecked((str[1]=='M') || (str[1]=='D') || (str[1]=='U'));
cb[1]->setChecked((str[1]=='C'));
cb[2]->setChecked((str[1]=='M') ); //mid
cb[3]->setChecked((str[1]=='U'));  //over
cb[4]->setChecked((str[1]=='D'));  //under
for (int k=0;k<6;k++)
 {
  char bch=QChar(str[15+k]);
  if(bch=='N')co[k]->setCurrentItem(0);
  if(bch=='L')co[k]->setCurrentItem(1);
  if(bch=='D')co[k]->setCurrentItem(2);
 }

  if(str[2]=='C')co[6]->setCurrentItem(0);
  if(str[2]=='L')co[6]->setCurrentItem(1);
  if(str[2]=='R')co[6]->setCurrentItem(2);

if(!cb[0]->isChecked())
    cb[2]->setChecked(TRUE);

kdDebug(39001) <<str<<endl;
}



#include "matrixwidget.moc"
