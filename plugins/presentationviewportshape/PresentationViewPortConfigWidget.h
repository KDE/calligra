#include <KoShapeConfigWidgetBase.h>

#include <QLabel>
#include "PresentationViewPortTool.h"
#include "ui_PresentationViewPortConfigWidget.h"

class PresentationViewPortConfigWidget : public KoShapeConfigWidgetBase
{
  
public:
    PresentationViewPortConfigWidget(PresentationViewPortTool* tool, QWidget* parent = 0);
    ~PresentationViewPortConfigWidget();
    
signals:
  void sequenceChanged(int);
  
private slots:
    void updateWidget();
  
private:
  void blockChildSignals(bool block);
  
  //Reimplemented methods
    void open(KoShape *shape);
    void save();
   // bool showOnShapeCreate();
    //bool showOnShapeSelect();
    
    Ui::PresentationViewPortConfigWidget m_widget;
    PresentationViewPortShape* m_shape;
    PresentationViewPortTool* m_tool;
};