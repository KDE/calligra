/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#define DAYS_PER_WEEK 7
#define DAYS_PER_MONTH 30

void KPTDurationWidget::addWeek()
{
    days->setValue(days->value() + DAYS_PER_WEEK);
    emit valueChanged();
}

void KPTDurationWidget::addMonth()
{
    days->setValue(days->value() + DAYS_PER_MONTH);
    emit valueChanged();
}

void KPTDurationWidget::subtractWeek()
{
    if (days->value() >= DAYS_PER_WEEK)
    {
        days->setValue(days->value() - DAYS_PER_WEEK);
        emit valueChanged();
    }
}

void KPTDurationWidget::subtractMonth()
{
    if (days->value() >= DAYS_PER_MONTH)
    {
        days->setValue(days->value() - DAYS_PER_MONTH);
        emit valueChanged();
    }
}

KPTDuration KPTDurationWidget::value()
{
    KPTDuration result(hhmmss->time());
    result.addDays(days->value());
    
    return result;
}

void KPTDurationWidget::days_valueChanged( int )
{
    emit valueChanged();
}

void KPTDurationWidget::hhmmss_valueChanged( const QTime & )
{
    emit valueChanged();
}


void KPTDurationWidget::setValue( const KPTDuration &duration )
{
    KPTDuration tmp = duration;
    tmp.addDays(-duration.days());
    hhmmss->setTime(tmp.time());
    days->setValue(duration.days());
    emit valueChanged();
}
