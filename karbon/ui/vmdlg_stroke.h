/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMDLGSTROKE_H__
#define __VMDLGSTROKE_H__

#include <qtabdialog.h>

class QColor;
class QSpinBox;
class KHSSelector;
class KGradientSelector;
class KColorPatch;
class KarbonPart;
class KIntNumInput;

class VMDlgStroke : public QTabDialog
{
	Q_OBJECT
public:
	VMDlgStroke( KarbonPart *part );

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
	KarbonPart *m_part;

private slots:
	void slotUpdateFromRGBSpinBoxes();
	void slotUpdateFromHSVSpinBoxes();
	void slotApplyButtonPressed();
	void slotUpdate( QColor *color = 0L );
};

#endif

