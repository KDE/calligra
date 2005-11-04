
#include "koOpenPane.h"

#include <qvbox.h>

#include <klocale.h>
#include <kdeversion.h>
#include <kfiledialog.h>
#include <kinstance.h>
#include <kpushbutton.h>

#include "koFilterManager.h"
#include "koTemplates.h"
#include "koDocument.h"
#include "koDetailsPane.h"

class KoOpenPanePrivate
{
  public:
    KoOpenPanePrivate() :
      m_instance(0)
    {
    }

    KInstance* m_instance;
  
};

KoOpenPane::KoOpenPane(QWidget *parent, KInstance* instance, const QString& templateType)
  : KJanusWidget(parent, "OpenPane", TreeList)
{
  d = new KoOpenPanePrivate;
  d->m_instance = instance;

  addButtonBelowList(i18n("Open Existing File"), this, SLOT(showOpenFileDialog()));
  setRootIsDecorated(false);
  setShowIconsInTreeList(true);

  if(!templateType.isEmpty())
  {
    KoTemplateTree templateTree(templateType.local8Bit(), instance, true);

    for (KoTemplateGroup *group = templateTree.first(); group != 0L; group = templateTree.next()) {
      if (group->isHidden()) {
        continue;
      }

      QVBox* page = addVBoxPage(group->name(), group->name(),
        group->first()->loadPicture(instance));
      KoTemplatesPane* pane = new KoTemplatesPane(page, instance, group);
      connect(pane, SIGNAL(openTemplate(const QString&)), this, SIGNAL(openTemplate(const QString&)));
    }
  }
}

KoOpenPane::~KoOpenPane()
{
}

void KoOpenPane::showOpenFileDialog()
{
  const QStringList mimeFilter = KoFilterManager::mimeFilter(KoDocument::readNativeFormatMimeType(),
      KoFilterManager::Import,
      KoDocument::readExtraNativeMimeTypes());

  QString filename = KFileDialog::getOpenFileName(":OpenDialog", mimeFilter.join(" "), this);

  emit openExistingFile(filename);
}

// KoOpenPane::addTemplatesPanel(KoTemplateGroup *group)
// {
//     /*
//         instantiate new koTemplatesPane and add it to the m_widgetStack and a row to
//         the m_itemsList at position last, unless the customDocumentCreater is already
//         added, then its at position last - 1
//     */
// }
// 
// KoOpenPane::addCustomDocumentCreator(KoCustomDocumentCreator *cdc)
// {
//     /*
//     if (dynamic_cast(QWidget*) <cdc>) == 0) {
//         print error and return
//     }
// 
//     add it to the m_widgetStack and a row to the m_itemsList at position last
//     */
// }

#include "koOpenPane.moc"
