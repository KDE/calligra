#ifndef STENCILSBAROPTIONSDIALOG_H
#define STENCILSBAROPTIONSDIALOG_H

#include "stencilsbaroptionsdialogbase.h"

#include <kurl.h>
#include <qscrollview.h>
#include <qpixmap.h>

class KivioView;

class Preview : public QScrollView
{ Q_OBJECT
public:
  Preview(QWidget* parent);

public slots:
  void showPreview(const KURL& url);

protected:
  void drawContents(QPainter*, int, int, int, int);

private:
  QPixmap pixmap;
};
/*******************************************************************************************************/
class StencilsBarOptionsDialog : public StencilsBarOptionsDialogBase
{ Q_OBJECT
public:
  StencilsBarOptionsDialog(KivioView* view, QWidget* parent=0, const char* name=0);
  ~StencilsBarOptionsDialog();

public slots:
  void apply(QWidget*);

protected slots:
  void selectSource();
  void slotOpenButton();
  void updatePreview();

protected:
  void updateEnabledState();

private:
  KivioView* m_pView;
};

#endif // STENCILSBAROPTIONSDIALOG_H
