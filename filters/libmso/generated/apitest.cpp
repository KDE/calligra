#include "api.h"
#include "pole.h"
#include <QtCore/QDebug>

using namespace std;

void
testFile(const char* path) {
    POLE::Storage storage(path);
    if (!storage.open()) return;
    string prefix;
    if (storage.isDirectory("PP97_DUALSTORAGE")) {
        prefix = "PP97_DUALSTORAGE/";
    } else {
        prefix = "/";
    }
    list<string> entries = storage.entries(prefix);
    for (list<string>::const_iterator i=entries.begin(); i!=entries.end(); ++i) {
        // if encrypted, do not report failure.
        if (*i == "EncryptedSummary") return;
    }
    POLE::Stream stream(&storage, prefix + "PowerPoint Document");

    QByteArray array;
    array.resize(stream.size());
    unsigned long read = stream.read((unsigned char*)array.data(), stream.size());
    if (read != stream.size()) {
        qDebug() << "Error reading stream ";
        return;
    }
    MSO::PowerPointStructs s(array.data(), array.size());
    qDebug() << "Parsed " << s._size << " of " << array.size() << ": "
        << ((s._size == array.size())?"OK":"FAIL");
    if (s._size != array.size()) {
        qDebug() << path;
    }
}

static const char* const fixedstring = "ABC";

void testiterator() {
    MSO::TODOS t(fixedstring, 3);
    int total = 0;
    foreach (const MSO::Byte& b, t.anon) {
        qDebug() << b.b;
        total += b.b;
    }
    for (int i = 0; i < t.anon.size(); ++i) {
        qDebug() << t.anon[i].b;
        total += t.anon[i].b;
    }
    qDebug() << total;
}

int
main(int argc, char** argv) {
    //testiterator();
    for (int i=1; i<argc; ++i) {
        testFile(argv[i]);
    }
    return 0;
}
