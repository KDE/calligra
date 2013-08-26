#ifndef FAKE_KDIALOG_H
#define FAKE_KDIALOG_H

#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QPrintDialog>
#else
class QPrinter;
class QPrintDialog;
#endif

class KdePrint
{
public:

    static QPrintDialog* createPrintDialog(QPrinter *printJob, QList<QWidget*> options, QWidget *parent)
    {
#ifndef QT_NO_PRINTER
        QPrintDialog *p = new QPrintDialog(printJob, parent);
        return p;
#else
        return 0;
#endif
    }

};

#endif
 
