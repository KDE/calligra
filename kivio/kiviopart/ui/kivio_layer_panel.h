#ifndef KIVIOLAYERPANEL_H
#define KIVIOLAYERPANEL_H

class KAction;

class KivioView;
class KivioLayer;

#include <qlistview.h>

#include "kivio_layer_panel_base.h"

class KivioLayerItem: public QListViewItem
{
friend class KivioLayerPanel;
public:
  KivioLayerItem(QListView* parent, KivioLayer*, int id);
  ~KivioLayerItem();

protected:
  void update();

private:
  KivioLayer* data;
};
/*********************************************************/
class KivioLayerPanel : public KivioLayerPanelBase
{ Q_OBJECT
public:
  KivioLayerPanel(KivioView* view, QWidget* parent=0, const char* name=0);
  ~KivioLayerPanel();

  bool eventFilter(QObject*, QEvent*);

public slots:
  void reset();

protected slots:
  void addItem();
  void removeItem();
  void renameItem();
  void upItem();
  void downItem();

  void itemClicked(QListViewItem*, const QPoint&, int);
  void itemActivated(QListViewItem*);
  void updateButtons(QListViewItem*);

private:
  KivioView* m_pView;

  KAction* actNew;
  KAction* actDel;
  KAction* actRename;
  KAction* actUp;
  KAction* actDown;

  int id;
};

#endif
