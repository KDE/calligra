
#include "kpropspin.h"
#include "kpropspin.moc"
#include <kapp.h>

KPropSpin::KPropSpin( QWidget *parent, const char *text, int perc, const char *key, const char *group, KConfig *config, const char *name )
	: QLabel( text, parent,name )
{
	setKConfig( config );
	setGroup( group );
	setKey( key );

	setFontPropagation( QWidget::SameFont );
	spinBox = new QSpinBox( this );
	percentage = perc;

	sizeUpdate();
}

KPropSpin::~KPropSpin()
{
	delete spinBox;
}

void KPropSpin::sizeUpdate()
{
	QFontMetrics fm( font() );
	int h=fm.height();
	
	setFixedHeight( h + 8 );
	spinBox->setFixedHeight( h + 4 );
}

void KPropSpin::resizeEvent( QResizeEvent *rev )
{
	int w = width()*percentage/100;
	spinBox->setGeometry( width() - w - 2, 2, w - 4, 100 );
}

void KPropSpin::fontChange( const QFont & )
{
	sizeUpdate();
}

QSpinBox *KPropSpin::getSpinBox()
{
	return spinBox;
}

const char *KPropSpin::getContents()
{
  return spinBox->text();
}

void KPropSpin::setContents( int value )
{
	spinBox->setValue( value );
}

void KPropSpin::setContents( const char *s )
{
  QString tmpStr(s);
  int index;
  if ((index = tmpStr.find(spinBox->suffix(), 0)) != -1)
    tmpStr.truncate(index);
  spinBox->setValue( tmpStr.toInt() );
}

void KPropSpin::setConfig()
{
  if( ConfigObject )
    {
      ConfigObject->setGroup( Group );
      //		printf("kpropspin: group=%s key=%s\n",ConfigObject->group(), Key.data() );
      if( Key.data() != 0 )
	ConfigObject->writeEntry( Key.data(), getContents() );
      else printf("kpropspin: Null key not allowed\n");
    }
}

void KPropSpin::getConfig()
{
  ConfigObject->setGroup( Group );
  QString valStr = ConfigObject->readEntry( Key.data(), "0" );
  setContents( valStr );
  //	printf("kpropspin: reading config %s = %s\n",Key.data(), s.data() );
}

void KPropSpin::setKey( const char *key )
{
	Key=key;
}

void KPropSpin::setGroup( const char *group )
{
	Group= group;
}

void KPropSpin::setKConfig( KConfig *config )
{
	if( config == 0 )
		ConfigObject = kapp->getConfig();
	else 
		ConfigObject=config;
}

KConfig *KPropSpin::getKConfig()
{
	return ConfigObject;
}

const char *KPropSpin::getKey()
{
	return Key.data();
}

const char *KPropSpin::getGroup()
{
	return Group.data();
}
