#ifndef FRAME_H
#define FRAME_H

#include <qwidget.h>
#include <qpoint.h>

class View;

class Frame : public QWidget
{
    Q_OBJECT
public:
    enum State { Inactive, Selected, Active };
    
    Frame( QWidget* parent = 0, const char* name = 0 );
    ~Frame();
    
    void setView( View* );
    View* view();

    void setState( State );

    int leftBorder() const;
    int rightBorder() const;
    int topBorder() const;
    int bottomBorder() const;

    int border() const;
    
protected:
    void paintEvent( QPaintEvent* );
    void mousePressEvent( QMouseEvent* );
    void mouseMoveEvent( QMouseEvent* );
    void mouseReleaseEvent( QMouseEvent* );
    void resizeEvent( QResizeEvent* );
    bool eventFilter( QObject*, QEvent* );
    
private:
    View* m_view;
    
    QPoint m_mousePressPos;
    QPoint m_framePos;
    int m_width;
    int m_height;
    int m_mode;
    
    State m_state;
};

#endif
