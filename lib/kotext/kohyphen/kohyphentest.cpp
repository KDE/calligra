//KoHyphenator test, Lukas Tinkl <lukas.tinkl@suse.cz>

#include <stdio.h>
#include <stdlib.h>

#include <qstringlist.h>

#include <kapplication.h>

#include "kohyphen.h"

int main (int argc, char ** argv)
{
    KApplication app(argc, argv, "KoHyphenator test");

    //testing Czech language, this text is in UTF-8!
    QStringList cs_tests = QStringList() << "Žluťoučký" << "kůň" << "úpěl" <<
                        "ďábelské" << "ódy";

    //testing English
    QStringList en_tests = QStringList() << "Follow" << "white" << "rabbit";

    KoHyphenator * hypher = new KoHyphenator();

    QStringList::ConstIterator it = cs_tests.begin();

    while (it!=cs_tests.end()) {
        qDebug("%s hyphenates like this: %s", (*it).latin1(),
               hypher->hyphenate((*it), "cs").latin1());
        ++it;
    }

    it = en_tests.begin();

    while (it!=en_tests.end()) {
        qDebug("%s hyphenates like this: %s", (*it).latin1(),
               hypher->hyphenate((*it), "en").latin1());
        ++it;
    }

    delete hypher;

    return 0;
}
