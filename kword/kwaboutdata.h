#include <kaboutdata.h>
#include <klocale.h>

static const char* KWORD_DESCRIPTION=I18N_NOOP("KOffice Word Processor");
// First official public release Oct 2000 version 0.8
static const char* KWORD_VERSION="0.9-devel";

KAboutData * newKWordAboutData()
{
    KAboutData * aboutData=new KAboutData( "kword", I18N_NOOP("KWord"),
                                             KWORD_VERSION, KWORD_DESCRIPTION, KAboutData::License_GPL,
                                             "(c) 1998-2000, Reginald Stadlbauer");
    aboutData->addAuthor("Reginald Stadlbauer", 0, "reggie@kde.org");
    aboutData->addAuthor("Thomas Zander", 0, "zander@earthling.net");
    aboutData->addAuthor("David Faure", 0, "david@mandrakesoft.com");
    aboutData->addAuthor("Laurent Montel", 0, "lmontel@mandrakesoft.com");
    return aboutData;
}
