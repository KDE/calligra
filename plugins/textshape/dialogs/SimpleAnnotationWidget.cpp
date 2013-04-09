#include "SimpleAnnotationWidget.h"


#include "../ReviewTool.h"
#include <kaction.h>
#include <kdebug.h>
SimpleAnnotationWidget::SimpleAnnotationWidget(ReviewTool *tool, QWidget *parent) :
    QWidget(parent),
    m_tool(tool)
{
    widget.setupUi(this);
    widget.insertAnnotation->setDefaultAction(tool->action("insert_annotations"));
    connect(widget.insertAnnotation, SIGNAL(clicked(bool)), this, SLOT(addAnnotation()));
}

void SimpleAnnotationWidget::addAnnotation()
{
    if (m_tool) {
        m_tool->insertAnnotation();
    }
}
