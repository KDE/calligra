#define UNX
#define LINUX
#define CPPU_ENV gcc3

#include <stdio.h>
#include <iostream>
#include <cppuhelper/bootstrap.hxx>
#include <osl/process.h>
#include <osl/file.hxx>
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
 
using namespace com::sun::star::bridge;
using namespace com::sun::star::container;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::uno;
using namespace cppu;
using namespace rtl;

#include <QtCore/QUrl>

void test()
{
    try {
        Reference<XComponentContext> rComponentContext(bootstrap());
        Reference<XMultiComponentFactory> xServiceManager(
                rComponentContext->getServiceManager());
        OUString d(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.frame.Desktop"));
        Reference<XComponentLoader> xComponentLoader(
                xServiceManager->createInstanceWithContext(d,
                     rComponentContext), UNO_QUERY_THROW);
/*
        OUString sAbsoluteDocUrl, sWorkingDir, sDocPathUrl;
        osl_getProcessWorkingDir(&sWorkingDir.pData);
        osl::FileBase::getFileURLFromSystemPath(OUString::createFromAscii(
                "private:levels.odp"), sDocPathUrl);
        osl::FileBase::getAbsoluteFileURL(sWorkingDir, sDocPathUrl,
                sAbsoluteDocUrl);
*/
        Sequence<PropertyValue> props(1);
        props[0].Name = OUString::createFromAscii("Hidden");
        props[0].Value = Any(true);

    QString from("/tmp/b.ppt");
    QString to("/tmp/out");
    QByteArray fromUrl(QUrl::fromLocalFile(from).toEncoded());
    QByteArray toUrl(QUrl::fromLocalFile(to+"/index.html").toEncoded());

        Reference<XComponent> xComponent
                = xComponentLoader->loadComponentFromURL(
                    //OUString::createFromAscii("file:///backup/backup/archive/dell/sda9/oever/Mir/Claessens-Delft-2007c.ppt"),
                    OUString::createFromAscii(fromUrl.constData()),
                    OUString::createFromAscii("_default"),
                    0, props);

        Sequence<PropertyValue> pngProps(3);
        pngProps[0].Name = OUString::createFromAscii("Format");
        pngProps[0].Value = Any(2); // png
        pngProps[1].Name = OUString::createFromAscii("Width");
        pngProps[1].Value = Any(640);
        pngProps[2].Name = OUString::createFromAscii("PublishMode");
        pngProps[2].Value = Any(0); // html

        Sequence<PropertyValue> htmlProps(3);
        htmlProps[0].Name = OUString::createFromAscii("Overwrite");
        htmlProps[0].Value = Any(true);
        htmlProps[1].Name = OUString::createFromAscii("FilterName");
        htmlProps[1].Value <<= OUString::createFromAscii("impress_html_Export");
        htmlProps[2].Name = OUString::createFromAscii("FilterData");
        htmlProps[2].Value <<= pngProps;

        Reference<XStorable> store(xComponent, UNO_QUERY);

        if (store.is()) {
            printf("storeis '%s'\n", toUrl.constData());
            OUString dest = OUString::createFromAscii(toUrl.constData());
            OUString dest_url;
            osl::FileBase::getFileURLFromSystemPath(dest , dest_url);
            store->storeToURL(dest, htmlProps);
        }
        // dispose the local service manager
        printf("Disposing of service\n");
        //Reference<XComponent>::query(xServiceManager)->dispose();
        //Reference<XComponent>::query(xServiceManager)->dispose();
        Reference<XDesktop>::query(xComponentLoader)->terminate();
    } catch(Exception &e) {
        OString o = OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US);
        printf( "Error: %s\n", o.pData->buffer );
        return;
    }
}

int
main() {
    test();
    return 0;
}
