//KoHyphenator test, Lukas Tinkl <lukas.tinkl@suse.cz>

#include <stdio.h>
#include <stdlib.h>

#include <qstringlist.h>

#include <kapplication.h>

#include "kohyphen.h"
#include <kdebug.h>

int main (int argc, char ** argv)
{
    KApplication app(argc, argv, "KoHyphenator test");

    //testing Czech language, this text is in UTF-8!
    QStringList cs_tests = QStringList() << "Žluťoučký" << "kůň" << "úpěl" <<
                        "ďábelské" << "ódy";

    //testing English
    QStringList en_tests = QStringList() << "Follow" << "white" << "rabbit";

    KoHyphenator * hypher = 0L;
    try {
        hypher = KoHyphenator::self();
    }
    catch (KoHyphenatorException &e)
    {
        kdDebug() << e.message() << endl;
        return 1;
    }

    QStringList::ConstIterator it = cs_tests.begin();

    while (it!=cs_tests.end()) {
        kdDebug() << (*it) << " hyphenates like this: " << hypher->hyphenate((*it), "cs") << endl;
        ++it;
    }

    it = en_tests.begin();

    while (it!=en_tests.end()) {
        kdDebug() << (*it) << " hyphenates like this: " << hypher->hyphenate((*it), "en") << endl;
        ++it;
    }

    return 0;
}
