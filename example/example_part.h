#ifndef EXAMPLE_PART_H
#define EXAMPLE_PART_H

#include <koDocument.h>

class ExamplePart : public KoDocument
{
    Q_OBJECT
public:
    ExamplePart( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );

    virtual KoMainWindow* createShell();

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE );

    virtual bool initDoc();

    virtual bool loadXML( const QDomDocument & );
    virtual QDomDocument saveXML();

    virtual QCString mimeType() const;

protected:
    virtual KoView* createViewInstance( QWidget* parent, const char* name );
};

#endif
