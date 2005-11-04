#ifndef KOOPENPANE_H
#define KOOPENPANE_H

#include <kjanuswidget.h>

class KoCustomDocumentCreator;
class KConfig;
class KoTemplateGroup;
class KoOpenPanePrivate;
class KInstance;

class KoOpenPane : public KJanusWidget
{
  Q_OBJECT

  public:
    KoOpenPane(QWidget *parent, KInstance* instance, const QString& templateType = QString::null);
    virtual ~KoOpenPane();

//     void addCustomDocumentCreator(KoCustomDocumentCreator *cdc);

  protected slots:
    void showOpenFileDialog();

  signals:
    void openExistingFile(const QString&);
    void openTemplate(const QString&);

  protected:
//     void addTemplatesPanel(KoTemplateGroup *group);

  private:
    KoOpenPanePrivate* d;
};

/// Interface for the application specific CustomDocumentCreator panel.
class KoCustomDocumentCreator
{
public:
    KoCustomDocumentCreator(KoOpenPane *parent, KConfig *config);
    virtual ~KoCustomDocumentCreator() = 0;

    // some signals / slots to signal enter and OK pressed.
    // also we should add an OK button at the bottom.

    // note we can't extend QObject here as the implementing class will surely
    // be a widget (we actually expect it to be one!).  So we have to do
    // signal slots via the virtual methods way.
};

#endif //KOOPENPANE_H
