//   numeric slider class
//
//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#include "numSlider.h"

numSlider::numSlider( QWidget *parent, const char *name )
	: QWidget(parent, name)
{
	init(0, 10, 0, 0, QBoxLayout::LeftToRight);
}

numSlider::numSlider( QBoxLayout::Direction aDir, QWidget *parent,
											const char *name )
	: QWidget(parent, name)
{
	init(0, 10, 0, 0, aDir);
}

numSlider::numSlider(double minValue, double maxValue, int decimals,
										 double value, QBoxLayout::Direction aDir,
										 QWidget *parent, const char *name )
	: QWidget(parent, name)
{
	init(minValue, maxValue, decimals, value, aDir);
}

void
numSlider::init(double minValue, double maxValue, int decimals, double value,
											QBoxLayout::Direction aDir)
{
	QSlider::Orientation orient;
	QBoxLayout *bl;

	theDecimals=decimals;
	dir=aDir;
	bl=new QBoxLayout(this, dir, 2);

	if ((dir==QBoxLayout::TopToBottom) || (dir==QBoxLayout::BottomToTop))
		orient=QSlider::Vertical;
	else
		orient=QSlider::Horizontal;

	slider=new QSlider(0, SLIDERMAX, SLIDERMAX/20, 0, orient, this);

	if (orient==QSlider::Vertical) {
		slider->setFixedWidth(20);
		setFixedWidth(20);
	} else {
		slider->setFixedHeight(20);
		setFixedHeight(20);
	}
	bl->addWidget(slider);

	numText=new QLineEdit(this);
	numText->setFixedHeight(20);
	bl->addWidget(numText);

	//	QDoubleValidator *dv=new QDoubleValidator(theMin,theMax,0,this);
	//	numText->setValidator(dv);

	theValue=value;
	theMin=minValue;
	theMax=maxValue;

	char str[16];
	sprintf(str,"00%.*f",decimals,theMin);
	int numWidth=MAX(20,numText->fontMetrics().width(str));
	//printf("fm=%d\n",numText->fontMetrics().width(str));
	sprintf(str,"00%.*f",decimals,theMax);
	numWidth=MAX(numWidth,numText->fontMetrics().width(str));
	//printf("fm=%d\n",numText->fontMetrics().width(str));
	numText->setFixedWidth(numWidth);
	
 	range=theMax-theMin;

	connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(sliderChanged(int)));
	connect(slider, SIGNAL(sliderReleased()), this, SLOT(doSliderReleased()));
	connect(numText, SIGNAL(returnPressed()), this, SLOT(numberChanged()));

	updateValue();
	numberChanged();
}

void
numSlider::sliderChanged(int value)
{
	//printf("value %d\n",value);
	theValue=range*value/SLIDERMAX+theMin;
	updateValue();
}

void
numSlider::doSliderReleased()
{
	emit newValue(theValue);
}

void
numSlider::updateValue()
{
	QString valueText;
	valueText.sprintf("%.*f",theDecimals,theValue);
	//cout << numText;
	numText->setText(valueText);

	emit valueChanged(theValue);
}

void
numSlider::numberChanged()
{
	theValue=atof(numText->text());
	if (theValue>theMax) { theValue=theMax; }
	if (theValue<theMin) { theValue=theMin; }
	//printf("value %f\n",theValue);
	int sliderValue=int(SLIDERMAX*(theValue-theMin)/range);
	//	printf("slidervalue %d\n",sliderValue);
	slider->setValue(sliderValue);
	updateValue();
	emit newValue(theValue);
}


double
numSlider::value()
{
	return(theValue);
}

void
numSlider::setValue(float val)
{
	val=MAX(val, theMin);
	val=MIN(val, theMax);
	if (theValue!=val) {
		theValue=val;
		updateValue();
		numberChanged();
	}
}


#include "numSlider.moc"
