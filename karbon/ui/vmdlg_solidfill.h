/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMDLGSOLIDFILL_H__
#define __VMDLGSOLIDFILL_H__

#include <qtabdialog.h>

class QSpinBox;
class KHSSelector;
class KGradientSelector;
class KColorPatch;

class VMDlgSolidFill : public QTabDialog
{
	Q_OBJECT
public:
	VMDlgSolidFill();

private:
	QWidget* mRGBWidget;
	KHSSelector* mColorSelector;
	QSpinBox* mRed;
	QSpinBox* mGreen;
	QSpinBox* mBlue;
	QSpinBox* mHue;
	QSpinBox* mSaturation;
	QSpinBox* mValue;
	KGradientSelector* mSelector;
	KColorPatch* mOldColor;
	KColorPatch* mColorPreview;

private slots:
	void slotUpdateFromRGBSpinBoxes();
	void slotUpdateFromHSVSpinBoxes();
};

#endif
