#include <koborder.h>
#include <qwidget.h>
#include <kapplication.h>
#include <qpainter.h>
#include <kozoomhandler.h>

class MyWidget : public QWidget
{
public:
    MyWidget( ) : QWidget( 0L ) {
        m_zh = new KoZoomHandler();
        m_leftBorder.setPenWidth( 5 );
        m_leftBorder.color = red;
        m_rightBorder.setPenWidth( 5 );
        m_rightBorder.color = green;
        m_topBorder.setPenWidth( 5 );
        m_topBorder.color = blue;
        m_bottomBorder.setPenWidth( 5 );
    }

protected:
    virtual void paintEvent( QPaintEvent* )
    {
        QPainter p( this );
        QRect rect( 20, 20, 120, 120 );
        KoBorder::drawBorders( p, m_zh, rect, m_leftBorder,
                               m_rightBorder, m_topBorder, m_bottomBorder,
                               0, QPen() );
    }
    KoZoomHandler* m_zh;
    KoBorder m_leftBorder;
    KoBorder m_rightBorder;
    KoBorder m_topBorder;
    KoBorder m_bottomBorder;
};

int main (int argc, char ** argv)
{
    KApplication app(argc, argv, "KoBorder test");
    MyWidget* w = new MyWidget;
    w->show();
    return app.exec();
}
