#include "kspread_map.h"

#include <time.h>
#include <qmsgbox.h>
#include <stdio.h>
#include <stdlib.h>

KSpreadMap::KSpreadMap( KSpreadDoc *_doc )
{
    m_pDoc = _doc;
    
    m_lstTables.setAutoDelete( true );

    m_strPythonCode = "";
    m_bPythonCodeInFile = FALSE;
}

KSpreadMap::~KSpreadMap()
{
}

void KSpreadMap::addTable( KSpreadTable *_table )
{
    m_lstTables.append( _table );
}

void KSpreadMap::removeTable( KSpreadTable *_table )
{
    m_lstTables.setAutoDelete( false );
    m_lstTables.removeRef( _table );
    m_lstTables.setAutoDelete( true );
}

/*
OBJECT KSpreadMap::save( KorbSession *korb, OBJECT o_map )
{
    printf("Saving map ....\n");

    QDataStream stream;

    // Real types
    TYPE t_m_lstTables =  korb->registerType( "KDE:kxcl:KSpreadm_lstTables" );

    // Properties
    PROPERTY p_tables = korb->registerProperty( "KDE:kxcl:KSpreadTables" );
    PROPERTY p_leftborder = korb->registerProperty( "KDE:kxcl:LeftBorder" );
    PROPERTY p_rightborder = korb->registerProperty( "KDE:kxcl:RightBorder" );
    PROPERTY p_topborder = korb->registerProperty( "KDE:kxcl:TopBorder" );
    PROPERTY p_bottomborder = korb->registerProperty( "KDE:kxcl:BottomBorder" );
    PROPERTY p_papersize = korb->registerProperty( "KDE:kxcl:PaperSize" );
    PROPERTY p_paperorientation = korb->registerProperty( "KDE:kxcl:PaperOrientation" );
    PROPERTY p_m_headLeft = korb->registerProperty( "KDE:kxcl:headLeft" );
    PROPERTY p_m_headMid = korb->registerProperty( "KDE:kxcl:headMid" );
    PROPERTY p_m_headRight = korb->registerProperty( "KDE:kxcl:headRight" );
    PROPERTY p_footLeft = korb->registerProperty( "KDE:kxcl:footLeft" );
    PROPERTY p_footMid = korb->registerProperty( "KDE:kxcl:footMid" );
    PROPERTY p_footRight = korb->registerProperty( "KDE:kxcl:footRight" );
    PROPERTY p_code = korb->registerProperty( "KDE:kxcl:PythonCode" );

    // A list of all table object ids.
    QStack<OBJECT> tableStack;
    tableStack.setAutoDelete( TRUE );

    KSpreadTable *t;
    for ( t = m_lstTables.first(); t != 0L; t = m_lstTables.next() )
    {
	OBJECT *o_table = new OBJECT( t->save( korb ) );
	
	if ( *o_table == 0 )
	    return 0;
	
	tableStack.push( o_table );
    }
    
    // Write the map
    VALUE value = korb->newValue( o_map, p_tables, t_m_lstTables );
    KorbDevice *device = korb->getDeviceForValue( value );
    stream.setDevice( device );
    stream << (UINT32)m_lstTables.count();
    while ( !tableStack.isEmpty() )
	stream << *( tableStack.pop() );
    stream.unsetDevice();
    korb->release( device );
    delete device;

    // Write the paper metrics
    korb->writeFloatValue( o_map, p_leftborder, xclPart->getLeftBorder() );
    korb->writeFloatValue( o_map, p_rightborder, xclPart->getRightBorder() );
    korb->writeFloatValue( o_map, p_topborder, xclPart->getTopBorder() );
    korb->writeFloatValue( o_map, p_bottomborder, xclPart->getBottomBorder() );
    korb->writeStringValue( o_map, p_papersize, xclPart->paperFormatString() );
    korb->writeStringValue( o_map, p_paperorientation, xclPart->orientationString() );
    korb->writeStringValue( o_map, p_m_headLeft, xclPart->getHeadLeft() );
    korb->writeStringValue( o_map, p_m_headMid, xclPart->getHeadMid() );
    korb->writeStringValue( o_map, p_m_headRight, xclPart->getHeadRight() );
    korb->writeStringValue( o_map, p_footLeft, xclPart->getFootLeft() );
    korb->writeStringValue( o_map, p_footMid, xclPart->getFootMid() );
    korb->writeStringValue( o_map, p_footRight, xclPart->getFootRight() );
    if ( !pythonCode.isNull() && pythonCode.length() > 0 )
	korb->writeStringValue( o_map, p_code, pythonCode.data() );
    
    printf("Closing .....\n");
    
    return o_map;
}

bool KSpreadMap::load( KorbSession *korb, OBJECT o_map )
{
    QDataStream stream;

    // Real types
    TYPE t_m_lstTables =  korb->findType( "KDE:kxcl:KSpreadm_lstTables" );

    PROPERTY p_tables = korb->findProperty( "KDE:kxcl:KSpreadTables" );
    PROPERTY p_leftborder = korb->findProperty( "KDE:kxcl:LeftBorder" );
    PROPERTY p_rightborder = korb->findProperty( "KDE:kxcl:RightBorder" );
    PROPERTY p_topborder = korb->findProperty( "KDE:kxcl:TopBorder" );
    PROPERTY p_bottomborder = korb->findProperty( "KDE:kxcl:BottomBorder" );
    PROPERTY p_papersize = korb->findProperty( "KDE:kxcl:PaperSize" );
    PROPERTY p_paperorientation = korb->findProperty( "KDE:kxcl:PaperOrientation" );
    PROPERTY p_m_headLeft = korb->findProperty( "KDE:kxcl:headLeft" );
    PROPERTY p_m_headMid = korb->findProperty( "KDE:kxcl:headMid" );
    PROPERTY p_m_headRight = korb->findProperty( "KDE:kxcl:headRight" );
    PROPERTY p_footLeft = korb->findProperty( "KDE:kxcl:footLeft" );
    PROPERTY p_footMid = korb->findProperty( "KDE:kxcl:footMid" );
    PROPERTY p_footRight = korb->findProperty( "KDE:kxcl:footRight" );
    PROPERTY p_code = korb->findProperty( "KDE:kxcl:PythonCode" );

    printf("Processed Properties\n");
    
    // Check if all really needed property and type keys could be found.
    // If not, raise an error
    if ( p_tables == 0L )
    {
	printf("ERROR: Could not find Property KDE:kxcl:KSpreadTables \n");
	return FALSE;
    }
    if ( t_m_lstTables == 0L )
    {
	printf("ERROR: Could not find Type KDE:kxcl:KSpreadm_lstTables \n");
	return FALSE;
    }
    if ( p_leftborder == 0L || p_rightborder == 0L || p_bottomborder == 0L || p_topborder == 0L )
    {
	printf("ERROR: Could not find Property KDE:kxcl:xxxxBorder \n");
	return FALSE;
    }
    if ( p_papersize == 0L )
    {
	printf("ERROR: Could not find Property KDE:kxcl:PaperSize\n");
	return FALSE;
    }
    if ( p_paperorientation == 0L )
    {
	printf("ERROR: Could not find Property KDE:kxcl:PaperOrientation\n");
	return FALSE;
    }
    if ( p_m_headLeft == 0L || p_m_headMid == 0L || p_m_headRight == 0L ||
	 p_footLeft == 0L || p_footMid == 0L || p_footRight == 0L )
    {
	printf("ERROR: Could not find head/foot Property\n");
	return FALSE;
    }
    
    // Find the list of tables in the map
    VALUE value = korb->findValue( o_map, p_tables, t_m_lstTables );
    if ( value == 0L )
    {
	printf("Error: Error in Map\n");
	return FALSE;
    }

    printf("Loading paper size\n");
    
    float l,r,b,t;
    if ( !korb->readFloatValue( o_map, p_leftborder, l ) ||
	 !korb->readFloatValue( o_map, p_rightborder, r ) ||
	 !korb->readFloatValue( o_map, p_bottomborder, b ) ||
	 !korb->readFloatValue( o_map, p_topborder, t ) )
    {
	printf("Error: Could not load paper border information\n");
	return FALSE;
    }
    
    QString orientation;
    orientation = korb->readStringValue( o_map, p_paperorientation ).copy();
    if ( orientation.isNull() )
    {
	printf("Error: Could not load paper orientation information\n");
	return FALSE;
    }

    QString format;
    format = korb->readStringValue( o_map, p_papersize ).copy();
    if ( format.isNull() )
    {
	printf("Error: Could not load paper format information\n");
	return FALSE;
    }

    QString hl = korb->readStringValue( o_map, p_m_headLeft ).copy();
    QString hm = korb->readStringValue( o_map, p_m_headMid ).copy();
    QString hr = korb->readStringValue( o_map, p_m_headRight ).copy();
    QString fl = korb->readStringValue( o_map, p_footLeft ).copy();
    QString fm = korb->readStringValue( o_map, p_footMid ).copy();
    QString fr = korb->readStringValue( o_map, p_footRight ).copy();
    xclPart->setHeadFootLine( hl, hm, hr, fl, fm, fr );
    
    printf("Done size information\n");

    if ( p_code )
	pythonCode = korb->readStringValue( o_map, p_code ).copy();

    // Read the list of tables
    KorbDevice *dev = korb->getDeviceForValue( value );
    stream.setDevice( dev );
    UINT32 ctables;
    stream >> ctables;
    printf("Loading %i tables\n",ctables);
    
    for ( UINT32 i = 0; i < ctables; i++ )
    {
	OBJECT o_table;
	stream >> o_table;
	printf("Loading table id %i\n",o_table);

	KSpreadTable *t = xclPart->newTable();
	t->setMap( this );
	
	printf("Loading new table .....\n");
	
	if ( !t->load( korb, o_table ) )
	{
	    stream.unsetDevice();
	    korb->release( dev );
	    delete dev;
	    return FALSE;
	}
	printf(".... Loaded table\n");
	
	m_lstTables.insert( 0, t );
    }

    xclPart->setPaperKSpreadLayout( l, t, r, b, format.data(), orientation.data() );

    stream.unsetDevice();
    korb->release( dev );
    delete dev;
    
    return TRUE;
}
*/

