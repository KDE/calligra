/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VFILLDLG_H__
#define __VFILLDLG_H__

#include <kdialogbase.h>

class QColor;
class QSpinBox;
class KHSSelector;
class KGradientSelector;
class KColorPatch;
class KarbonPart;
class KIntNumInput;

class VFillDlg : public KDialogBase
{
	Q_OBJECT

public:
	VFillDlg( KarbonPart* part, QWidget* parent = 0L, const char* name = 0L );

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

signals:
	void fillChanged();

private slots:
	void slotUpdateFromRGBSpinBoxes();
	void slotUpdateFromHSVSpinBoxes();
	void slotApplyButtonPressed();
	void slotUpdate(QColor *color = 0L);
    void slotVChanged( int );
	void slotHSChanged( int, int );
};

#endif

