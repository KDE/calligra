#ifndef __GRADIENTDLG_H__
#define __GRADIENTDLG_H__

#include "kfloatingdialog.h"

class KImageShopDoc;

class GradientDialog : public KFloatingDialog
{
  Q_OBJECT

public:

	GradientDialog( KImageShopDoc *_doc, QWidget *_parent = 0 , const char *_name = 0 );
	~GradientDialog();

private:

  KImageShopDoc *m_pDoc;
};

#endif
