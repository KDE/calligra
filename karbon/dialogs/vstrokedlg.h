/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSTROKEDLG_H__
#define __VSTROKEDLG_H__

#include <qtabdialog.h>

class QColor;
class QSpinBox;
class KHSSelector;
class KGradientSelector;
class KColorPatch;
class KarbonPart;
class KIntNumInput;

class VStrokeDlg : public QTabDialog
{
	Q_OBJECT

public:
	VStrokeDlg( KarbonPart* part, QWidget* parent = 0L, const char* name = 0L );

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
	void strokeChanged( const VStroke & );

private slots:
	void slotUpdateFromRGBSpinBoxes( int );
	void slotUpdateFromHSVSpinBoxes( int );
	void slotApplyButtonPressed();
	void slotUpdate( QColor *color = 0L );
	void slotVChanged( int );
	void slotHSChanged( int, int );
};

#endif

