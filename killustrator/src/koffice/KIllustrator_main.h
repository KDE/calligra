#ifndef KIllustrator_main_h_
#define KIllustrator_main_h_

#include <op_app.h>
#include <part_shell_impl.h>
#include <parts.h>

#include "KIllustrator_doc.h"

class KIllustratorOPApp : public OPApplication {
  Q_OBJECT
public:
  KIllustratorOPApp (int argc, char** argv);
  ~KIllustratorOPApp ();

  void start ();

protected:
};

#endif
