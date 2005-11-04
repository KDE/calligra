#ifndef KODETAILSPANE_H
#define KODETAILSPANE_H

#include "koDetailsPaneBase.h"

class KoTemplateGroup;
class KInstance;
class QListViewItem;

class KoTemplatesPane : public KoDetailsPaneBase
{
  Q_OBJECT
  public:
    KoTemplatesPane(QWidget* parent, KInstance* instance, KoTemplateGroup* group);
//     ~KoTemplatesPane();


  signals:
    void openTemplate(const QString&);

  protected slots:
    void selectionChanged(QListViewItem* item);
    void openTemplate();
    void openTemplate(QListViewItem* item);
};


class KoRecentDocumentsPane : public KoDetailsPaneBase
{
  Q_OBJECT
  public:
    KoRecentDocumentsPane(QWidget* parent, KInstance* instance);
//     ~KoRecentDocumentsPane();
};

#endif //KODETAILSPANE_H
