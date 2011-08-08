#include "PresentationViewPortConfigWidget.h"
#include "ui_PresentationViewPortConfigWidget.h"

#include <QLayout>
#include "PresentationViewPortShape.h"

PresentationViewPortConfigWidget::PresentationViewPortConfigWidget(PresentationViewPortTool * tool, QWidget* parent)
{
  Q_ASSERT(tool);
  m_widget.setupUi(this);
  
  connect(m_widget.sequence, SIGNAL(valueChanged(int)), this, SIGNAL(propertyChanged()));
  
   
    /*QGridLayout *g = new QGridLayout(this);
    
    m_title = new QLabel;
    m_title->setText("View Port");
    
    g->addWidget(m_title);
    
    setLayout(g);
    qDebug()<<"In here";*/
    
    
}

PresentationViewPortConfigWidget::~PresentationViewPortConfigWidget()
{

}
/*
bool PresentationViewPortConfigWidget::showOnShapeCreate()
{
    return true;
}

bool PresentationViewPortConfigWidget::showOnShapeSelect()
{
    return true;
}
*/
void PresentationViewPortConfigWidget::open(KoShape* shape)
{
    m_shape = dynamic_cast<PresentationViewPortShape*>(shape);
    if(!m_shape)
      return;
    m_widget.sequence->blockSignals(true);
    m_widget.sequence->setValue(m_shape->sequence());
    m_widget.sequence->blockSignals(false);
    qDebug() << "PVPConfigWidget::open()";
}

void PresentationViewPortConfigWidget::save()
{
    m_shape->setSequence(m_widget.sequence->value());
}

