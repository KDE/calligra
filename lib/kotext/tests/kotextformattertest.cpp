//KoTextFormatter test (also for profiling purposes), GPL v2, David Faure <faure@kde.org>

#include <kapplication.h>
#include <kdebug.h>

#include "kotextformatter.h"
#include "kotextformat.h"
#include "kotextdocument.h"
#include "kozoomhandler.h"

int main (int argc, char ** argv)
{
    KApplication app(argc, argv, "KoTextFormatter test");

    KoZoomHandler zh;
    KoTextFormatCollection* fc = new KoTextFormatCollection; // owned by the doc
    KoTextFormatter* formatter = new KoTextFormatter; // owned by the doc
    // Create test doc and paragraph
    KoTextDocument doc( &zh, fc, formatter );
    KoTextParag* parag = new KoTextParag( &doc ); // owned by the doc
    parag->append( "They burst into flames when it is time for them to die, and then they are reborn from the ashes" );

    // Format it
    for ( uint i = 0 ; i < 50 ; ++i )
    {
      parag->invalidate(0);
      parag->format();
    }

    return 0;
}
