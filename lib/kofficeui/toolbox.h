#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <qframe.h>
#include <qpoint.h>

class QVBoxLayout;
class QSpinBox;

class KoDocumentChild;

class KoToolBox : public QFrame
{
    Q_OBJECT
public:
    KoToolBox( QWidget* parent = 0, const char* name = 0 );

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

class KoTransformToolBox : public KoToolBox
{
    Q_OBJECT
public:
    KoTransformToolBox( KoDocumentChild* child, QWidget* parent = 0, const char* name = 0 );

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
    void setDocumentChild( KoDocumentChild* );

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
    KoDocumentChild* m_child;
};

#endif
