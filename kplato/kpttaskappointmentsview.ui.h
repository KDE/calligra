/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/


void KPTTaskAppointmentsView::clear()
{
    if (m_appList) m_appList->clear();
    if (m_taskName) m_taskName->clear();
    if (m_responsible) m_responsible->clear();
    if (m_costToDate) m_costToDate->clear();
    if (m_totalCost) m_totalCost->clear();
    if (m_workToDate) m_workToDate->clear();
    if (m_totalWork) m_totalWork->clear();
}

void KPTTaskAppointmentsView::draw(KPTTask *task)
{
    //kdDebug()<<k_funcinfo<<endl;
    clear();
    if (!task)
        return;
    m_taskName->setText(task->name());
    m_responsible->setText(task->leader());
    QDateTime dt = QDateTime::currentDateTime();
    m_costToDate->setText(KGlobal::locale()->formatMoney(task->plannedCost(dt)));
    m_totalCost->setText(KGlobal::locale()->formatMoney(task->plannedCost()));
    m_workToDate->setText(QString("%1").arg(task->plannedWork(dt)));
    m_totalWork->setText(QString("%1").arg(task->plannedWork()));

    KPTProject *p = dynamic_cast<KPTProject *>(task->projectNode());
    if (!p) {
        kdError()<<k_funcinfo<<"Task: '"<<task->name()<<"' has no project"<<endl;
        return;
    }
    QPtrListIterator<KPTResourceGroup> it(p->resourceGroups());
    for (; it.current(); ++it) {
        QPtrListIterator<KPTResource> rit(it.current()->resources());
        for (; rit.current(); ++rit) {
            KPTResource *r = rit.current();
            QPtrListIterator<KPTAppointment> ait(rit.current()->appointments());
            for (; ait.current(); ++ait) {
                if (ait.current()->task() == task) {
                    QListViewItem *item = new QListViewItem(m_appList, r->name());
                    item->setText(1, r->typeToString());
                    item->setText(2, ait.current()->startTime().date().toString(ISODate));
                    item->setText(3, ait.current()->duration().toString(KPTDuration::Format_Hour));
                    item->setText(4, KGlobal::locale()->formatMoney(r->normalRate()));
                    item->setText(5, KGlobal::locale()->formatMoney(r->overtimeRate()));
                    item->setText(6, KGlobal::locale()->formatMoney(r->fixedCost()));
                }
            }
        }
    }
}


void KPTTaskAppointmentsView::init()
{
    m_appList->setColumnAlignment(1, AlignHCenter);
    m_appList->setColumnAlignment(3, AlignRight);
    m_appList->setColumnAlignment(4, AlignRight);
    m_appList->setColumnAlignment(5, AlignRight);
    m_appList->setColumnAlignment(6, AlignRight);
}
