KDE_CXXFLAGS = $(USE_RTTI) -fexceptions -g3  -DEASYSOAP_SSL -fPIC
#for expat we will want to get this from configure some day
INCLUDES  = -I/usr/local/include -I/usr/include/openssl

lib_LTLIBRARIES = libeasysoap.la

libeasysoap_la_SOURCES = SOAPArrayHandler.cpp SOAPBase64.cpp SOAPBody.cpp \
        SOAPBodyHandler.cpp SOAPCGITransport.cpp SOAPClientSocketImp.cpp \
        SOAPDebugger.cpp SOAPEnvelope.cpp SOAPEnvelopeHandler.cpp \
        SOAPFault.cpp SOAPFaultHandler.cpp SOAPHeader.cpp \
        SOAPHeaderHandler.cpp SOAPMethod.cpp SOAPMethodHandler.cpp \
        SOAPPacketWriter.cpp SOAPParameter.cpp SOAPParameterHandler.cpp \
        SOAPParse.cpp SOAPProxy.cpp SOAPQName.cpp \
        SOAPResponse.cpp SOAPSecureSocketImp.cpp SOAPServerDispatch.cpp \
        SOAPSocket.cpp SOAPStructHandler.cpp SOAPUrl.cpp \
        SOAPonHTTP.cpp XMLParser.cpp

libeasysoap_la_LDFLAGS = -g3 -fexceptions
libeasysoap_la_LIBADD = -lssl -lcrypto -lexpat

METASOURCES = AUTO

noinst_HEADERS = SOAP.h SOAPDispatchHandler.h SOAPMethod.h SOAPProxy.h SOAPString.h config/* \
	SOAPArray.h SOAPEnvelope.h SOAPNamespaces.h SOAPQName.h SOAPTransport.h \
	SOAPBase64.h SOAPException.h SOAPPacketWriter.h SOAPResponse.h SOAPUrl.h \
	SOAPBody.h SOAPFault.h SOAPParameter.h SOAPServerDispatch.h SOAPUtil.h \
	SOAPCGIHandler.h SOAPHashMap.h SOAPParse.h SOAPSocket.h SOAPonHTTP.h \
	SOAPDebugger.h SOAPHeader.h SOAPParseEventHandler.h SOAPStack.h XMLParser.h

srcdoc: $(noinst_HEADERS)
	kdoc -f html  -d api-docs/  $(noinst_HEADERS)
