/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCOLORTAB_H
#define __VCOLORTAB_H

#include <qtabwidget.h>

class QColor;
class QSpinBox;
class KHSSelector;
class KGradientSelector;
class KColorPatch;
class KIntNumInput;
class VColor;

class VColorTab : public QTabWidget
{
	Q_OBJECT

public:
	VColorTab( const VColor &c, QWidget* parent = 0L, const char* name = 0L );

	VColor getColor();

private:
	QWidget* mRGBWidget;
	KHSSelector* mColorSelector;
	QSpinBox* mRed;
	QSpinBox* mGreen;
	QSpinBox* mBlue;
	QSpinBox* mHue;
	QSpinBox* mSaturation;
	QSpinBox* mValue;
	KIntNumInput* mOpacity;
	KGradientSelector* mSelector;
	KColorPatch* mOldColor;
	KColorPatch* mColorPreview;

private slots:
	void slotUpdateFromRGBSpinBoxes();
	void slotUpdateFromHSVSpinBoxes();
	void slotUpdate(QColor *color = 0L);
    void slotVChanged( int );
	void slotHSChanged( int, int );
};

#endif

