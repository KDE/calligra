/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void KPTTaskResourcesPanelBase::init()
{
    effort = new KPTDurationWidget( groupBox1, "effort" );
    layout1->addWidget( effort, 0, 1 );

    // Reset the tab order now that we have added a widget.
    setFocusProxy(effort);
    effort->setFocusPolicy(QWidget::StrongFocus);
    QWidget::setTabOrder(effort, optimisticValue);
    QWidget::setTabOrder(optimisticValue, effortType);
    QWidget::setTabOrder(effortType, risk);
    QWidget::setTabOrder(risk, pessimisticValue);
    QWidget::setTabOrder(pessimisticValue, groupList);
    QWidget::setTabOrder(groupList, resourceTable);
}
