#ifndef __KARBON_PART_H__
#define __KARBON_PART_H__

#include <qlist.h>
#include <koDocument.h>

#include "vlayer.h"

class KarbonPart : public KoDocument
{
// TODO: add wrapper functions to access layers and remove this friendship
	friend class VCanvas;	// drawDocument() has to access m_layers

	Q_OBJECT
public:
	KarbonPart( QWidget* parentWidget = 0, const char* widgetName = 0,
		QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
	virtual ~KarbonPart();

	virtual void paintContent( QPainter& painter, const QRect& rect,
	bool transparent = false, double zoomX = 1.0, double zoomY = 1.0 );

	virtual bool initDoc();

	virtual bool loadXML( QIODevice*, const QDomDocument& );
	virtual QDomDocument saveXML();

	QList<VLayer> &layers() { return m_layers; }
		
protected:
	virtual KoView* createViewInstance( QWidget* parent, const char* name );

private:
	QList<VLayer> m_layers;
};

#endif
