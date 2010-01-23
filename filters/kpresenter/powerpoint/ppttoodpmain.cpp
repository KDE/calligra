#include "ppttoodp.h"
#include <KoOdf.h>
#include <QtCore/QCoreApplication>

int
main(int argc, char** argv)
{
    if (argc != 3) {
        return 1;
    }
    QCoreApplication app(argc, argv);

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];

    // open inputFile
    POLE::Storage storage(inputFile);
    if (!storage.open()) {
        qDebug() << "Cannot open " << inputFile;
        return KoFilter::StupidError;
    }
    // create output store
    KoStore* storeout = KoStore::createStore(outputFile, KoStore::Write,
                        KoOdf::mimeType(KoOdf::Presentation), KoStore::Zip);
    if (!storeout) {
        return KoFilter::FileNotFound;
    }

    PptToOdp ppttoodp;
    ppttoodp.convert(storage, storeout);
    delete storeout;

    return 0;
}
