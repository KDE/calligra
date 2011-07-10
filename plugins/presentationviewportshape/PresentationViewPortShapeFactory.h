#include <KoShapeFactoryBase.h>


class PresentationViewPortShapeFactory : public KoShapeFactoryBase 
{
public:
    PresentationViewPortShapeFactory();
 
    KoShape* createDefaultShape() const;
    //KoShape* createShape(const KoProperties* params) const;
    bool supports(const KoXmlElement & e, KoShapeLoadingContext &/*context*/) const;
};