/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, The Karbon Developers
*/

/* vtranslate.cc */
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <klocale.h>
#include <knuminput.h>

#include "vtranslate.h"

VTranslate::VTranslate(QWidget *parent, const char *name)
    : QWidget(parent, name)
{
    setCaption(i18n("Translate"));
    QVBoxLayout *mainlayout = new QVBoxLayout(this, 7);
    mainlayout->addSpacing(5);

    QGridLayout *inputlayout = new QGridLayout(this, 5, 3);
    mainlayout->addLayout(inputlayout);
    labelx = new QLabel(i18n("X:"), this);
    inputlayout->addWidget(labelx, 0, 0);
    labely = new QLabel(i18n("Y:"), this);
    inputlayout->addWidget(labely, 1, 0);
    inputlayout->addColSpacing(1, 1);
    inputlayout->addColSpacing(3, 5);
    inputx = new KDoubleNumInput(0.00, this);
    inputx->setRange(-10000.00, 10000.00, 1.00, false); //range is just for example - for now :-)
    inputlayout->addWidget(inputx, 0, 2);
    inputy = new KDoubleNumInput(0.00, this);
    inputy->setRange(-10000.00, 10000.00, 1.00, false);
    inputlayout->addWidget(inputy, 1, 2);
    unit1 = new QLabel("", this);
    inputlayout->addWidget(unit1, 0, 4);
    unit2 = new QLabel("", this);
    inputlayout->addWidget(unit2, 1, 4);
    mainlayout->addSpacing(5);
    positionbox = new QCheckBox(i18n("Relative &position"), this);
    mainlayout->addWidget(positionbox);
    mainlayout->addSpacing(5);
    apdbutton = new QPushButton(i18n("&Duplicate"), this);
    mainlayout->addWidget(apdbutton);
    mainlayout->addSpacing(1);
    apbutton = new QPushButton(i18n("&Apply"), this);
    mainlayout->addWidget(apbutton);

    mainlayout->activate();

    setFixedSize(baseSize()); //Set the size tp fixed values
}

void VTranslate::setUnits( const QString& units )
{
    unit1->setText(units);
    unit2->setText(units);
}

VTranslate::~VTranslate() { }

#include "vtranslate.moc"
