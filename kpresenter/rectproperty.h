#ifndef RECTPROPERTY_H
#define RECTPROPERTY_H

#include <qwidget.h>

#include "kprcommand.h"

class RectPropertyUI;

class RectProperty : public QWidget
{
    Q_OBJECT
public:
    RectProperty( QWidget *parent, const char *name, RectValueCmd::RectValues &rectValue );
    ~RectProperty();

    int getRectPropertyChange() const;
    RectValueCmd::RectValues getRectValues() const;

    void apply();

protected:
    int getXRnd() const;
    int getYRnd() const;

    RectPropertyUI *m_ui;

    RectValueCmd::RectValues m_rectValue;

protected slots:
    void slotReset();
    void slotRndChanged();
};

#endif /* RECTPROPERTY_H */
