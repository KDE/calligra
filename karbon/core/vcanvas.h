#ifndef __VCANVAS_H__
#define __VCANVAS_H__

#include <qscrollview.h>

class KoRuler;
class KarbonPart;
class KarbonView;

class VCanvas : public QScrollView {
    Q_OBJECT
public:
    VCanvas( KarbonView* view, KarbonPart* part );

protected:
    virtual void paintEvent( QPaintEvent* event );
    virtual void resizeEvent( QResizeEvent* event );

private:
    KarbonPart* m_part;
    KarbonView* m_view;
    KoRuler* m_vRuler;
    KoRuler* m_hRuler;
};

#endif
