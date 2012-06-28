
#ifndef KPTPRINTINGCONTROLPRIVATE_H
#define KPTPRINTINGCONTROLPRIVATE_H

#include <QObject>

class QPrintDialog;

namespace KPlato
{
class PrintingDialog;

class PrintingControlPrivate : public QObject
{
    Q_OBJECT
public:
    PrintingControlPrivate( PrintingDialog *job, QPrintDialog *dia );
    ~PrintingControlPrivate() {}
public slots:
    void slotChanged();
private:
    PrintingDialog *m_job;
    QPrintDialog *m_dia;
};

}

#endif //KPTPRINTINGCONTROLPRIVATE_H
