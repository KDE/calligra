#include "timedateformatwidget.h"
#include "timeformatwidget_impl.h"
#include "timeformatwidget_impl.moc"
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
TimeFormatWidget::TimeFormatWidget( QWidget* parent,  const char* name, WFlags fl )
    : TimeDateFormatWidgetPrototype( parent, name, fl )
{
    setCaption( i18n( "TimeFormat", "This dialog allows you to set the format of the time variable" ) );

    QStringList listTimeFormat;
    listTimeFormat<<i18n("Locale");
    listTimeFormat<<i18n("hh:mm");
    listTimeFormat<<i18n("hh:mm:ss");
    listTimeFormat<<i18n("hh:mm AP");
    listTimeFormat<<i18n("hh:mm:ss AP");
    listTimeFormat<<i18n("mm:ss.zzz");

    combo2->insertItem( i18n( "Hour" ) );
    combo2->insertItem( i18n( "Hour (2 digit)" ) );
    combo2->insertItem( i18n( "Minute" ) );
    combo2->insertItem( i18n( "Minute (2 digit)" ) );
    combo2->insertItem( i18n( "Second" ) );
    combo2->insertItem( i18n( "am/pm" ) );
    combo2->insertItem( i18n( "AM/PM" ) );
    combo2->setCurrentItem( 0 );

    ComboBox3->insertStringList(listTimeFormat);

    connect( CheckBox1, SIGNAL(toggled ( bool )),this,SLOT(slotPersonalizeChanged(bool)));
    connect( ComboBox3, SIGNAL(activated ( const QString & )), this, SLOT(slotDefaultValueChanged(const QString &)));
    slotPersonalizeChanged(false);
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
void TimeFormatWidget::slotDefaultValueChanged(const QString & )
{
    updateLabel();
}

void TimeFormatWidget::slotPersonalizeChanged(bool b)
{
    combo2->setEnabled(b);
    TextLabel1->setEnabled(b);
    combo1->setEnabled(b);
    ComboBox3->setEnabled(!b);
    updateLabel();

}

void TimeFormatWidget::comboActivated()
{
    QString string=combo2->currentText();
    if(combo1->currentText().lower()==i18n("Locale").lower())
        combo1->setCurrentText("");
    else if(string==i18n("Hour"))
        combo1->lineEdit()->insert("h");
    else if(string==i18n("Hour (2 digit)"))
        combo1->lineEdit()->insert("hh");
    else if(string==i18n("Minute"))
        combo1->lineEdit()->insert("m");
    else if(string==i18n("Minute (2 digit)"))
        combo1->lineEdit()->insert("mm");
    else if(string==i18n("Second"))
        combo1->lineEdit()->insert("s");
    else if(string==i18n("AM/PM"))
        combo1->lineEdit()->insert("AP");
    else if(string==i18n("am/pm"))
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
    if(CheckBox1->isChecked())
    {
        if(combo1->currentText().lower()==i18n("Locale").lower())
        {
            label->setText(KGlobal::locale()->formatTime( ct ));
            return;
        }
        label->setText(ct.toString(combo1->currentText()));
    }
    else
    {

        if(ComboBox3->currentText().lower()==i18n("Locale").lower())
        {
            label->setText(KGlobal::locale()->formatTime( ct ));
            return;
        }
        label->setText(ct.toString(ComboBox3->currentText()));
    }
}

QString TimeFormatWidget::resultString()
{
    return (CheckBox1->isChecked() ? combo1->currentText():ComboBox3->currentText());
}
