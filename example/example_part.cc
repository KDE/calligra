#include "example_part.h"
#include "example_view.h"
#include "example_shell.h"

#include <qpainter.h>

ExamplePart::ExamplePart( KoDocument* parent, const char* name )
    : KoDocument( parent, name )
{
}
 
bool ExamplePart::initDoc()
{
    // If nothing is loaded, do initialize here
    return TRUE;
}

QCString ExamplePart::mimeType() const
{
    return "application/x-example";
}

View* ExamplePart::createView( QWidget* parent, const char* name )
{
    ExampleView* view = new ExampleView( this, parent, name );
    addView( view );

    return view;
}

Shell* ExamplePart::createShell()
{
    Shell* shell = new ExampleShell;
    shell->setRootPart( this );
    shell->show();

    return shell;}

void ExamplePart::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/ )
{
    // ####### handle transparency
    
    // Need to draw only the document rectangle described in the parameter rect.
    int left = rect.left() / 20;
    int right = rect.right() / 20 + 1;
    int top = rect.top() / 20;
    int bottom = rect.bottom() / 20 + 1;
    
    for( int x = left; x < right; ++x )
	painter.drawLine( x * 20, top * 20, x * 20, bottom * 20 );
    for( int y = left; y < right; ++y )
	painter.drawLine( left * 20, y * 20, right * 20, y * 20 );
}

QString ExamplePart::configFile() const
{
    return readConfigFile( "example.rc" );
}

#include "example_part.moc"
