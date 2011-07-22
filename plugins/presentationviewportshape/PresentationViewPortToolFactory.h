#ifndef PRESENTATIONVIEWPORTTOOLFACTORY_H
#define PRESENTATIONVIEWPORTTOOLFACTORY_H

#include <KoToolFactoryBase.h>

class PresentationViewPortToolFactory : public KoToolFactoryBase
{
public:
    PresentationViewPortToolFactory();
    virtual ~PresentationViewPortToolFactory();

    KoToolBase * createTool( KoCanvasBase *canvas );
};

#endif // PRESENTATIONVIEWPORTTOOLFACTORY_H
