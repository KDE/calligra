include( ../kdchartserializer.pri )

TEMPLATE = lib
TARGET = kdchartserializer
DESTDIR	= ../../lib

QT += xml

# use qmake CONFIG+=dll to compile
# the kdchart serializer library as a DLL
dll {
	DEFINES += KDCHARTSERIALIZER_DLL
}
DLLDESTDIR = ../../bin

DEFINES += KDCHART_BUILD_KDCHARTSERIALIZER_LIB

KDCHART_PATH = ../..
INCLUDEPATH+= ../include \
            $(QTDIR)/src/3rdparty/zlib \
            $$KDCHART_PATH/include \
            $$KDCHART_PATH/src \
            .

KDCHARTLIB = kdchart
CONFIG(debug, debug|release) {
    !unix: KDCHARTLIB = "kdchartd"
}

DEPENDPATH += ../include \
            $(QTDIR)/src/3rdparty/zlib \
            $$KDCHART_PATH/include \
            .
LIBS        += -L$$KDCHART_PATH/lib -l$$KDCHARTLIB


SOURCES= \
        KDChartSerializer.cpp \
        KDChartSerializeCollector.cpp \
        KDChartIdMapper.cpp \
        KDChartAttributesSerializer.cpp \
        KDChartAttributesModelSerializer.cpp \
        KDChartAbstractAreaBaseSerializer.cpp \
        KDChartCoordPlanesSerializer.cpp \
        KDChartDiagramsSerializer.cpp \
        KDChartAxesSerializer.cpp \
        KDChartTextAreaSerializer.cpp \
        KDChartLegendsSerializer.cpp \
        KDXMLTools.cpp \
        KDChartDiagramSerializerFactory.cpp \
        KDChartLegendSerializerFactory.cpp \
        KDChartTextAreaSerializerFactory.cpp \
        KDChartAxisSerializerFactory.cpp \
        KDChartCoordPlaneSerializerFactory.cpp \
        KDChartAbstractSerializerFactory.cpp \

HEADERS= \
        kdchartserializer_export.h \
        KDChartSerializer.h \
        KDChartSerializer_p.h \
        KDChartSerializeCollector.h \
        KDChartSerializeCollector_p.h \
        KDChartIdMapper.h \
        KDChartIdMapper_p.h \
        KDChartAttributesSerializer.h \
        KDChartAttributesSerializer_p.h \
        KDChartAttributesModelSerializer.h \
        KDChartAttributesModelSerializer_p.h \
        KDChartAbstractAreaBaseSerializer.h \
        KDChartAbstractAreaBaseSerializer_p.h \
        KDChartCoordPlanesSerializer.h \
        KDChartCoordPlanesSerializer_p.h \
        KDChartDiagramsSerializer.h \
        KDChartDiagramsSerializer_p.h \
        KDChartAxesSerializer.h \
        KDChartAxesSerializer_p.h \
        KDChartTextAreaSerializer.h \
        KDChartTextAreaSerializer_p.h \
        KDChartLegendsSerializer.h \
        KDChartLegendsSerializer_p.h \
        KDXMLTools.h \
        KDChartDiagramSerializerFactory.h \
        KDChartDiagramSerializerFactory_p.h \
        KDChartLegendSerializerFactory.h \
        KDChartLegendSerializerFactory_p.h \
        KDChartTextAreaSerializerFactory.h \
        KDChartTextAreaSerializerFactory_p.h \
        KDChartAxisSerializerFactory.h \
        KDChartAxisSerializerFactory_p.h \
        KDChartCoordPlaneSerializerFactory.h \
        KDChartCoordPlaneSerializerFactory_p.h \
        KDChartAbstractSerializerFactory.h \
        KDChartSerializerRegistrator.h \

