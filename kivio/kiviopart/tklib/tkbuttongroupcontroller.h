#ifndef TKBUTTONGROUPCONTROLLER_H
#define TKBUTTONGROUPCONTROLLER_H

#include <qobject.h>

class QButtonGroup;

class TKButtonGroupController : public QObject
{ Q_OBJECT
public:
  TKButtonGroupController(QButtonGroup* parent, const char* name=0);
  ~TKButtonGroupController();

protected slots:
  void slotClicked(int);

private:
  QButtonGroup* m_pGroup;
};

#endif