KSpreadTable* KSpreadMap::findTable( const char *_name )
{
    KSpreadTable *t;
 
    for ( t = m_lstTables.first(); t != 0L; t = m_lstTables.next() )
    {
	if ( strcmp( _name, t->name() ) == 0 )
	    return t;
    }

    return 0L;
}

/*    
void KSpreadMap::initAfterLoading()
{
    KSpreadTable *ta;
    for ( ta = m_lstTables.first(); ta != 0L; ta = m_lstTables.next() )
	ta->initAfterLoading();
}
*/

bool KSpreadMap::movePythonCodeToFile()
{
    if ( m_bPythonCodeInFile )
	return TRUE;
    QString d;
    d.sprintf( "%s/.koffice/xcl/tmp/script%i", getenv( "HOME" ), time( 0L ) );
    FILE *f = fopen( d, "w" );
    if ( f == 0L )
    {
      // HACK
      // QMessageBox::message( "Kxcl Error", "Could not write to\n~/.koffice/xcl/tmp" );
	return FALSE;
    }
    fwrite( m_strPythonCode.data(), 1, m_strPythonCode.length(), f );
    fclose( f );
    m_strPythonCodeFile = d.data();

    m_bPythonCodeInFile = TRUE;

    return TRUE;
}

bool KSpreadMap::getPythonCodeFromFile()
{
    if ( !m_bPythonCodeInFile )
	return TRUE;
    
    FILE *f = fopen( m_strPythonCodeFile, "r" );
    if ( f == 0L )
    {
	QMessageBox::message( "Kxcl Error", "Could not read from\n~/.koffice/xcl/tmp" );
	return FALSE;
    }
    char buffer[ 4096 ];
    m_strPythonCode = "";
    while ( !feof( f ) )
    {
	int n = fread( buffer, 1, 4095, f );
	if ( n > 0 )
	{
	    buffer[n] = 0;
	    m_strPythonCode += buffer;
	}
    }
    fclose( f );
    
    m_bPythonCodeInFile = FALSE;

    return TRUE;
}
