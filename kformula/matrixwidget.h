/**
 * KFormula - Formula Editor
 *
 * This code is free. Do with it what you want.
 *
 * (c) KDE Project
 * Andrea Rizzi
 * Bilibao@ouverture.it
 */

#ifndef _matrix_widget_h
#define _matrix_widget_h
#include <klocale.h>
#include <kspinbox.h>
#include <kfontdialog.h>
#include <qdialog.h>
#include <qchkbox.h>
#include <klined.h>
#include <qradiobt.h>

#include <qscrbar.h>
#include <qwidget.h>
#include <qpushbt.h>


class MatrixSetupWidget  : public QDialog
{
    Q_OBJECT

public:

    MatrixSetupWidget(QWidget* parent = NULL,const char* name = NULL);

    virtual ~MatrixSetupWidget();
    void setString(QString str);
    
public slots:
   

protected slots:

    virtual void valueChanged();
    virtual void CancelPressed();
    virtual void OkPressed();
/*    virtual void getValue1(bool);
    virtual void getValue3(bool);
    virtual void getValue2(bool);
    virtual void getValue4(bool);
*/
protected:
    QComboBox* co[7];
    QRadioButton* cb[6];
//    QCheckBox* fcb[5];
    KNumericSpinBox* spb[5];
signals:	 
   void  returnString(QString);

};

#endif
