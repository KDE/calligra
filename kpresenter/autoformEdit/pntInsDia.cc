/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Autoform Editor                                                */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: point insert dialog                                    */
/******************************************************************/

#include "pntInsDia.h"
#include "pntInsDia.moc"

/******************************************************************/
/* class PntInsDia                                                */
/******************************************************************/

/*==================== constructor ===============================*/
PntInsDia::PntInsDia( QWidget* parent, const char* name, int points )
    :QDialog(parent,name,true)
{
    label1 = new QLabel("Insert the new point",this);
    label1->resize(label1->sizeHint());
    label1->move(20,10);

    btnGrp = new QButtonGroup(this);
    btnGrp->hide();

    radioBefore = new QRadioButton("&Before",this);
    radioBefore->resize(radioBefore->sizeHint());
    radioBefore->move(label1->x(),label1->y()+label1->height()+10);

    radioAfter = new QRadioButton("&After",this);
    radioAfter->resize(radioAfter->sizeHint());
    radioAfter->move(label1->x(),radioBefore->y()+radioBefore->height()+10);
    radioAfter->setChecked(true);

    btnGrp->insert(radioBefore);
    btnGrp->insert(radioAfter);

    if (points < 2)
        label2 = new QLabel("Point Number 1",this);
    else
        label2 = new QLabel("Point Number 1",this);
    label2->resize(label2->sizeHint());
    label2->move(label1->x(),radioAfter->y()+radioAfter->height()+10);

    spinBox = new QSpinBox(1, points, 1, this);
    spinBox->setValue(points);
    spinBox->resize(spinBox->sizeHint().width()/4,spinBox->sizeHint().height());
    spinBox->move(label1->x(),label2->y()+label2->height()+10);
    if (points < 2) spinBox->hide();

    cancelBut = new QPushButton(this);
    cancelBut->setText("Cancel");
    cancelBut->resize(cancelBut->sizeHint());

    okBut = new QPushButton(this);
    okBut->setText("OK");
    okBut->setAutoRepeat(false);
    okBut->setAutoResize(false);
    okBut->setAutoDefault(true);
    okBut->setDefault(true);
    okBut->resize(cancelBut->width(),okBut->sizeHint().height());

    okBut->move(20,spinBox->y()+spinBox->height()+20);
    cancelBut->move(okBut->x()+okBut->width()+10,spinBox->y()+spinBox->height()+20);

    connect(okBut,SIGNAL(clicked()),this,SLOT(accept()));
    connect(okBut,SIGNAL(clicked()),this,SLOT(insPnt()));
    connect(cancelBut,SIGNAL(clicked()),this,SLOT(reject()));

    resize(okBut->width()+cancelBut->width()+40,okBut->y()+okBut->height()+10);
}

/*===================== destructor ===============================*/
PntInsDia::~PntInsDia()
{
}

/*========================= ok pressed ===========================*/
void PntInsDia::insPnt()
{
    bool pos;
    int index;

    if (radioBefore->isChecked()) pos = false;
    else pos = true;
    index = spinBox->value();

    emit insPoint(index,pos);
}
