//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#ifndef NUMSLIDER_H
#define NUMSLIDER_H

#include <stream.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <qslider.h>
#include <qlayout.h>
#include <qlined.h>
#include <qvalidator.h>

#include "misc.h"

#define SLIDERMAX 1000000

class numSlider : public QWidget
{
	Q_OBJECT;

 public:
	//	enum Direction { LeftToRight, RightToLeft, TopToBottom, BottomToTop,

	numSlider( QWidget *parent=0, const char *name=0 );
	numSlider( QBoxLayout::Direction aDir, QWidget *parent=0, 
						 const char *name=0 );
	numSlider(double minValue, double maxValue, int decimals, double value,
					QBoxLayout::Direction aDir, QWidget *parent=0, const char *name=0 );

	double value();
	void setValue(float val);

 protected slots:
	 void sliderChanged(int value);
   void numberChanged();
	 void doSliderReleased();

 signals:
	 void valueChanged(double);
	 void newValue(double);

 private:
	void init(double minValue, double maxValue, int decimals, double value,
						QBoxLayout::Direction aDir);
	void updateValue();

	QSlider *slider;
	QLineEdit *numText;
	QBoxLayout::Direction dir;
	double theValue,theMax,theMin,range;
	int theDecimals;
};

#endif
