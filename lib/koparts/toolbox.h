#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <qframe.h>
#include <qpoint.h>

class QVBoxLayout;
class QSpinBox;

class PartChild;

class ToolBox : public QFrame
{
    Q_OBJECT
public:
    ToolBox( QWidget* parent = 0, const char* name = 0 );
    
    virtual void setCaption( const QString& );
    
public slots:
    void setEnabled( bool enable );
    
protected:
    void paintEvent( QPaintEvent* );
    void childEvent( QChildEvent* ev );
    void mousePressEvent( QMouseEvent* ev );
    void mouseMoveEvent( QMouseEvent* ev );
    
private:
    QVBoxLayout* m_layout;
    QPoint m_mousePos;
    QPoint m_startPos;
};

class TransformToolBox : public ToolBox
{
    Q_OBJECT
public:
    TransformToolBox( PartChild* child, QWidget* parent = 0, const char* name = 0 );
    
    double rotation() const;
    double scaling() const;
    double xShearing() const;
    double yShearing() const;
    
    void setRotation( double );
    void setScaling( double );
    void setXShearing( double );
    void setYShearing( double );

signals:
    void rotationChanged( double );
    void scalingChanged( double );
    void xShearingChanged( double );
    void yShearingChanged( double );
    
public slots:
    void setPartChild( PartChild* );
    
private slots:
    void slotRotationChanged( int );
    void slotScalingChanged( int );
    void slotXShearingChanged( int );
    void slotYShearingChanged( int );
 
private:
    QSpinBox* m_rotation;
    QSpinBox* m_scale;
    QSpinBox* m_shearX;
    QSpinBox* m_shearY;
    PartChild* m_child;
};

#endif
