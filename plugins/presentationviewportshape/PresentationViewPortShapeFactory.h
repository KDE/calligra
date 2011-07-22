#include <KoShapeFactoryBase.h>


class PresentationViewPortShapeFactory : public KoShapeFactoryBase 
{
public:
  PresentationViewPortShapeFactory(QObject* parent);
    //PresentationViewPortShapeFactory();
 
     virtual KoShape *createDefaultShape(KoResourceManager *documentResources = 0) const;
    //KoShape* createShape(const KoProperties* params) const;
    bool supports(const KoXmlElement & e, KoShapeLoadingContext &/*context*/) const;
    };