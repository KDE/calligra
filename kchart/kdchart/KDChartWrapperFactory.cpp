#include "KDChartWrapperFactory.h"
#include <KDChartTable.h>
#include <qfont.h>
#include "wrappers/KDChartParamsWrapper.h"
#include "wrappers/KDChartTableDataWrapper.h"
#include "wrappers/KDChartCustomBoxWrapper.h"
#include <KDChartCustomBox.h>
#include "wrappers/KDChartAxisParamsWrapper.h"

KDChartWrapperFactory::KDChartWrapperFactory()
{
    registerWrapper( "KDChartParams", "KDChartParamsWrapper" );
    registerWrapper( "KDChartVectorTableData", "KDChartTableDataWrapper" );
    registerWrapper( "KDChartCustomBox", "KDChartCustomBoxWrapper" );
    registerWrapper( "KDChartAxisParams", "KDChartAxisParamsWrapper" );
}

QObject* KDChartWrapperFactory::create( const QString& className, void* ptr )
{
    if ( className == QString::fromLatin1( "KDChartParams" ) )
        return new KDChartParamsWrapper( static_cast<KDChartParams*>( ptr ) );

    if ( className == QString::fromLatin1( "KDChartVectorTableData" ) )
        return new KDChartTableDataWrapper( static_cast<KDChartTableData*>( ptr ) );

    if ( className == QString::fromLatin1( "KDChartCustomBox" ) )
        return new KDChartCustomBoxWrapper( static_cast<KDChartCustomBox*>( ptr ) );

    if ( className == QString::fromLatin1( "KDChartAxisParams" ) )
        return new KDChartAxisParamsWrapper( static_cast<KDChartAxisParams*>( ptr ) );

    return 0;
}
