#include <klocale.h>

#include "KPrIrisWipeEffectFactory.h"
#include "../KPrIrisWipeEffectStrategyBase.h"

#define IrisWipeEffectId "IrisWipeEffect"

KPrIrisWipeEffectFactory::KPrIrisWipeEffectFactory()
: KPrPageEffectFactory( IrisWipeEffectId, i18n("Iris Wipe Effect") )
{

     QPainterPath shape;

    //iris rectangle
    shape.addRect( -25, -25, 50, 50 );
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Rectangle, "irisWipe", "rectangle", false) );

    //iris diamond
    shape = QPainterPath();
    shape.moveTo( 0, -25);
    shape.lineTo( 25, 0 );
    shape.lineTo( 0, 25 );
    shape.lineTo(-25, 0 );
    shape.closeSubpath();

    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Diamond, "irisWipe", "diamond", false) );
}

KPrIrisWipeEffectFactory::~KPrIrisWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "Rectangular" ),
    I18N_NOOP( "Diamond" )
};

QString KPrIrisWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}