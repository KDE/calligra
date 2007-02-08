/* This file is part of the KDE project
   Copyright (C) 2003 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2005 Rob Buis <buis@kde.org>
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#if 0

// 8x8 pixels

static const char* const buttonnone[]={
"8 8 7 1",
"c c #0a0000",
". c #0a0a0a",
"b c #330000",
"a c #331f1f",
"# c #333333",
"e c #ff0000",
"d c #ffffff",
".####abc",
"#ddddeeb",
"#dddeeea",
"#ddeeed#",
"#deeedd#",
"aeeeddd#",
"beedddd#",
"cba####."};

static const char* const buttonsolid[]={
"8 8 1 1",
". c #000000",
"........",
"........",
"........",
"........",
"........",
"........",
"........",
"........"};

static const char* const buttongradient[]={
"8 8 14 1",
"f c #000000",
"e c #040404",
". c #0a0a0a",
"d c #0f0f0f",
"c c #181818",
"b c #212121",
"a c #292929",
"# c #303030",
"g c #333333",
"l c #484848",
"k c #787878",
"j c #a7a7a7",
"i c #cdcdcd",
"h c #f1f1f1",
".#abcdef",
"ghijkl#f",
"ghijkl#f",
"ghijkl#f",
"ghijkl#f",
"ghijkl#f",
"ghijkl#f",
".#abcdef"};

static const char* const buttonpattern[]={
"8 8 4 1",
". c #0a0a0a",
"# c #333333",
"a c #a0a0a0",
"b c #ffffff",
".######.",
"#aabbaa#",
"#aabbaa#",
"#bbaabb#",
"#bbaabb#",
"#aabbaa#",
"#aabbaa#",
".######."};

#else

// 16x16 pixels

static const char* const buttonnone[]={
"16 16 7 1",
"c c #0a0000",
". c #0a0a0a",
"b c #330000",
"a c #331f1f",
"# c #333333",
"e c #ff0000",
"d c #ffffff",
"..########aabbcc",
"..########aabbcc",
"##dddddddddeeebb",
"##ddddddddeeeebb",
"##dddddddeeeeeaa",
"##ddddddeeeeedaa",
"##dddddeeeeedd##",
"##ddddeeeeeddd##",
"##dddeeeeedddd##",
"##ddeeeeeddddd##",
"aaeeeeeedddddd##",
"aaeeeeeddddddd##",
"bbeeeedddddddd##",
"bbeeeddddddddd##",
"ccbbaa########..",
"ccbbaa########.."};

static const char* const buttonsolid[]={
"16 16 1 1",
". c #000000",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................"};


// FIXME: Smoother gradient button.

static const char* const buttongradient[]={
"16 16 14 1",
"f c #000000",
"e c #040404",
". c #0a0a0a",
"d c #0f0f0f",
"c c #181818",
"b c #212121",
"a c #292929",
"# c #303030",
"g c #333333",
"l c #484848",
"k c #787878",
"j c #a7a7a7",
"i c #cdcdcd",
"h c #f1f1f1",
"..##aabbccddeeff",
"..##aabbccddeeff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"..##aabbccddeeff",
"..##aabbccddeeff"};

static const char* const buttonpattern[]={
"16 16 4 1",
". c #0a0a0a",
"# c #333333",
"a c #a0a0a0",
"b c #ffffffff",
"..############..",
"..############..",
"##aaaabbbbaaaa##",
"##aaaabbbbaaaa##",
"##aaaabbbbaaaa##",
"##aaaabbbbaaaa##",
"##bbbbaaaabbbb##",
"##bbbbaaaabbbb##",
"##bbbbaaaabbbb##",
"##bbbbaaaabbbb##",
"##aaaabbbbaaaa##",
"##aaaabbbbaaaa##",
"##aaaabbbbaaaa##",
"##aaaabbbbaaaa##",
"..############..",
"..############.."};

#endif


#include <qpixmap.h>
#include <qtoolbutton.h>
#include <QToolTip>
#include <QButtonGroup>
#include <QGridLayout>

#include <klocale.h>

#include <KoCanvasController.h>
#include <KoToolManager.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoShapeBackgroundCommand.h>
#include <KoShapeBorderCommand.h>
#include <KoCanvasResourceProvider.h>
#include <KoShapeBorderModel.h>
#include <KoLineBorder.h>
#include <KoColor.h>

#include "karbon_part.h"
#include "vtypebuttonbox.h"

VTypeButtonBox::VTypeButtonBox( QWidget* parent )
    : QGroupBox( parent ), m_isStrokeManipulator( false )
{
    QGridLayout * layout = new QGridLayout( this );
    QButtonGroup * group = new QButtonGroup( this );

    // The button for no fill
    QToolButton* button = new QToolButton( this );
    button->setIcon( QPixmap( (const char **) buttonnone ) );
    button->setMinimumSize( 14, 14 );
    button->setToolTip( i18n( "None" ) );
    group->addButton( button, None );
    layout->addWidget( button, 0, 0 );

    // The button for solid fill
    button = new QToolButton( this );
    button->setIcon( QPixmap( (const char **) buttonsolid ) );
    button->setMinimumSize( 14, 14 );
    button->setToolTip( i18n( "Solid" ) );
    group->addButton( button, Solid );
    layout->addWidget( button, 0, 1 );

    // The button for gradient fill
    button = new QToolButton( this );
    button->setIcon( QPixmap( (const char **) buttongradient ) );
    button->setMinimumSize( 14, 14 );
    button->setToolTip( i18n( "Gradient" ) );
    group->addButton( button, Gradient );
    layout->addWidget( button, 1, 0 );

    // The button for pattern fill
    button = new QToolButton( this );
    button->setIcon( QPixmap( (const char **) buttonpattern ) );
    button->setMinimumSize( 14, 14 );
    button->setToolTip( i18n( "Pattern" ) );
    group->addButton( button, Pattern );
    layout->addWidget( button, 1, 1 );

    layout->setMargin( 1 );
    layout->setSpacing( 1 );
    layout->setColumnStretch( 0, 1 );
    layout->setColumnStretch( 1, 1 );

    connect( group, SIGNAL( buttonClicked( int ) ), this, SLOT( slotButtonPressed( int ) ) );
}

void VTypeButtonBox::slotButtonPressed( int id )
{
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    if( ! selection || ! selection->count() )
        return;

    if ( m_isStrokeManipulator )
        manipulateStrokes( id );
    else
        manipulateFills( id );
}

void VTypeButtonBox::setStroke()
{
    m_isStrokeManipulator = true;
}

void VTypeButtonBox::setFill()
{
    m_isStrokeManipulator = false;
}

bool VTypeButtonBox::isStrokeManipulator() const
{
    return m_isStrokeManipulator;
}

void VTypeButtonBox::manipulateFills( int id )
{
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();

    // TODO get the actual background from the ResourceProvider
    QBrush fill;

    switch( id )
    {
        case None:
            fill = QBrush();
            break;
        case Solid:
            fill = QBrush( canvasController->canvas()->resourceProvider()->backgroundColor().toQColor() );
            break;
        case Gradient:
            //m_fill.setType( VFill::grad );
            break;
        case Pattern:
            //m_fill.setType( VFill::patt );
            break;
    }
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    canvasController->canvas()->addCommand( new KoShapeBackgroundCommand( selection->selectedShapes(), fill ) );
}

void VTypeButtonBox::manipulateStrokes( int id )
{
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    // TODO get the actual border stroke from the ResourceProvider
    KoShapeBorderModel * border = 0;
    switch( id )
    {
        case None:
        break;
        case Solid:
        {
            QColor color = canvasController->canvas()->resourceProvider()->backgroundColor().toQColor();
            border = new KoLineBorder( 1.0, color );
            break;
        }
        case Gradient:
            //m_stroke.setType( VStroke::grad );
            break;
        case Pattern:
            //m_stroke.setType( VStroke::patt );
            break;
    }
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    canvasController->canvas()->addCommand( new KoShapeBorderCommand( selection->selectedShapes(), border ) );
}

#include "vtypebuttonbox.moc"

