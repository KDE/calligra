#ifndef __KARBON_VIEW__
#define __KARBON_VIEW__

#include <koView.h>

#include "vcanvas.h"	// necessary because of canvas()

class KAction;
class KSelectAction;
class QPaintEvent;

class KarbonPart;

class KarbonView : public KoView
{
	Q_OBJECT
public:
	KarbonView( KarbonPart* part, QWidget* parent = 0, const char* name = 0 );
	virtual ~KarbonView();

	virtual void paintEverything( QPainter &p, const QRect &rect, bool transparent = false );

	virtual QWidget* canvas() { return m_canvas; }

protected slots:
	void cut();

protected:
	virtual void updateReadWrite( bool rw );
	virtual void resizeEvent( QResizeEvent* event );

private:
	void initActions();

	KarbonPart* m_part;
	VCanvas* m_canvas;

	KSelectAction* m_zoomAction;
};

#endif
