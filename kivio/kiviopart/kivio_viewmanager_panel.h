#ifndef KIVIO_VIEWMANAGER_PANEL_H
#define KIVIO_VIEWMANAGER_PANEL_H

#include <qlistview.h>
#include <qptrlist.h>
#include <koRect.h>

class KAction;
class KivioView;
class QDomElement;

namespace Kivio {

struct ViewItemData
{
  QString name;
  int id;
  int pageId;
  KoRect rect;
  bool isZoom;
  bool isPage;
};
/*********************************************************/
typedef QPtrList<ViewItemData> ViewItemDataList;

class ViewItemList : public QObject
{ Q_OBJECT
public:
  ViewItemList(QObject* parent=0, const char* name=0);
  ~ViewItemList();

  void save(QDomElement&);
  void load(const QDomElement&);

  ViewItemDataList data() const { return list; }

  int add(ViewItemData*);
  void remove(ViewItemData*);
  void update(ViewItemData*);

  void activate(ViewItemData*, KivioView*);

signals:
  void itemAdd(ViewItemData*);
  void itemRemoved(ViewItemData*);
  void itemChanged(ViewItemData*);
  void reset();

private:
  ViewItemDataList list;
  int freeId;
};

}

using namespace Kivio;

/*********************************************************/
class KivioViewItem: public QListViewItem
{
friend class KivioViewManagerPanel;
public:
  KivioViewItem(QListView* parent, ViewItemData*);
  ~KivioViewItem();

protected:
  void update();

private:
  ViewItemData* data;
};
/*********************************************************/
class KivioViewManagerPanel : public QWidget
{ Q_OBJECT
public:
  KivioViewManagerPanel(KivioView* view, QWidget* parent=0, const char* name=0);
  virtual ~KivioViewManagerPanel();

  bool eventFilter(QObject*, QEvent*);

protected slots:
  void addItem();
  void removeItem();
  void renameItem();
  void upItem();
  void downItem();

  void itemAdd(ViewItemData*);
  void itemRemoved(ViewItemData*);
  void itemChanged(ViewItemData*);
  void reset();

  void itemClicked(QListViewItem* , const QPoint&, int);
  void itemActivated(QListViewItem*);

  void updateButtons(QListViewItem*);

private:
  KivioView* m_pView;
  QListView* list;
  Kivio::ViewItemList* viewItems;

  KAction* actNew;
  KAction* actDel;
  KAction* actRename;
  KAction* actUp;
  KAction* actDown;
};

#endif

