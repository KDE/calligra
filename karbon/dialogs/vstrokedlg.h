/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSTROKEDLG_H__
#define __VSTROKEDLG_H__

#include <kdialogbase.h>

class QComboBox;
class QVButtonGroup;

class KarbonPart;
class TKUFloatSpinBox;
class VStroke;
class VColorTab;

class VStrokeDlg : public KDialogBase
{
	Q_OBJECT

public:
	VStrokeDlg( KarbonPart* part, QWidget* parent = 0L, const char* name = 0L );

private:
	VColorTab* m_colortab;
	KarbonPart *m_part;
	TKUFloatSpinBox *m_setLineWidth;
	QComboBox *m_styleCombo;
	QVButtonGroup *m_typeOption;
	QVButtonGroup *m_capOption;
	QVButtonGroup *m_joinOption;

protected:
	VStroke m_stroke;

signals:
	void strokeChanged( const VStroke & );
	
private slots:
	void slotTypeChanged( int ID );
	void slotCapChanged( int ID );
	void slotJoinChanged( int ID );
	void slotOKClicked();
	void slotUpdateDialog();

};

#endif

