
#include "kpropradio.h"
#include "kpropradio.moc"
#include <kapp.h>

KPropRadio::KPropRadio( QWidget *parent, const char *text, const char *key, const char *group, KConfig *config, const char *name )
	: QButtonGroup( text, parent,name )
{
	State = -1;

	setKConfig( config );
	setGroup( group );
	setKey( key );

	
//	printf("update size...\n");

	radioList = new QList<QRadioButton>;
	radioList->setAutoDelete(TRUE);

	setFontPropagation( QWidget::SameFont );

	layout = new QVBoxLayout( this, 16,1 );
}

KPropRadio::~KPropRadio()
{
	radioList->clear();
	delete radioList;
}

void KPropRadio::sizeUpdate()
{
	QFontMetrics fm( font() );
	int h=fm.height();
	
	setFixedHeight( (radioList->count() + 1) * (h+8) );
}

void KPropRadio::resizeEvent( QResizeEvent * )
{}

void KPropRadio::fontChange( const QFont & )
{
	sizeUpdate();
}

void KPropRadio::setState( int state )
{
	if( radioList->at(state) )
	{
		radioList->current()->setChecked( TRUE );
		State = state;
	}
}

int KPropRadio::getState()
{
	for( radioList->first(); radioList->current(); radioList->next() )
		if( radioList->current()->isChecked() )
			State = radioList->at();
	
	return State;
}

QRadioButton * KPropRadio::addRadio( const char *text, bool state )
{
        QRadioButton *r = new QRadioButton(text, this);
	radioList->append( r );
	layout->addWidget( radioList->current() );
	radioList->current()->setChecked( state );

	sizeUpdate();
        return r;
}

void KPropRadio::setConfig()
{
	if( ConfigObject )
	{
		ConfigObject->setGroup( Group );
//		printf("kpropradio: group=%s key=%s\n",ConfigObject->group(), Key.data() );
		if( Key.data() != 0 )
			ConfigObject->writeEntry( Key.data(), getState() );
		else printf("kpropradio: Null key not allowed\n");
	}
}

void KPropRadio::getConfig()
{
	ConfigObject->setGroup( Group );
	setState( ConfigObject->readNumEntry( Key.data() ) );
}

void KPropRadio::setKey( const char *key )
{
	Key=key;
}

void KPropRadio::setGroup( const char *group )
{
	Group= group;
}

void KPropRadio::setKConfig( KConfig *config )
{
	if( config == 0 )
		ConfigObject = kapp->getConfig();
	else 
		ConfigObject=config;
}

KConfig *KPropRadio::getKConfig()
{
	return ConfigObject;
}

const char *KPropRadio::getKey()
{
	return Key.data();
}

const char *KPropRadio::getGroup()
{
	return Group.data();
}
