#include "SimpleAnnotationWidget.h"


#include "../ReviewTool.h"
#include <kaction.h>
#include <kdebug.h>
SimpleAnnotationWidget::SimpleAnnotationWidget(ReviewTool *tool, QWidget *parent) :
    QWidget(parent),
    m_tool(tool)
{
    widget.setupUi(this);

    connect(widget.insertAnnotation, SIGNAL(clicked()), this, SLOT(addAnnotation()));
    connect(widget.removeAnnotation, SIGNAL(clicked()), this, SLOT(removeAnnotation()));
}

void SimpleAnnotationWidget::addAnnotation()
{
    if (m_tool) {
        m_tool->insertAnnotation();
    }
}

void SimpleAnnotationWidget::removeAnnotation()
{
    if (m_tool) {
        m_tool->removeAnnotation();
    }
}
