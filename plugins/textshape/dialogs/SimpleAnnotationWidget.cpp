#include "SimpleAnnotationWidget.h"


#include "../ReviewTool.h"
#include <kaction.h>
#include <kdebug.h>
SimpleAnnotationWidget::SimpleAnnotationWidget(ReviewTool *tool, QWidget *parent) :
    QWidget(parent),
    m_tool(tool)
{
    widget.setupUi(this);
    widget.insertAnnotation->setDefaultAction(tool->action("insert_annotation"));
    widget.removeAnnotation->setDefaultAction(tool->action("remove_annotation"));

    // This doesn't work due to a bug in Qt.
    // FIXME: Check if there is a workaround.
    widget.removeAnnotation->setDisabled(true);

    connect(widget.insertAnnotation, SIGNAL(clicked(bool)), this, SLOT(addAnnotation()));
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
