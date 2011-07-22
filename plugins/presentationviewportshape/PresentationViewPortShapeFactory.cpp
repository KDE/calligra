#include "PresentationViewPortShapeFactory.h"
#include "PresentationViewPortShape.h"

#include <KoShapeFactoryBase.h>
#include "KoLineBorder.h"
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoGradientBackground.h>
#include <KoShapeLoadingContext.h>

#include <klocale.h>

PresentationViewPortShapeFactory::PresentationViewPortShapeFactory(QObject * parent)
    : KoShapeFactoryBase("PresentationViewPortShape", i18n("PresentationViewPort"))
{
  //qDebug() <<"PresentationViewPortShapeFactory created";
    setToolTip(i18n("A Presentation View Port"));
    setIcon("rectangle-shape");
    setFamily("geometric"); //FIXME Where to put this?
    //setOdfElementNames(KoXmlNS::draw, QStringList("rect"));
    setLoadingPriority(1);
}

KoShape *PresentationViewPortShapeFactory::createDefaultShape(KoResourceManager *) const
{
   // qDebug() << "PresentationViewPortShapeFactory::createDefaultShape() called.";
  
    PresentationViewPortShape *viewport = new PresentationViewPortShape();
 
    return viewport;
}

bool PresentationViewPortShapeFactory::supports(const KoXmlElement & e, KoShapeLoadingContext &/*context*/) const
{
    Q_UNUSED(e);
    return true;
    //return (e.localName() == "rect" && e.namespaceURI() == KoXmlNS::draw);
}

/*QList<KoShapeConfigWidgetBase*> PresentationViewPortShapeFactory::createShapeOptionPanels()
{
    QList<KoShapeConfigWidgetBase*> panels;
    panels.append(new PresentationViewPortShapeConfigWidget());
    return panels;
}
*/
