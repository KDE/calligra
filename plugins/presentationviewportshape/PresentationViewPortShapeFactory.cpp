#include "PresentationViewPortShapeFactory.h"
#include "PresentationViewPortShape.h"

#include <KoShapeFactoryBase.h>
#include "KoLineBorder.h"
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoGradientBackground.h>
#include <KoShapeLoadingContext.h>

#include <klocale.h>

PresentationViewPortShapeFactory::PresentationViewPortShapeFactory()
    : KoShapeFactoryBase(PresentationViewPortShapeId, i18n("PresentationViewPort"))
{
    setToolTip(i18n("A PresentationViewPort"));
    setIcon("rectangle-shape");
    setFamily("geometric");
    setOdfElementNames(KoXmlNS::draw, QStringList("rect"));
    setLoadingPriority(1);
}

KoShape *PresentationViewPortShapeFactory::createDefaultShape() const
{
    PresentationViewPortShape *rect = new PresentationViewPortShape();

    rect->setBorder(new KoLineBorder(4.0)); //Thick border
    rect->setShapeId(PresentationViewPortShapeId);

    //No gradient
    
    return rect;
}

bool PresentationViewPortShapeFactory::supports(const KoXmlElement & e, KoShapeLoadingContext &/*context*/) const
{
    Q_UNUSED(e);
    return (e.localName() == "rect" && e.namespaceURI() == KoXmlNS::draw);
}

/*QList<KoShapeConfigWidgetBase*> PresentationViewPortShapeFactory::createShapeOptionPanels()
{
    QList<KoShapeConfigWidgetBase*> panels;
    panels.append(new PresentationViewPortShapeConfigWidget());
    return panels;
}
*/
