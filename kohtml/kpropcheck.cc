
#include "kpropcheck.h"
#include "kpropcheck.moc"
#include <kapp.h>

KPropCheck::KPropCheck( QWidget *parent, const char *text, const char *key, const char *group, KConfig *config, const char *name )
	: QCheckBox( text, parent,name )
{
	setKConfig( config );
	setGroup( group );
	setKey( key );

	sizeUpdate();
}

KPropCheck::~KPropCheck()
{}

void KPropCheck::sizeUpdate()
{
	QFontMetrics fm( font() );
	int h=fm.height();
	
	setFixedHeight( h + 8 );
}

void KPropCheck::fontChange( const QFont & )
{
	sizeUpdate();
}

void KPropCheck::setState( bool state )
{
	setChecked( state );
}

bool KPropCheck::getState()
{
	return isChecked();
}

void KPropCheck::setConfig()
{
	if( ConfigObject )
	{
		ConfigObject->setGroup( Group );
		//		printf("kpropcheck: group=%s key=%s\n",ConfigObject->group(), Key.data() );
		if( Key.data() != 0 )
			ConfigObject->writeEntry( Key.data(), getState() );
		else printf("kpropcheck: Null key not allowed\n");
	}
}

void KPropCheck::getConfig()
{
	ConfigObject->setGroup( Group );
	setState( ConfigObject->readBoolEntry( Key.data() ) );
	//	printf("kpropcheck:state=%d\n", getState() );
}


void KPropCheck::setKey( const char *key )
{
	Key=key;
}

void KPropCheck::setGroup( const char *group )
{
	Group= group;
}

void KPropCheck::setKConfig( KConfig *config )
{
	if( config == 0 )
		ConfigObject = kapp->getConfig();
	else 
		ConfigObject=config;
	
}

KConfig *KPropCheck::getKConfig()
{
	return ConfigObject;
}

const char *KPropCheck::getKey()
{
	return Key.data();
}

const char *KPropCheck::getGroup()
{
	return Group.data();
}
