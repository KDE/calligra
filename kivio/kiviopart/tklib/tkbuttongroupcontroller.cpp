#include "tkbuttongroupcontroller.h"

#include <qbuttongroup.h>
#include <qcheckbox.h>

TKButtonGroupController::TKButtonGroupController(QButtonGroup* parent, const char* name)
: QObject(parent,name), m_pGroup(parent)
{
  connect(m_pGroup,SIGNAL(clicked(int)),SLOT(slotClicked(int)));
}

TKButtonGroupController::~TKButtonGroupController()
{
}

void TKButtonGroupController::slotClicked(int id)
{
  QButton* b = m_pGroup->find(id);
  if (b->inherits("QCheckBox")) {
    if (((QCheckBox*)b)->isChecked()) {
      for (int i = 0; i < m_pGroup->count(); i++)
        if (i!=id && m_pGroup->find(i)->inherits("QCheckBox"))
          ((QCheckBox*)m_pGroup->find(i))->setChecked(false);
    }
  }
}
#include "tkbuttongroupcontroller.moc"
