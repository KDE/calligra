#ifndef __GRADIENTDLG_H__
#define __GRADIENTDLG_H__

#include "kfloatingdialog.h"

class KImageShopDoc;

class GradientDialog : public KFloatingDialog
{
  Q_OBJECT

public:

	GradientDialog( KisDoc *_doc, QWidget *_parent = 0 , const char *_name = 0 );
	~GradientDialog();

private:

  KisDoc *m_pDoc;
};

#endif
