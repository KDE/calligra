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
#include <kdebug.h>
#include <knuminput.h>
#include <koVariable.h>

/*
 *  Constructs a DateFormatWidget which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'fl'
 */
DateFormatWidget::DateFormatWidget( QWidget* parent,  const char* name, WFlags fl )
    : TimeDateFormatWidgetPrototype( parent, name, fl )
{
    setCaption( i18n( "DateFormat", "This dialog allows you to set the format of the date variable" ) );
    QStringList listDateFormat = KoVariableDateFormat::staticTranslatedFormatPropsList();
    combo1->insertStringList(listDateFormat);

    combo2->insertItem( i18n( "Day"));
    combo2->insertItem( i18n( "Day (2 digits)"));
    combo2->insertItem( i18n( "Day (abbreviated name)"));
    combo2->insertItem( i18n( "Day (long name)"));
    combo2->insertItem( i18n( "Month" ) );
    combo2->insertItem( i18n( "Month (2 digits)" ) );
    combo2->insertItem( i18n( "Month (abbreviated name)" ) );
    combo2->insertItem( i18n( "Month (long name)" ) );
    combo2->insertItem( i18n( "Month (possessive abbreviated name)" ) );
    combo2->insertItem( i18n( "Month (possessive long name)" ) );
    combo2->insertItem( i18n( "Year (2 digits)" ) );
    combo2->insertItem( i18n( "Year (4 digits)" ) );
    combo2->setCurrentItem( 0 );

    label_correction->setText(i18n("Correct in Days"));

    connect( CheckBox1, SIGNAL(toggled ( bool )),this,SLOT(slotPersonalizeChanged(bool)));
    connect( combo1, SIGNAL(activated ( const QString & )), this, SLOT(slotDefaultValueChanged(const QString &)));
    connect( KIntNumInput1, SIGNAL(valueChanged(int)), this, SLOT( slotOffsetChanged(int)));
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

void DateFormatWidget::slotOffsetChanged(int)
{
    updateLabel();
}

void DateFormatWidget::slotPersonalizeChanged(bool b)
{
    combo2->setEnabled(b);
    TextLabel1->setEnabled(b);
    combo1->setEditable(b);
    updateLabel();

}

void DateFormatWidget::comboActivated()
{
    QString string=combo2->currentText();
    if(string==i18n( "Day"))
        combo1->lineEdit()->insert("d");
    else if(string==i18n( "Day (2 digits)"))
        combo1->lineEdit()->insert("dd");
    else if(string==i18n( "Day (abbreviated name)"))
        combo1->lineEdit()->insert("ddd");
    else if(string==i18n( "Day (long name)"))
        combo1->lineEdit()->insert("dddd");
    else if(string==i18n( "Month" ) )
        combo1->lineEdit()->insert("M");
    else if(string==i18n( "Month (2 digits)" ) )
        combo1->lineEdit()->insert("MM");
    else if(string==i18n( "Month (abbreviated name)" ) )
        combo1->lineEdit()->insert("MMM");
    else if(string==i18n( "Month (long name)" ) )
        combo1->lineEdit()->insert("MMMM");
    else if(string==i18n( "Month (possessive abbreviated name)" ) )
        combo1->lineEdit()->insert("PPP");
    else if(string==i18n( "Month (possessive long name)" ) )
        combo1->lineEdit()->insert("PPPP");
    else if(string==i18n( "Year (2 digits)" ) )
        combo1->lineEdit()->insert("yy");
    else if(string==i18n( "Year (4 digits)" ) )
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
    ct = ct.addDays(correctValue());
    if(combo1->currentText().lower()==i18n("Locale").lower())
      {
	label->setText(KGlobal::locale()->formatDate( ct ));
	return;
      }
    QString tmp=combo1->currentText();;
    tmp.replace("PPPP", KGlobal::locale()->monthNamePossessive(ct.month(), false)); //long possessive month name
    tmp.replace("PPP", KGlobal::locale()->monthNamePossessive(ct.month(), true)); //short possessive month name
    label->setText(ct.toString(tmp));
}

QString DateFormatWidget::resultString()
{
    return combo1->currentText();
}

int DateFormatWidget::correctValue()
{
    return KIntNumInput1->value();
}
