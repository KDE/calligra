#include "PresentationViewPortConfigWidget.h"
#include "ui_PresentationViewPortConfigWidget.h"

#include <QLayout>

PresentationViewPortConfigWidget::PresentationViewPortConfigWidget(QWidget* parent)
{
  m_widget.setupUi(this);
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

bool PresentationViewPortConfigWidget::showOnShapeCreate()
{
    return false;
}

bool PresentationViewPortConfigWidget::showOnShapeSelect()
{
    return true;
}

void PresentationViewPortConfigWidget::open(KoShape* shape)
{

}

void PresentationViewPortConfigWidget::save()
{

}

