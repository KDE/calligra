#ifndef MAILMERGE_QtSqlPLUGIN_EASYFILTER
#define MAILMERGE_QtSqlPLUGIN_EASYFILTER

#include <kdialogbase.h>

class Q3Table;
class Q3ScrollView;
class QStringList;

class KWQtSqlEasyFilter: public KDialogBase
{
	Q_OBJECT
public:
    KWQtSqlEasyFilter( QWidget *parent);
    virtual ~KWQtSqlEasyFilter();
protected:
    void createColumn(int i);

protected slots:
    void slotValueChanged ( int, int);
private:
	Q3Table *m_table;
	Q3ScrollView *m_view;
        QStringList m_fieldList,m_sortingList,m_operationList;


};

#endif
