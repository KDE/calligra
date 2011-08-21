#include "PresentationViewPortConfigWidget.h"
#include "ui_PresentationViewPortConfigWidget.h"

#include <QLayout>
#include "PresentationViewPortShape.h"
#include <KoToolSelection.h>

PresentationViewPortConfigWidget::PresentationViewPortConfigWidget(PresentationViewPortTool * tool, QWidget* parent)
:m_tool(tool)
{
  Q_ASSERT(m_tool);
  
  m_widget.setupUi(this);
  //connect(m_widget.sequence, SIGNAL(valueChanged(int)), this, SIGNAL(sequenceChanged(int)));
  connect(m_widget.sequence, SIGNAL(valueChanged(int)), m_tool, SIGNAL(sequenceChanged(int)));
  connect(m_widget.duration, SIGNAL(valueChanged(int)), m_tool, SIGNAL(durationChanged(int)));
  connect(m_widget.zoom, SIGNAL(valueChanged(int)), m_tool, SIGNAL(zoomChanged(int)));
  connect(m_widget.transitionProfile, SIGNAL(currentIndexChanged(QString)), m_tool, SIGNAL(transitionProfileChanged(QString)));
  
}

PresentationViewPortConfigWidget::~PresentationViewPortConfigWidget()
{

}

void PresentationViewPortConfigWidget::updateWidget()
{
    //TODO
    qDebug() << "in PVPConfigWidget::updateWidget()";
    
  
}

void PresentationViewPortConfigWidget::blockChildSignals(bool block)
{
    m_widget.sequence->blockSignals(block);
}

/*void PresentationViewPortConfigWidget::updateWidget()
{
    KoToolSelection* selection = m_tool->selection();
    if(!selection)
      return;
    
    blockChildSignals(true);    
    
    qDebug() << "Value in widget = " << m_widget.sequence->value();
    m_shape->setSequence(m_widget.sequence->value());
    qDebug() << m_shape->sequence();
    
    blockChildSignals(false);
  
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


