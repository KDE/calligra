#ifndef __GRADIENTEDITORDLG_H__
#define __GRADIENTEDITORDLG_H__

#include "kfloatingdialog.h"

class GradientView;
class KImageShopDoc;

class GradientEditorDialog : public KFloatingDialog
{
  Q_OBJECT

public:

  GradientEditorDialog( KImageShopDoc *_doc, QWidget *_parent = 0 , const char *_name = 0 );
  ~GradientEditorDialog();

private:

  GradientView* m_pGradient;
  KImageShopDoc *m_pDoc;
};

#endif





