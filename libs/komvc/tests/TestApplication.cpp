#include "TestApplication.h"
#include "TestDocument.h"
#include "TestView.h"
#include "TestMainWindow.h"

TestApplication::TestApplication(int &argc, char **argv)
    : KoApplicationBase(argc, argv)
{
}

void TestApplication::initialize()
{
    KoApplicationBase::initialize(new TestMainWindowFactory(), new TestDocumentFactory(), new TestViewFactory());
}
