#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './vtransformdlg_stub.ui'
**
** Created: Thu Jul 19 20:14:22 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "vtransformdlg_stub.h"

#include <qcombobox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
"12 12 2 1",
". c None",
"# c #000000",
"............",
"............",
"#..........#",
"#..........#",
"#..........#",
"############",
"############",
"#..........#",
"#..........#",
"#..........#",
"............",
"............"};

static const char* const image1_data[] = { 
"12 12 2 1",
". c None",
"# c #000000",
"....####....",
"#..#....##..",
"###.......#.",
"####......#.",
"#####......#",
"###........#",
"#..........#",
"...........#",
"..........#.",
"..........#.",
"..##....##..",
"....####...."};

static const char* const image2_data[] = { 
"12 12 3 1",
". c None",
"# c #000000",
"a c #ffffff",
".....#######",
".....#aaaaa#",
"....#aaaaa#.",
"....#aaaaa#.",
"...#aaaaa#..",
"...#aaaaa#..",
"..#aaaaa#...",
"..#aaaaa#...",
".#aaaaa#....",
".#aaaaa#....",
"#aaaaa#.....",
"#######....."};

static const char* const image3_data[] = { 
"12 12 2 1",
". c None",
"# c #000000",
"............",
"...#...#....",
"....#.#.....",
".....#......",
"....#.#.....",
"...#...#....",
"............",
"............",
"..##....##..",
"############",
".###....###.",
"...#....#..."};

static const char* const image4_data[] = { 
"12 12 2 1",
". c None",
"# c #000000",
".#..........",
".##.........",
"###.........",
"####..#...#.",
".#.....#.#..",
".#.....#.#..",
".#.....#.#..",
".#......#...",
"####....#...",
"###....#....",
".##.........",
".#.........."};

static const char* const image5_data[] = { 
"12 12 4 1",
". c None",
"a c None",
"# c #000000",
"b c #ffffff",
"............",
"............",
"#aaaaaaaaaa#",
"#aaaaaaaaaa#",
"#a#a#a#a#a##",
"#a#b#a#a#a##",
"##a#a#a#a#a#",
"##a#a#a#a#a#",
"#aaaaaaaaaa#",
"#aaaaaaaaaa#",
"............",
"............"};

static const char* const image6_data[] = { 
"12 12 4 1",
". c None",
"a c None",
"# c #000000",
"b c #ffffff",
"..########..",
"..aaaa##aa..",
"..aa##baaa..",
"..aaaa##aa..",
"..aa##aaaa..",
"..aaaa##aa..",
"..aa##aaaa..",
"..aaaa##aa..",
"..aa##aaaa..",
"..aaaa##aa..",
"..aa##aaaa..",
"..########.."};


/* 
 *  Constructs a VTransformDlg which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
VTransformDlg::VTransformDlg( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    QPixmap image0( ( const char** ) image0_data );
    QPixmap image1( ( const char** ) image1_data );
    QPixmap image2( ( const char** ) image2_data );
    QPixmap image3( ( const char** ) image3_data );
    QPixmap image4( ( const char** ) image4_data );
    QPixmap image5( ( const char** ) image5_data );
    QPixmap image6( ( const char** ) image6_data );
    if ( !name )
	setName( "Form3" );
    resize( 245, 159 ); 
    setCaption( i18n( "Form3" ) );

    PixmapLabel6_2 = new QLabel( this, "PixmapLabel6_2" );
    PixmapLabel6_2->setGeometry( QRect( 160, 25, 12, 12 ) ); 
    PixmapLabel6_2->setFrameShape( QLabel::MShape );
    PixmapLabel6_2->setFrameShadow( QLabel::MShadow );
    PixmapLabel6_2->setPixmap( image0 );
    PixmapLabel6_2->setScaledContents( TRUE );

    TabWidget2 = new QTabWidget( this, "TabWidget2" );
    TabWidget2->setGeometry( QRect( 0, 0, 252, 160 ) ); 

    tab = new QWidget( TabWidget2, "tab" );

    xpos = new QLineEdit( tab, "xpos" );
    xpos->setGeometry( QRect( 24, 4, 90, 22 ) ); 

    width = new QLineEdit( tab, "width" );
    width->setGeometry( QRect( 148, 4, 90, 22 ) ); 

    ypos = new QLineEdit( tab, "ypos" );
    ypos->setGeometry( QRect( 24, 32, 90, 22 ) ); 

    height = new QLineEdit( tab, "height" );
    height->setGeometry( QRect( 148, 32, 90, 22 ) ); 

    Line1 = new QFrame( tab, "Line1" );
    Line1->setGeometry( QRect( 6, 62, 235, 2 ) ); 
    Line1->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    rotatepix = new QLabel( tab, "rotatepix" );
    rotatepix->setGeometry( QRect( 6, 75, 12, 12 ) ); 
    rotatepix->setPixmap( image1 );
    rotatepix->setScaledContents( TRUE );

    skewpix = new QLabel( tab, "skewpix" );
    skewpix->setGeometry( QRect( 6, 103, 12, 12 ) ); 
    skewpix->setPixmap( image2 );
    skewpix->setScaledContents( TRUE );

    rotatecombo = new QComboBox( FALSE, tab, "rotatecombo" );
    rotatecombo->setGeometry( QRect( 24, 70, 90, 22 ) ); 

    skewcombo = new QComboBox( FALSE, tab, "skewcombo" );
    skewcombo->setGeometry( QRect( 24, 99, 90, 22 ) ); 

    xpospix = new QLabel( tab, "xpospix" );
    xpospix->setGeometry( QRect( 6, 11, 12, 12 ) ); 
    xpospix->setPixmap( image3 );
    xpospix->setScaledContents( TRUE );

    ypospix = new QLabel( tab, "ypospix" );
    ypospix->setGeometry( QRect( 6, 37, 12, 12 ) ); 
    ypospix->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, ypospix->sizePolicy().hasHeightForWidth() ) );
    ypospix->setPixmap( image4 );
    ypospix->setScaledContents( TRUE );

    widthpix = new QLabel( tab, "widthpix" );
    widthpix->setGeometry( QRect( 131, 9, 12, 12 ) ); 
    widthpix->setPixmap( image5 );
    widthpix->setScaledContents( TRUE );

    heightpix = new QLabel( tab, "heightpix" );
    heightpix->setGeometry( QRect( 130, 37, 12, 12 ) ); 
    heightpix->setPixmap( image6 );
    heightpix->setScaledContents( TRUE );
    TabWidget2->insertTab( tab, i18n( "Transform" ) );

    tab_2 = new QWidget( TabWidget2, "tab_2" );
    TabWidget2->insertTab( tab_2, i18n( "Arrange" ) );

    tab_3 = new QWidget( TabWidget2, "tab_3" );
    TabWidget2->insertTab( tab_3, i18n( "Paths" ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
VTransformDlg::~VTransformDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

#include "vtransformdlg_stub.moc"
