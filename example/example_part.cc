#include "example_part.h"
#include "example_view.h"
#include "example_shell.h"

#include <qpainter.h>

ExamplePart::ExamplePart( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
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

KoView* ExamplePart::createViewInstance( QWidget* parent, const char* name )
{
    return new ExampleView( this, parent, name );
}

KoMainWindow* ExamplePart::createShell()
{
    KoMainWindow* shell = new ExampleShell;
    shell->setRootDocument( this );
    shell->show();

    return shell;
}

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

#include "example_part.moc"
