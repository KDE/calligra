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
        // First square
        m_leftBorder.setPenWidth( 6 );
        m_leftBorder.color = red;
        m_rightBorder.setPenWidth( 9 );
        m_rightBorder.color = red;
        m_topBorder.setPenWidth( 11 );
        m_topBorder.color = blue;
        m_bottomBorder.setPenWidth( 16 );
        m_bottomBorder.color = green;
        // Second square
        m_doubleBorder.setPenWidth( 2 );
        m_doubleBorder.setStyle( KoBorder::DOUBLE_LINE );
    }

protected:
    virtual void paintEvent( QPaintEvent* )
    {
        QPainter p( this );
        // Lines
        QPen bigPen( black, 8, SolidLine, FlatCap, MiterJoin );
        p.setPen( bigPen );
        p.drawLine( 20, 5, 120, 5 );
        bigPen.setCapStyle( SquareCap );
        p.setPen( bigPen );
        p.drawLine( 20, 20, 120, 20 );
        bigPen.setCapStyle( RoundCap );
        p.setPen( bigPen );
        p.drawLine( 20, 40, 120, 40 );
        // First square
        QRect rect( 20, 80, 100, 100 );
        KoBorder::drawBorders( p, m_zh, rect, m_leftBorder,
                               m_rightBorder, m_topBorder, m_bottomBorder,
                               0, QPen() );
        // Show the corners of the initial rect - they must apppear _inside_ the borders.
        p.setPen( black );
        p.drawPoint( rect.topLeft() );
        p.drawPoint( rect.topRight() );
        p.drawPoint( rect.bottomRight() );
        p.drawPoint( rect.bottomLeft() );
        // Second square
        rect = QRect( 20, 250, 100, 100 );
        KoBorder::drawBorders( p, m_zh, rect, m_doubleBorder,
                               m_doubleBorder, m_doubleBorder, m_doubleBorder,
                               0, QPen() );
    }
    KoZoomHandler* m_zh;
    KoBorder m_leftBorder;
    KoBorder m_rightBorder;
    KoBorder m_topBorder;
    KoBorder m_bottomBorder;
    KoBorder m_doubleBorder;
};

int main (int argc, char ** argv)
{
    KApplication app(argc, argv, "KoBorder test");
    MyWidget* w = new MyWidget;
    w->show();
    return app.exec();
}
