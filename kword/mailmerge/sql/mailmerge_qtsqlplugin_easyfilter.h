#ifndef MAILMERGE_QTSQLPLUGIN_EASYFILTER
#define MAILMERGE_QTSQLPLUGIN_EASYFILTER

#include <kdialogbase.h>

class QTable;
class QScrollView;
class QStringList;

class KWQTSqlEasyFilter: public KDialogBase
{
	Q_OBJECT
public:
    KWQTSqlEasyFilter( QWidget *parent);
    virtual ~KWQTSqlEasyFilter();
protected:
    void createColumn(int i);

protected slots:
    void slotValueChanged ( int, int);
private:
	QTable *m_table;
	QScrollView *m_view;
        QStringList m_fieldList,m_sortingList,m_operationList;


};

#endif
