
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
#include <ktabctl.h>
#include <stdlib.h>
#include "matrixwidget.h"
/*
Matrix Setup Dialog
*/

MatrixSetupWidget::MatrixSetupWidget(QWidget* parent,
	const char* name): QDialog( parent, name, TRUE )
{
    setMinimumSize(380,360);
    setMaximumSize(380,360);

    KTabCtl *tab = new KTabCtl( this, "ktab" );
    setCaption( i18n("KFormula - Matrix Element Setup " ));
    tab->setGeometry(5,10,340,300);
    QWidget *w = new QWidget( tab, "page one" );
    QButtonGroup* tmpQButtonGroup;
    tmpQButtonGroup = new QButtonGroup( w, "ButtonGroup_1" );
    tmpQButtonGroup->setGeometry( 10, 10, 320, 100 );	
    tmpQButtonGroup->setFrameStyle( 49 );
    tmpQButtonGroup->setTitle( i18n("Size & Space:") );
    QLabel* tmpQLabel;
    tmpQLabel = new QLabel( w, "Label_1" );
    tmpQLabel->setGeometry( 20, 25, 90, 30 );
    tmpQLabel->setText(i18n("Columns:" ));
    spb[1] = new KNumericSpinBox( w, "SpinX" );
    spb[1]->setGeometry( 110, 25, 60, 30 );
    spb[1]->setRange(1,999);
    tmpQLabel = new QLabel( w, "Label_2" );
    tmpQLabel->setGeometry( 180, 25, 90, 30 );
    tmpQLabel->setText(i18n("Rows" ));
    spb[2] = new KNumericSpinBox( w, "SpinY" );
    spb[2]->setGeometry( 260, 25, 60, 30 );
    spb[2]->setRange(1,999);
    connect(spb[2],SIGNAL(valueIncreased()),this, SLOT(valueChanged()));
    connect(spb[2],SIGNAL(valueDecreased()),this, SLOT(valueChanged()));
    tmpQLabel = new QLabel( w, "Label_2" );
    tmpQLabel->setGeometry( 20, 70,100, 30 );
    tmpQLabel->setText(i18n("Space:" ));
    spb[3] = new KNumericSpinBox( w, "SpinSpace" );
    spb[3]->setGeometry( 110, 70, 60, 30 );
    spb[3]->setRange(1,999);
    tmpQButtonGroup = new QButtonGroup( w, "ButtonGroup_2" );
    tmpQButtonGroup->setGeometry( 10, 110, 320, 150 );	
    tmpQButtonGroup->setFrameStyle( 49 );
    tmpQButtonGroup->setTitle( i18n("Borders:") );
    co[0] = new QComboBox(w,"Combo1");
    co[0]->setGeometry( 200,130,100,20);
    co[0]->insertItem(i18n("No-border"));
    co[0]->insertItem(i18n("Single Line"));		
    co[0]->insertItem(i18n("Double Lne"));		

    co[1] = new QComboBox(w,"Combo2");
    co[1]->setGeometry( 200,150,100,20);
    co[1]->insertItem(i18n("No-border"));
    co[1]->insertItem(i18n("Single Line"));		
    co[1]->insertItem(i18n("Double Lne"));		

    co[2] = new QComboBox(w,"Combo3");
    co[2]->setGeometry( 200,170,100,20);
    co[2]->insertItem(i18n("No-border"));
    co[2]->insertItem(i18n("Single Line"));		
    co[2]->insertItem(i18n("Double Lne"));		

    co[3] = new QComboBox(w,"Combo4");
    co[3]->setGeometry( 200,190,100,20);
    co[3]->insertItem(i18n("No-border"));
    co[3]->insertItem(i18n("Single Line"));		
    co[3]->insertItem(i18n("Double Lne"));		

    co[4] = new QComboBox(w,"Combo5");
    co[4]->setGeometry( 200,210,100,20);
    co[4]->insertItem(i18n("No-border"));
    co[4]->insertItem(i18n("Single Line"));		
    co[4]->insertItem(i18n("Double Lne"));		

    co[5] = new QComboBox(w,"Combo6");
    co[5]->setGeometry( 200,230,100,20);
    co[5]->insertItem(i18n("No-border"));
    co[5]->insertItem(i18n("Single Line"));		
    co[5]->insertItem(i18n("Double Lne"));		

    tmpQLabel = new QLabel( w, "Label_8" );
    tmpQLabel->setGeometry(20, 130, 170, 20 );
    tmpQLabel->setText(i18n("Internal Horizonatl" ));
    tmpQLabel = new QLabel( w, "Label_3" );
    tmpQLabel->setGeometry(20, 150, 170, 20 );
    tmpQLabel->setText(i18n("Internal Vertical" ));
    tmpQLabel = new QLabel( w, "Label_4" );
    tmpQLabel->setGeometry(20, 170, 170, 20 );
    tmpQLabel->setText(i18n("Top" ));
    tmpQLabel = new QLabel( w, "Label_5" );
    tmpQLabel->setGeometry(20, 190, 170, 20 );
    tmpQLabel->setText(i18n("Bottom" ));
    tmpQLabel = new QLabel( w, "Label_6" );
    tmpQLabel->setGeometry(20, 210, 170, 20 );
    tmpQLabel->setText(i18n("Left" ));
    tmpQLabel = new QLabel( w, "Label_7" );
    tmpQLabel->setGeometry(20, 230, 170, 20 );
    tmpQLabel->setText(i18n("Rigth" ));


    w->resize( 350, 300 );
    tab->addTab( w, i18n("General"));
    w = new QWidget( tab, "page two" );
    tmpQButtonGroup = new QButtonGroup( w, "ButtonGroup_3" );
    tmpQButtonGroup->setGeometry( 10, 10, 320, 170 );	
    tmpQButtonGroup->setFrameStyle( 49 );
    tmpQButtonGroup->setTitle( i18n("Vertical:") );
    cb[0] = new QRadioButton( w, "RadioButton_1" );
    cb[0]->setGeometry( 15, 25, 120, 30 );
    cb[0]->setText( i18n("Fixed Row") );
    spb[0] = new KNumericSpinBox( w, "FixRow" );
    spb[0]->setGeometry( 130, 25, 40, 30 );	
    cb[1] = new QRadioButton( w, "RadioButton_2" );
    cb[1]->setGeometry( 15, 120, 120, 30 );
    cb[1]->setText( i18n("Half Matrix") );
    tmpQButtonGroup->insert( cb[0] );
    tmpQButtonGroup->insert( cb[1] );
    tmpQButtonGroup->setExclusive( TRUE );

    tmpQButtonGroup = new QButtonGroup( w, "ButtonGroup_5" );
    tmpQButtonGroup->setGeometry( 1, 1, 0, 0 );	
    tmpQButtonGroup->setFrameStyle( 0 );
    cb[2] = new QRadioButton( w, "RadioButton_1" );
    cb[2]->setGeometry( 190, 25, 120, 30 );
    cb[2]->setText( i18n("Row Midline") );
    cb[3] = new QRadioButton( w, "RadioButton_1" );
    cb[3]->setGeometry( 190, 55, 120, 30 );
    cb[3]->setText( i18n("Over the row" ));
    cb[4] = new QRadioButton( w, "RadioButton_2" );
    cb[4]->setGeometry( 190, 85, 120, 30 );
    cb[4]->setText( i18n("Under the row" ));

	tmpQButtonGroup->insert( cb[2] );
	tmpQButtonGroup->insert( cb[3] );
	tmpQButtonGroup->insert( cb[4] );
	tmpQButtonGroup->setExclusive( TRUE );
	
    tmpQButtonGroup = new QButtonGroup( w, "ButtonGroup_4" );
    tmpQButtonGroup->setGeometry( 10, 180, 320, 70 );	
    tmpQButtonGroup->setFrameStyle( 49 );
    tmpQButtonGroup->setTitle( i18n("Horizontal:") );
    co[6] = new QComboBox(w,"Combo6");
    co[6]->setGeometry( 70,200,170,30);
    co[6]->insertItem(i18n("Center "));
    co[6]->insertItem(i18n("Left side"));		
    co[6]->insertItem(i18n("Right side"));		
		
    w->resize( 350, 300 );
    tab->addTab( w, i18n("Align"));

    QPushButton* tmpQPushButton;
    tmpQPushButton = new QPushButton( this,"pushButton_1" );
    tmpQPushButton->setGeometry( 260, 320, 80, 30 );
    connect( tmpQPushButton, SIGNAL(clicked()), SLOT(OkPressed()) );
    tmpQPushButton->setText( i18n("&OK" ));
    tmpQPushButton = new QPushButton( this,"pushButton_2" );
    tmpQPushButton->setGeometry( 20, 320, 80, 30 );
    connect( tmpQPushButton, SIGNAL(clicked()), SLOT(CancelPressed()) );
    tmpQPushButton->setText( i18n("&Cancel" ));
	
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
    warning("CB1 checked");
     if (cb[2]->isChecked())
      vch='M';
     if (cb[3]->isChecked())
      vch='U';
     if (cb[4]->isChecked())
      vch='D';
  }

ret.sprintf("M%cC%03d%03d%03d%03dLLLLLL",vch,spb[2]->getValue(),spb[1]->getValue(),spb[0]->getValue()-1,spb[3]->getValue());
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


warning(ret);
emit returnString(ret);
close();
}

void MatrixSetupWidget::CancelPressed()
{
close();
delete this;
}

void MatrixSetupWidget::valueChanged()
{
int x=spb[2]->getValue();
spb[0]->setRange(1,x);
if(spb[0]->getValue()>x)
  spb[0]->setValue(x);
}

void MatrixSetupWidget::setString(QString str)
{
spb[2]->setValue(atoi(str.mid(3,3)));
spb[1]->setValue(atoi(str.mid(6,3)));
spb[0]->setValue(atoi(str.mid(9,3))+1);
spb[3]->setValue(atoi(str.mid(12,3)));
spb[0]->setRange(1,spb[2]->getValue());
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
warning(str);
}



#include "matrixwidget.moc"
