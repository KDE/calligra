/*
    This file is part of the KDE Libraries

    Copyright (c) 2007 David Jarvie <software@astrojar.org.uk>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB. If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kcolorcombotest.h"

#include <QApplication>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QBoxLayout>
#include <QDebug>

#include <kcolorcombo.h>

KColorComboTest::KColorComboTest(QWidget* widget)
    : QWidget(widget)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    QHBoxLayout *hboxLayout = new QHBoxLayout(this);

    // Standard color list
    QLabel *lbl = new QLabel(QLatin1Literal("&Standard colors:"), this);
    lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    mStandard = new KColorCombo(this);
    mStandard->setObjectName(QLatin1Literal("StandardColors"));
    lbl->setBuddy(mStandard);
    QLabel *lblPreset = new QLabel(QLatin1Literal("Preset to green (0,255,0)"), this);

    // add to box layout
    hboxLayout->addWidget(lbl);
    hboxLayout->addWidget(mStandard);
    hboxLayout->addWidget(lblPreset);
    vbox->addLayout(hboxLayout);

    hboxLayout = new QHBoxLayout(this);

    // Custom color list
    lbl = new QLabel(QLatin1Literal("&Reds, greens, blues:"), this);
    lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    mCustom = new KColorCombo(this);
    mCustom->setObjectName(QLatin1Literal("CustomColors"));
    lbl->setBuddy(mCustom);
    lblPreset = new QLabel(QLatin1Literal("Preset to green (0,192,0)"), this);

    // add to box layout
    hboxLayout->addWidget(lbl);
    hboxLayout->addWidget(mCustom);
    hboxLayout->addWidget(lblPreset);
    vbox->addLayout(hboxLayout);

    // Create an exit button
    mExit = new QPushButton(QLatin1Literal("E&xit"), this);
    QObject::connect(mExit, SIGNAL(clicked()), SLOT(quitApp()));

    vbox->addWidget(mExit);

    // Populate the custom list
    QList<QColor> standardList;
    standardList << Qt::red << Qt::green << Qt::blue << Qt::cyan << Qt::magenta << Qt::yellow << Qt::darkRed
                 << Qt::darkGreen << Qt::darkBlue << Qt::darkCyan << Qt::darkMagenta << Qt::darkYellow
                 << Qt::white << Qt::lightGray << Qt::gray << Qt::darkGray << Qt::black;
    QList<QColor> list;
    list << QColor(255,0,0) << QColor(192,0,0) << QColor(128,0,0) << QColor(64,0,0)
         << QColor(0,255,0) << QColor(0,192,0) << QColor(0,128,0) << QColor(0,64,0)
         << QColor(0,0,255) << QColor(0,0,192) << QColor(0,0,128) << QColor(0,0,64);
    mCustom->setColors(list);
    if (mCustom->colors() != list)
        qCritical() << "Custom combo: setColors() != colors()";
    mCustom->setColors(QList<QColor>());
    if (mCustom->colors() != standardList)
        qCritical() << "Custom combo: setColors(empty) != standard colors";
    mCustom->setColors(list);
    if (mCustom->colors() != list)
        qCritical() << "Custom combo: setColors() != colors()";

    if (mStandard->colors() != standardList)
        qCritical() << "Standard combo: colors()";

    QColor col = QColor(1,2,3);
    mStandard->setColor(col);
    if (mStandard->color() != col)
        qCritical() << "Standard combo: set custom color -> " << mStandard->color().red() << "," << mStandard->color().green() << "," << mStandard->color().blue();
    if (!mStandard->isCustomColor())
        qCritical() << "Standard combo: custom color: isCustomColor() -> false";
    mStandard->setColor(Qt::green);
    if (mStandard->color() != Qt::green)
        qCritical() << "Standard combo: color() -> " << mStandard->color().red() << "," << mStandard->color().green() << "," << mStandard->color().blue();
    if (mStandard->isCustomColor())
        qCritical() << "Standard combo: standard color: isCustomColor() -> true";

    col = QColor(1,2,3);
    mCustom->setColor(col);
    if (mCustom->color() != col)
        qCritical() << "Custom combo: set custom color -> " << mCustom->color().red() << "," << mCustom->color().green() << "," << mCustom->color().blue();
    if (!mCustom->isCustomColor())
        qCritical() << "Custom combo: custom color: isCustomColor() -> false";
    col = QColor(0,192,0);
    mCustom->setColor(col);
    if (mCustom->color() != col)
        qCritical() << "Custom combo: color() -> " << mCustom->color().red() << "," << mCustom->color().green() << "," << mCustom->color().blue();
    if (mCustom->isCustomColor())
        qCritical() << "Custom combo: standard color: isCustomColor() -> true";

}

KColorComboTest::~KColorComboTest()
{
}

void KColorComboTest::quitApp()
{
    qApp->closeAllWindows();
}

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    KColorComboTest* t = new KColorComboTest;
    t->show();
    return a.exec();
}

