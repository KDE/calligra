#include "timedateformatwidget.h"
#include "dateformatwidget_impl.h"
#include "dateformatwidget_impl.moc"
#include <qdatetime.h>
#include <qcombobox.h>
#include <qcheckbox.h>
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
DateFormatWidget::DateFormatWidget( QWidget* parent,  const char* name, WFlags fl )
    : TimeDateFormatWidgetPrototype( parent, name, fl )
{
    setCaption( i18n( "DateFormat", "This dialog allows you to set the format of the date variable" ) );

    QStringList listDateFormat;
    listDateFormat<<i18n("Locale");
    listDateFormat<<i18n("dd/MM/yy");
    listDateFormat<<i18n("dd/MM/yyyy");
    listDateFormat<<i18n("MMM dd,yy");
    listDateFormat<<i18n("MMM dd,yyyy");
    listDateFormat<<i18n("dd.MMM.yyyy");
    listDateFormat<<i18n("MMMM dd, yyyy");
    listDateFormat<<i18n("dddd, MMM dd,yy");
    listDateFormat<<i18n("MM-dd");
    listDateFormat<<i18n("yyyy-MM-dd");
    listDateFormat<<i18n("MMMM");

    combo2->insertItem( i18n( "Month" ) );
    combo2->insertItem( i18n( "Month (2 digit)" ) );
    combo2->insertItem( i18n( "Abbrevated month name" ) );
    combo2->insertItem( i18n( "Long month name" ) );
    combo2->insertItem( i18n( "The year as two digit" ) );
    combo2->insertItem( i18n( "The year as four digit" ) );
    combo2->setCurrentItem( 0 );

    ComboBox3->insertStringList(listDateFormat);

    connect( CheckBox1, SIGNAL(toggled ( bool )),this,SLOT(slotPersonalizeChanged(bool)));
    connect( ComboBox3, SIGNAL(activated ( const QString & )), this, SLOT(slotDefaultValueChanged(const QString &)));
    slotPersonalizeChanged(false);
}

/*
 *  Destroys the object and frees any allocated resources
 */
DateFormatWidget::~DateFormatWidget()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 * public slot
 */
void DateFormatWidget::slotDefaultValueChanged(const QString & )
{
    updateLabel();
}

void DateFormatWidget::slotPersonalizeChanged(bool b)
{
    combo2->setEnabled(b);
    TextLabel1->setEnabled(b);
    combo1->setEnabled(b);
    ComboBox3->setEnabled(!b);
    updateLabel();

}

void DateFormatWidget::comboActivated()
{
	QString string=combo2->currentText();
	if(combo1->currentText().lower()==i18n("Locale").lower())
		combo1->setCurrentText("");
        if(string==i18n( "Month" ) )
            combo1->lineEdit()->insert("M");
        if(string==i18n( "Month (2 digit)" ) )
            combo1->lineEdit()->insert("MM");
        if(string==i18n( "Abbrevated month name" ) )
            combo1->lineEdit()->insert("MMM");
        if(string==i18n( "Long month name" ) )
            combo1->lineEdit()->insert("MMMM");
        if(string==i18n( "The year as two digit" ) )
            combo1->lineEdit()->insert("yy");
        if(string==i18n( "The year as four digit" ) )
            combo1->lineEdit()->insert("yyyy");
	updateLabel();
	combo1->setFocus();
}

/*
 * public slot
 */
void DateFormatWidget::updateLabel()
{
    QDate ct=QDate::currentDate();
    if(CheckBox1->isChecked())
    {
        if(combo1->currentText().lower()==i18n("Locale").lower())
        {
            label->setText(KGlobal::locale()->formatDate( ct ));
            return;
        }
        label->setText(ct.toString(combo1->currentText()));
    }
    else
    {

        if(ComboBox3->currentText().lower()==i18n("Locale").lower())
        {
            label->setText(KGlobal::locale()->formatDate( ct ));
            return;
        }
        label->setText(ct.toString(ComboBox3->currentText()));
    }
}

QString DateFormatWidget::resultString()
{
    return (CheckBox1->isChecked() ? combo1->currentText():ComboBox3->currentText());
}
