#ifndef KSPREAD_DLG_FORMAT
#define KSPREAD_DLG_FORMAT

#include <kdialogbase.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qcolor.h>
#include <qpen.h>
#include <qfont.h>

#include "kspread_cell.h"

class QComboBox;
class QLabel;
class QDomDocument;

class KSpreadView;
class KSpreadLayout;

class KSpreadFormatDlg : public KDialogBase
{
    Q_OBJECT
public:
    KSpreadFormatDlg( KSpreadView* view, const char* name = 0 );
    ~KSpreadFormatDlg();
    
private slots:
    void slotActivated( int index );
    void slotOk();

private:
    bool parseXML( const QDomDocument& doc );
	
    QComboBox* m_combo;
    QLabel* m_label;
    KSpreadView* m_view;

    struct Entry
    {
	QString xml;
	QString image;
	QString config;
	QString name;
    };
    QValueList<Entry> m_entries;

    KSpreadLayout* m_cells[ 16 ];
};

#endif
