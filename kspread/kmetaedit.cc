#include "kmetaedit.h"
#include "kspread_emacs.h"

KMetaEditor* createEditor()
{
  return new KEmacs;
}
