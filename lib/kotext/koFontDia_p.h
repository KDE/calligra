#ifndef KOFONTDIA_P_H
#define KOFONTDIA_P_H

#include <qframe.h>
class QSpinBox;
class QPushButton;
class KColorButton;
class QLabel;

class KoShadowPreview : public QFrame
{
    Q_OBJECT

public:
    // constructor - destructor
    KoShadowPreview( QWidget* parent, const char* );
    ~KoShadowPreview() {}

    void setShadowDistanceX( double sd ) { shadowDistanceX = sd; repaint( true ); }
    void setShadowDistanceY( double sd ) { shadowDistanceY = sd; repaint( true ); }
    void setShadowColor( QColor sc ) { shadowColor = sc; repaint( true ); }

protected:
    void drawContents( QPainter* );

    double shadowDistanceX;
    double shadowDistanceY;
    QColor shadowColor;
};


class KoTextShadowWidget: public QWidget
{
    Q_OBJECT
public:
    KoTextShadowWidget( QWidget * parent, const char * name=0 );
    virtual ~KoTextShadowWidget() {}

    QString tabName();

    void setShadow( double shadowDistanceX, double shadowDistanceY, const QColor& shadowColor );

    double shadowDistanceX() const;
    double shadowDistanceY() const;
    QColor shadowColor() const { return m_shadowColor; }

signals:
    void changed();

protected slots:
    void luChanged();
    void uChanged();
    void ruChanged();
    void rChanged();
    void rbChanged();
    void bChanged();
    void lbChanged();
    void lChanged();
    void colorChanged( const QColor& );
    void distanceChanged( int );

private:
    void setShadowDirection( short int sd );

private:
    KoShadowPreview *m_shadowPreview;

    QSpinBox *distance;
    QPushButton *lu, *u, *ru, *r, *rb, *b, *lb, *l;
    KColorButton *color;
    QLabel *lcolor, *ldirection, *ldistance;

    enum {
        SD_LEFT_UP = 1,
        SD_UP = 2,
        SD_RIGHT_UP = 3,
        SD_RIGHT = 4,
        SD_RIGHT_BOTTOM = 5,
        SD_BOTTOM = 6,
        SD_LEFT_BOTTOM = 7,
        SD_LEFT = 8
    } ShadowDirection;

    short int m_shadowDirection;
    double m_shadowDistance;
    QColor m_shadowColor;
};

#endif /* KOFONTDIA_P_H */

