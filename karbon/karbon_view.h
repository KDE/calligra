#ifndef __KARBON_VIEW__
#define __KARBON_VIEW__

#include <koView.h>

#include "vcanvas.h"	// necessary because of canvas()

class KAction;
class QPaintEvent;

class KarbonPart;

class KarbonView : public KoView
{
    Q_OBJECT
public:
    KarbonView( KarbonPart* part, QWidget* parent = 0, const char* name = 0 );
    virtual ~KarbonView();

    virtual QWidget* canvas() { return m_canvas; }

protected slots:
    void cut();

protected:
    virtual void updateReadWrite( bool rw );
    virtual void resizeEvent( QResizeEvent* event );    

private:
    void initActions();
    
    VCanvas* m_canvas;
};

#endif
