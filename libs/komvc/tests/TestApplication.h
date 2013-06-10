#ifndef TESTAPPLICATION_H
#define TESTAPPLICATION_H

#include <KoApplicationBase.h>

class TestApplication : public KoApplicationBase
{
    Q_OBJECT
public:
    TestApplication(int &argc, char **argv);

    void initialize();
};

#endif // TESTAPPLICATION_H
