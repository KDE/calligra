#ifndef KDCHARTCUSTOMBOX_H
#define KDCHARTCUSTOMBOX_H

#include <qobject.h>
#include "KDChartCustomBox.h"
class KDChartCustomBox;
class KDChartTextPiece;
class KDChartCustomBoxWrapper :public QObject
{
    Q_OBJECT

public:
    KDChartCustomBoxWrapper( KDChartCustomBox* );

public slots:
    const KDChartTextPiece* content() const;


    // These methods need to be here, for the enums to work.
    void setAnchorPosition( int position )
        {
            _data->setAnchorPosition( (KDChartEnums::PositionFlag) position );
        }



private:
    KDChartCustomBox* _data;
};


#endif /* KDCHARTCUSTOMBOX_H */

