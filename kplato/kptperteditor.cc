#include "kptperteditor.h"

//#include "kptcommand.h"
//#include "kptitemmodelbase.h"
//#include "kptcalendar.h"
//#include "kptduration.h"
//#include "kptfactory.h"
//#include "kptresourceappointmentsview.h"
#include "kptview.h"
//#include "kptnode.h"
//#include "kptproject.h"
//#include "kpttask.h"
//#include "kptschedule.h"
//#include "kptdatetime.h"
//#include "kptcontext.h"

//#include <kicon.h>
//#include <kglobal.h>
//#include <klocale.h>
//#include <kprinter.h>
//#include <kxmlguifactory.h>
//#include <kactioncollection.h>

#include <kdebug.h>

namespace KPlato
{

//-----------------------------------
PertEditor::PertEditor( Part *part, QWidget *parent )
    : ViewBase( part, parent )
{
    setupGui();

    QHBoxLayout * Main_PERT = new QHBoxLayout( this );

    Main_PERT->setObjectName(i18n("Main_PERT"));

    vboxLayout = new QVBoxLayout();
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(0);
    vboxLayout->setObjectName(i18n("hboxLayout"));
    label = new QLabel(i18n("Tasks List : "));
    label->setObjectName(i18n("labeltasklist"));
    vboxLayout->addWidget(label);
    tableTaskWidget = new QTableWidget();
    tableTaskWidget->setObjectName(i18n("tableTaskWidget"));
    tableTaskWidget->setColumnCount(2);
    QTableWidgetItem * colItem = new QTableWidgetItem();
    QTableWidgetItem * colItem1 = new QTableWidgetItem();
    colItem->setText(i18n("Task Name"));
    colItem1->setText(i18n("Duration"));
    tableTaskWidget->setHorizontalHeaderItem(0,colItem);
    tableTaskWidget->setHorizontalHeaderItem(1,colItem1);


    vboxLayout->addWidget(tableTaskWidget);

    Main_PERT->addLayout(vboxLayout);

    vboxLayout1 = new QVBoxLayout();
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(0);
    vboxLayout1->setObjectName(i18n("hboxLayout"));
    label1 = new QLabel(i18n("Required Tasks : "));
    label1->setObjectName(i18n("labelrequiredlist"));
    vboxLayout1->addWidget(label1);

    assignList = new KActionSelector(this);
    assignList->setObjectName(i18n("tableTaskWidget"));
    assignList->setShowUpDownButtons(false);
    assignList->setSelectedLabel(i18n("Required"));
    vboxLayout1->addWidget(assignList);
    
    Main_PERT->addLayout(vboxLayout1);

}

void PertEditor::setupGui()
{

}


} // namespace KPlato

#include "kptperteditor.moc"
