#ifndef SimpleExport_h_
#define SimpleExport_h_

#include "ExportFilter.h"

/** 
 * An example of a very simple export filter.
 */
class SimpleExport : public ExportFilter {
public:
  SimpleExport ();
  ~SimpleExport ();
  
  bool setup (GDocument *doc, const char* fmt);
  bool exportToFile (GDocument *doc);
};

#endif
