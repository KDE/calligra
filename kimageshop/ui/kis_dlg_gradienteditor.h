#ifndef __GRADIENTEDITORDLG_H__
#define __GRADIENTEDITORDLG_H__

#include "kfloatingdialog.h"

class GradientView;
class KisDoc;

class GradientEditorDialog : public KFloatingDialog
{
  Q_OBJECT

public:

  GradientEditorDialog( KisDoc *_doc, QWidget *_parent = 0 , const char *_name = 0 );
  ~GradientEditorDialog();

private:

  GradientView* m_pGradient;
  KisDoc *m_pDoc;
};

#endif





