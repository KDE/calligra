#ifndef __ko_print_dia__
#define __ko_print_dia__

#include <qdialog.h>
#include <qprinter.h>
#include <qcombo.h>
#include <qlined.h>
#include <qpushbt.h>
#include <qframe.h>
#include <qlabel.h>

class KoPrintDiaData
{
public:
    KoPrintDiaData ( QWidget* parent );

    QComboBox* printer;
    QLineEdit* fileName;
    QLineEdit* printCmd;
    QPushButton* cancel;
    QPushButton* ok;
    QLineEdit* copies;
    QPushButton* pageSetup;
    QLineEdit* printerName;
    QPushButton* browse;
};

/**
 */
class KoPrintDia : public QDialog, private KoPrintDiaData
{
    Q_OBJECT
public:
    KoPrintDia( QWidget* parent = NULL, const char* name = NULL );
    virtual ~KoPrintDia();

    void configurePrinter( QPrinter &_printer );
    
    static bool print( QPrinter &_printer );

public slots:
    void slotPaperLayout();
    void slotPrinter( const char *_data );
    void slotBrowse();
  
signals:
    void sig_paperLayout( KoPrintDia* );
};

#endif

