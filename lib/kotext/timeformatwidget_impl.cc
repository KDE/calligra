#include "timeformatwidget.h"
#include "timeformatwidget_impl.h"
#include <qdatetime.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <kglobal.h>
#include <klocale.h>

/* 
 *  Constructs a TimeFormatWidget which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
TimeFormatWidget::TimeFormatWidget( QWidget* parent,  const char* name, WFlags fl )
    : TimeFormatWidgetPrototype( parent, name, fl )
{
    radio1->setChecked(true);
    updateLabel();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
TimeFormatWidget::~TimeFormatWidget()
{
    // no need to delete child widgets, Qt does it all for us
}

/* 
 * public slot
 */
void TimeFormatWidget::updateLabel()
{
    QTime ct=QTime::currentTime();
    if(combo->currentText().lower()==i18n("locale")){
	label->setText(KGlobal::locale()->formatTime( ct ));
	box->setEnabled(false);
	return;
    }
    switch(box->id(box->selected()))
    {
	case 0:
	    suffix="";
	    break;	
	case 1:
	    suffix=" ap";
	    break;	
	case 2:
	    suffix=" AP";
	    break;
    }
    box->setEnabled(true);
    label->setText(ct.toString(combo->currentText()+suffix)); 
}

QString TimeFormatWidget::resultString()
{
    return combo->currentText()+suffix;
}
