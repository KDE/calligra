#include <kunittest/runner.h>
#include <kunittest/module.h>
#include <kdebug.h>

#include "KWPageManagerTester.h"

#include <KWPageManager.h>
#include <KWPage.h>

using namespace KUnitTest;

KUNITTEST_MODULE(kunittest_KWPageManagerTester, "PageManager Tester");
KUNITTEST_MODULE_REGISTER_TESTER(KWPageManagerTester);

void KWPageManagerTester::allTests() {
    KWPageManager *pageManager = new KWPageManager();
    KWPage *page = pageManager->pageAt(0);
    CHECK(page == 0, true);
    page = pageManager->pageAt(2);
    CHECK(page == 0, true);

    page = pageManager->pageAt(1);
    CHECK(page == 0, false);
    CHECK(page->pageNum(), 1);
    CHECK(page->pageSide(), KWPage::Right);

    page->setWidth(134.2);
    page->setHeight(521.4);
    CHECK(page->size().right(), 134.2);
    CHECK(page->size().bottom(), 521.4);

    // test setStartPage
    pageManager->setStartPage(4);
    page = pageManager->pageAt(0);
    CHECK(page == 0, true);
    page = pageManager->pageAt(3);
    CHECK(page == 0, true);
    page = pageManager->pageAt(5);
    CHECK(page == 0, true);
    page = pageManager->pageAt(4);
    CHECK(page == 0, false);
    CHECK(page->pageNum(), 4);
    CHECK(page->pageSide(), KWPage::Left);

    pageManager->setStartPage(1);
    page = pageManager->pageAt(0);
    CHECK(page == 0, true);
    page = pageManager->pageAt(3);
    CHECK(page == 0, true);
    page = pageManager->pageAt(2);
    CHECK(page == 0, true);
    page = pageManager->pageAt(1);
    CHECK(page == 0, false);
    CHECK(page->pageNum(), 1);
    CHECK(page->pageSide(), KWPage::Right);

    // adding pages
    CHECK(pageManager->numPages(), 1);
    CHECK(pageManager->lastPageNumber(), 1);
    pageManager->setStartPage(40);
    CHECK(pageManager->numPages(), 1);
    CHECK(pageManager->lastPageNumber(), 40);
    page = pageManager->createPage();
    CHECK(pageManager->numPages(), 2);
    CHECK(pageManager->lastPageNumber(), 41);

    CHECK(page == 0, false);
    CHECK(page->pageNum(), 41);
}
