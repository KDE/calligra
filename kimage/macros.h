#define TEXT( text ) Q2C( i18n( text ) )
#define PIX( pix ) *OPUIUtils::convertPixmap( ICON( pix ) )

#define MENU( menu, text ) _menubar->insertMenu( TEXT( text ), menu, -1, -1 );
#define ITEM1( id, menu, text, func ) id = menu->insertItem( TEXT( text ), this, func, 0, -1, -1 );
#define ITEM2( id, menu, text, func, key ) id = menu->insertItem4( TEXT( text ), this, func, key, -1, -1 );
#define ITEM3( id, menu, pix, text, func ) id = menu->insertItem6( PIX( pix ), TEXT( text ), this, func, 0, -1, -1 );
#define ITEM4( id, menu, pix, text, func, key ) id = menu->insertItem6( PIX( pix ), TEXT( text ), this, func, key, -1, -1 );
#define MN_SEPARATOR( menu ) menu->insertSeparator( -1 );

#define TOOLBAR( bar ) bar = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
#define BUTTON1( id, bar, pix, num, func, text ) id = bar->insertButton2( PIX( pix ), num, SIGNAL( clicked() ), this, func, true, TEXT( text ), -1 );
#define TB_SEPARATOR( bar ) bar->insertSeparator( -1 );
