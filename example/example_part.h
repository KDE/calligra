#ifndef EXAMPLE_PART_H
#define EXAMPLE_PART_H

#include <koDocument.h>

class ExamplePart : public KoDocument
{
    Q_OBJECT
public:
    ExamplePart( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );

    virtual KoView* createView( QWidget* parent = 0, const char* name = 0 );
    virtual KoMainWindow* createShell();

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE );

    virtual bool initDoc();

    virtual QCString mimeType() const;
};

#endif
