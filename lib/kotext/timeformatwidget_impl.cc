#include "timeformatwidget.h"
#include "timeformatwidget_impl.h"
#include <qdatetime.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <kglobal.h>
#include <klocale.h>
#include <qlineedit.h>

/* 
 *  Constructs a TimeFormatWidget which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
TimeFormatWidget::TimeFormatWidget( QWidget* parent,  const char* name, WFlags fl )
    : TimeFormatWidgetPrototype( parent, name, fl )
{
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
void TimeFormatWidget::comboActivated()
{
	QString string=combo2->currentText();
	if(combo1->currentText().lower()==i18n("Locale").lower())
		combo1->setCurrentText("");
	if(string==i18n("Hour"))
		combo1->lineEdit()->insert("h");
	if(string==i18n("Hour (2 digit)"))
		combo1->lineEdit()->insert("hh");
	if(string==i18n("Minute"))
		combo1->lineEdit()->insert("m");
	if(string==i18n("Minute (2 digit)"))
		combo1->lineEdit()->insert("mm");
	if(string==i18n("Second"))
		combo1->lineEdit()->insert("s");
	if(string==i18n("AM/PM"))
		combo1->lineEdit()->insert("AP");
	if(string==i18n("am/pm"))
		combo1->lineEdit()->insert("ap");
	updateLabel();
	combo1->setFocus();
}

/* 
 * public slot
 */
void TimeFormatWidget::updateLabel()
{
    QTime ct=QTime::currentTime();
    if(combo1->currentText().lower()==i18n("Locale").lower())
    {
	label->setText(KGlobal::locale()->formatTime( ct ));
	return;
    }
    label->setText(ct.toString(combo1->currentText()));
}

QString TimeFormatWidget::resultString()
{
    return combo1->currentText();
}
