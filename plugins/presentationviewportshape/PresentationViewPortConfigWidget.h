#include <KoShapeConfigWidgetBase.h>

class PresentationViewPortConfigWidget : public KoShapeConfigWidgetBase
{
    PresentationViewPortConfigWidget();
    ~PresentationViewPortConfigWidget();
    
    //Reomplemented methods
    void open(KoShape *shape);
    void save();
    bool showOnShapeCreate();
    bool showOnShapeSelect();
};