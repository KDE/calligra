#ifndef EXAMPLE_PART_H
#define EXAMPLE_PART_H

#include <koDocument.h>

class ExamplePart : public KoDocument
{
    Q_OBJECT
public:
    ExamplePart( KoDocument* parent = 0, const char* name = 0 );
    
    virtual View* createView( QWidget* parent = 0, const char* name = 0 );
    virtual Shell* createShell();

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE );

    virtual bool initDoc();

    virtual QCString mimeType() const;
    
protected:
    virtual QString configFile() const;
};

#endif
