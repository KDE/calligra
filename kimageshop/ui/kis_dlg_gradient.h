#ifndef __GRADIENTDLG_H__
#define __GRADIENTDLG_H__

#include "kfloatingdialog.h"

class KImageShopDoc;

class GradientDialog : public KFloatingDialog
{
  Q_OBJECT

public:

	GradientDialog( kisDoc *_doc, QWidget *_parent = 0 , const char *_name = 0 );
	~GradientDialog();

private:

  kisDoc *m_pDoc;
};

#endif
