#include "KDChartCustomBoxWrapper.h"
KDChartCustomBoxWrapper::KDChartCustomBoxWrapper( KDChartCustomBox* data)
    :QObject(0), _data(data)
{
}

const KDChartTextPiece* KDChartCustomBoxWrapper::content() const
{
    return &_data->content();
}


#include "KDChartCustomBoxWrapper.moc"
