#ifndef __KARBON_PART_H__
#define __KARBON_PART_H__

#include <qlist.h>
#include <koDocument.h>

#include "vobject.h"

class KarbonPart : public KoDocument
{
    Q_OBJECT
public:
    KarbonPart( QWidget* parentWidget = 0, const char* widgetName = 0,
	QObject* parent = 0, const char* name = 0, bool singleViewMode = false );

    virtual void paintContent( QPainter& painter, const QRect& rect,
	bool transparent = false );

    virtual bool initDoc();

    virtual bool loadXML( QIODevice*, const QDomDocument& );
    virtual QDomDocument saveXML();

protected:
    virtual KoView* createViewInstance( QWidget* parent, const char* name );

private:
    QList<VObject> m_objects; // all objects
};

#endif
