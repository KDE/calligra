#ifndef KDCHARTTABLEDATAWRAPPER_H
#define KDCHARTTABLEDATAWRAPPER_H
#include <qobject.h>
#include <KDChartTable.h>
class KDChartData;

class KDChartTableDataWrapper :public QObject
{
    Q_OBJECT
public:
    KDChartTableDataWrapper( KDChartTableData* );
public slots:
/*
    QVariant cell( uint _row, uint _col );
    void setCell( uint _row, uint _col,
                  const QVariant& _value1,
                  const QVariant& _value2=QVariant() );
    void setCell( uint _row, uint _col, double _element );

    // PENDING(blackie) This one do not work, due to QSA bug regarding function overloading.
    void setCell( uint _row, uint _col, QString _element );
*/
private:
    KDChartTableData* _data;
};


#endif /* KDCHARTTABLEDATAWRAPPER_H */

