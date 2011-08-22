#include "PresentationViewPortTool.h"
#include "ui_PresentationViewPortConfigWidget.h"

class PresentationViewPortConfigWidget : public QWidget
{
  
public:
    PresentationViewPortConfigWidget(PresentationViewPortTool* tool, QWidget* parent = 0);
    ~PresentationViewPortConfigWidget();
      
public slots:
    void updateWidget();
  
private:
    Ui::PresentationViewPortConfigWidget m_widget;
    PresentationViewPortShape* m_shape;
    PresentationViewPortTool* m_tool;
};