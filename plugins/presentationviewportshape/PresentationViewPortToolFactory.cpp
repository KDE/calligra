#include "PresentationViewPortToolFactory.h"

#include "PresentationViewPortShape.h"
#include "PresentationViewPortTool.h"

PresentationViewPortToolFactory::PresentationViewPortToolFactory()
    : KoToolFactoryBase("PresentationViewPortShape")
{
    setToolTip( i18n("Presentation View Port Tool") );
    setToolType( dynamicToolType() );
    //setIcon ("artistictext-tool");
    setPriority( 1 );
    setActivationShapeId("PresentationViewPortShape");
}

PresentationViewPortToolFactory::~PresentationViewPortToolFactory()
{

}

KoToolBase* PresentationViewPortToolFactory::createTool(KoCanvasBase* canvas)
{
    return new PresentationViewPortTool(canvas);
}
