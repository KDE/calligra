#ifndef KEXIABOUTDATA_H
#define KEXIABOUTDATA_H

#include <kaboutdata.h>
#include <klocale.h>
#include <kofficeversion.h>

static const char* KEXIDESCRIPTION = I18N_NOOP("KOffice Database Application");
static const char* KEXIVERSION = KOFFICE_VERSION_STRING;

KAboutData* newAboutData()
{
    KAboutData * aboutData = new KAboutData( "kexi", I18N_NOOP("Kexi"),
                                             KEXIVERSION, KEXIDESCRIPTION, KAboutData::License_LGPL,
                                             I18N_NOOP("(c) 2002-2007, The Kexi Team"), 0,
               "http://www.koffice.org/kexi/");
    //aboutData->addAuthor("Name", 0, "my@mail.address");
    //aboutData->addCredit("Other Name", "bla bla");
    return aboutData;
}

#endif
