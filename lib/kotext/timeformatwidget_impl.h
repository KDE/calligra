#ifndef TIMEFORMATWIDGET_H
#define TIMEFORMATWIDGET_H
#include "timeformatwidget.h"

class TimeFormatWidget : public TimeFormatWidgetPrototype
{ 
    Q_OBJECT

public:
    TimeFormatWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~TimeFormatWidget();
    QString resultString();
public slots:
    void updateLabel();
    void comboActivated();
    void slotPersonalizeChanged(bool b);
    void slotDefaultValueChanged(const QString & );
};

#endif // TIMEFORMATWIDGET_H
