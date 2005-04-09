#ifndef BRUSHSTYLEWIDGET_H
#define BRUSHSTYLEWIDGET_H

#include <qbrush.h>
#include <qwidget.h>

#include "kprcommand.h"

class BrushPropertyUI;
class GradientPropertyUI;
class PBPreview;

class QWidgetStack;

class KComboBox;


class BrushProperty : public QWidget
{
    Q_OBJECT
public:
    BrushProperty( QWidget *parent, const char *name, const BrushCmd::Brush &brush );
    ~BrushProperty();

    int getBrushPropertyChange() const;
    BrushCmd::Brush getBrush() const;

    void setBrush( BrushCmd::Brush &brush );

    void apply();

protected:
    FillType getFillType() const;
    QBrush getQBrush() const;

    QColor getGColor1() const;
    QColor getGColor2() const;
    BCType getGType() const;
    bool getGUnbalanced() const;
    int getGXFactor() const;
    int getGYFactor() const;

    void setQBrush( const QBrush &brush );
    void setGradient( const QColor &_c1, const QColor &_c2, BCType _t,
                      bool _unbalanced, int _xfactor, int _yfactor );
    void setUnbalancedEnabled( bool state );

    KComboBox *m_typeCombo;
    QWidgetStack *m_stack;
    BrushPropertyUI *m_brushUI;
    GradientPropertyUI *m_gradientUI;
    PBPreview *m_preview;

    BrushCmd::Brush m_brush;

protected slots:
    void slotReset();
    void slotTypeChanged( int pos );
    void slotBrushChanged();

    void slotColor1Changed();
    void slotColor2Changed();
    void slotBackColorTypeChanged();
    void slotUnbalancedChanged();
    void slotXFactorChanged();
    void slotYFactorChanged();
};

#endif /* BRUSHSTYLEWIDGET_H */
