#include "KIllustrator_main.h"
#include "KIllustrator_main.moc"
#include "KIllustrator.h"
#include "KIllustrator_shell.h"

#include <koScanParts.h>
#include <factory_impl.h>

bool withGUI = true;

FACTORY(KIllustratorDocument, KIllustrator::Factory_skel, KIllustratorFactory)

typedef AutoLoader<KIllustratorFactory> KIllustratorAutoLoader;

KIllustratorOPApp::KIllustratorOPApp (int argc, char** argv) :
  OPApplication (argc, argv) {
}

KIllustratorOPApp::~KIllustratorOPApp () {
}

void KIllustratorOPApp::start () {
  koScanParts ();

  if (withGUI) {
    KIllustratorShell* m_pShell;

    m_pShell = new KIllustratorShell;
    m_pShell->enableMenuBar ();
    m_pShell->PartShell_impl::enableStatusBar ();
    m_pShell->enableToolBars ();

    m_pShell->show ();
  }
}

int main (int argc, char** argv) {
  for (int i = 1; i < argc; i++) {
    if (::strcmp (argv[i], "-s") == 0 || 
	::strcmp (argv[i], "--server") == 0)
      withGUI = false;
  }

  KIllustratorAutoLoader loader ("IDL:KIllustrator/Factory:1.0");
  KIllustratorOPApp app (argc, argv);
  app.exec ();

  return 0;
}
