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
#include <kfontdialog.h>
#include <qdialog.h>
#include <qchkbox.h>
#include <klineedit.h>
#include <qradiobt.h>

#include <qscrbar.h>
#include <qwidget.h>
#include <qpushbt.h>

class QSpinBox;

class MatrixSetupWidget  : public QDialog
{
    Q_OBJECT

public:

    MatrixSetupWidget(QWidget* parent = 0,const char* name = 0);

    virtual ~MatrixSetupWidget();
    void setString(QString str);
    
protected slots:

    virtual void valueChanged(int);
    virtual void CancelPressed();
    virtual void OkPressed();

protected:
    QComboBox* co[7];
    QRadioButton* cb[6];
    QSpinBox* spb[5];
signals:	 
   void  returnString(QString);

};

#endif
