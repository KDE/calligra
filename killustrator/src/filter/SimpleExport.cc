#include "SimpleExport.h"

SimpleExport::SimpleExport () {
}

SimpleExport::~SimpleExport () {
}

bool SimpleExport::setup (GDocument *doc, const char* fmt) {
  // Initialize the filter...
  printf ("setup\n");
  return true;
}

bool SimpleExport::exportToFile (GDocument* doc) {
  // Export the document
  // A real filter should provide an instance of QPainter and draw
  // all objects.
  printf ("export %d objects\n", doc->objectCount ());
  QListIterator<GObject> it = doc->getObjects ();
  for (; it.current (); ++it) {
    printf ("object = %s\n", it.current ()->className ());
  }
  return true;
}
