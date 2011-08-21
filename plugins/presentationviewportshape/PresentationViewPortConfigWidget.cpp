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
    PresentationViewPortShape* currShape = m_tool->currentShape();
//    qDebug() << "Sequence in current shape = " << (currShape->attribute("sequence")).toInt();
   
    m_widget.sequence->setValue((currShape->attribute("sequence")).toInt());
    m_widget.duration->setValue((currShape->attribute("transition-duration-ms").toInt()));
    m_widget.zoom->setValue((currShape->attribute("transition-zoom-percent")).toInt());
    
    int index = currShape->transitionProfileIndex(currShape->attribute("transition-profile"));
    m_widget.transitionProfile->setCurrentIndex(index);
    
  
}

void PresentationViewPortConfigWidget::blockChildSignals(bool block)
{
    m_widget.sequence->blockSignals(block);
}
