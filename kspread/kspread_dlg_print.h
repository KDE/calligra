#ifndef KSpreadPrintDlg_included
#define KSpreadPrintDlg_included

class KSpreadView;

#include <qdialog.h>
#include <qprinter.h>
#include <qcombo.h>
#include <qlined.h>
#include <qpushbt.h>
#include <qframe.h>
#include <qlabel.h>

class KSpreadPrintDlgData
{
public:
    KSpreadPrintDlgData ( QWidget* parent );

    QComboBox* printer;
    QLineEdit* fileName;
    QLineEdit* printCmd;
    QPushButton* cancel;
    QPushButton* ok;
    QLineEdit* copies;
    QPushButton* pageSetup;
    QLineEdit* printerName;
};

/**
 */
class KSpreadPrintDlg : public QDialog, private KSpreadPrintDlgData
{
    Q_OBJECT
public:
    KSpreadPrintDlg( KSpreadView *_view, QWidget* parent = NULL, const char* name = NULL );
    virtual ~KSpreadPrintDlg();

    void configurePrinter( QPrinter &_printer );
    
public slots:
    void slotPageSetup();
    void slotPrinter( const char *_data );
    
protected:
    KSpreadView *m_pView;
};

#endif

