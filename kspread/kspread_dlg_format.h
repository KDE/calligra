#ifndef KSPREAD_DLG_FORMAT
#define KSPREAD_DLG_FORMAT

#include <qdialog.h>
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

class KSpreadFormatDlg : public QDialog
{
    Q_OBJECT
public:
    KSpreadFormatDlg( KSpreadView* view, const char* name = 0 );

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

    struct Cell
    {
	KSpreadCell::Align align;
	KSpreadCell::FloatFormat floatFormat;
	KSpreadCell::FloatColor floatColor;
	QColor bgColor;
	QPen pen;
	QFont font;
	QPen leftPen;
	QPen topPen;
    };
    QValueList<Cell> m_cells;
};

#endif
