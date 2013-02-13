project( kastencore )

add_definitions( -DKDE_DEFAULT_DEBUG_AREA=1803 )

include_directories(
  entity
  document
  io
  io/filesystem
  system
)

set( KASTEN_CORE_ENTITY_SRCS
#   entity/group.cpp
  entity/person.cpp
#   entity/abstractentity.cpp
)

set( KASTEN_CORE_DOCUMENT_SRCS
  document/abstractmodel.cpp
  document/abstractmodelselection.cpp
  document/abstracttool.cpp
  document/abstractdocument.cpp
  document/abstractdocumentfactory.cpp
  document/documentversiondata.cpp
)

set( KASTEN_CORE_IO_SRCS
  io/abstractloadjob.cpp
  io/abstractconnectjob.cpp
  io/abstractsynctoremotejob.cpp
  io/abstractsyncfromremotejob.cpp
  io/abstractsyncwithremotejob.cpp
  io/abstractmodelstreamencoder.cpp
  io/abstractexportjob.cpp
  io/abstractmodelexporter.cpp
  io/modeldatageneratethread.cpp
  io/abstractmodeldatagenerator.cpp
  io/modelstreamencodethread.cpp
  io/abstractmodelsynchronizer.cpp
  io/abstractmodelsynchronizerfactory.cpp
)

set( KASTEN_CORE_IO_FILESYSTEM_SRCS
  io/filesystem/abstractfilesystemloadjob_p.cpp
  io/filesystem/abstractfilesystemloadjob.cpp
  io/filesystem/abstractfilesystemsynctoremotejob_p.cpp
  io/filesystem/abstractfilesystemsynctoremotejob.cpp
  io/filesystem/abstractfilesystemsyncfromremotejob_p.cpp
  io/filesystem/abstractfilesystemsyncfromremotejob.cpp
  io/filesystem/abstractfilesystemsyncwithremotejob_p.cpp
  io/filesystem/abstractfilesystemsyncwithremotejob.cpp
  io/filesystem/abstractfilesystemconnectjob_p.cpp
  io/filesystem/abstractfilesystemconnectjob.cpp
  io/filesystem/abstractfilesystemexportjob_p.cpp
  io/filesystem/abstractfilesystemexportjob.cpp
  io/filesystem/modelencoderfilesystemexportjob_p.cpp
  io/filesystem/modelencoderfilesystemexportjob.cpp
  io/filesystem/modelencoderfilesystemexporter.cpp
  io/filesystem/abstractmodelfilesystemsynchronizer_p.cpp
  io/filesystem/abstractmodelfilesystemsynchronizer.cpp
)

set( KASTEN_CORE_SYSTEM_SRCS
  system/jobmanager.cpp
  system/documentmanager.cpp
  system/documentcreatemanager.cpp
  system/documentsyncmanager.cpp
  system/modelcodecmanager.cpp
)

set( kastencore_LIB_SRCS
  kastencore.cpp
  ${KASTEN_CORE_ENTITY_SRCS}
  ${KASTEN_CORE_DOCUMENT_SRCS}
  ${KASTEN_CORE_IO_SRCS}
  ${KASTEN_CORE_IO_FILESYSTEM_SRCS}
  ${KASTEN_CORE_SYSTEM_SRCS}
)

set( kastencore_LIB_HDRS
  kastencore_export.h
  kastencore.h
  document/abstractmodel.h
  document/abstractmodelselection.h
  document/abstractdocument.h
  document/abstractdocumentfactory.h
  document/abstracttool.h
  document/abstracttoolfactory.h
  document/documentversiondata.h
  document/documentversionid.h
  document/versionable.h
  document/userlistable.h
  entity/person.h
  io/abstractsynctoremotejob.h
  io/abstractmodelsynchronizer.h
  io/abstractsyncwithremotejob.h
  io/filesystem/abstractfilesystemsyncwithremotejob.h
  io/filesystem/abstractfilesystemsyncfromremotejob.h
  io/filesystem/abstractfilesystemsynctoremotejob.h
  io/filesystem/abstractmodelfilesystemsynchronizer.h
  io/filesystem/abstractfilesystemconnectjob.h
  io/filesystem/abstractfilesystemloadjob.h
  io/filesystem/abstractfilesystemexportjob.h
  io/filesystem/modelencoderfilesystemexporter.h
  io/filesystem/modelencoderfilesystemexportjob.h
  io/abstractsyncfromremotejob.h
  io/abstractconnectjob.h
  io/abstractmodelstreamencoder.h
  io/abstractmodeldatagenerator.h
  io/abstractexportjob.h
  io/abstractmodelexporter.h
  io/modelstreamencodethread.h
  io/modeldatageneratethread.h
  io/abstractloadjob.h
  io/abstractmodelsynchronizerfactory.h
  system/abstractoverwritedialog.h
  system/abstractsavediscarddialog.h
  system/jobmanager.h
  system/documentmanager.h
  system/documentcreatemanager.h
  system/documentsyncmanager.h
  system/modelcodecmanager.h
)

kde4_add_library( ${kastencore_LIB} SHARED ${kastencore_LIB_SRCS} )
target_link_libraries( ${kastencore_LIB}
  ${KDE4_KIO_LIBS}
  ${KDE4_SOLID_LIBS}
)
set_target_properties( ${kastencore_LIB}  PROPERTIES
  OUTPUT_NAME ${kastencore_LIB_NAME}
  VERSION     ${KASTEN_LIB_VERSION}
  SOVERSION   ${KASTEN_LIB_SOVERSION}
)

install( TARGETS ${kastencore_LIB}  ${INSTALL_TARGETS_DEFAULT_ARGS} )

install( FILES ${kastencore_LIB_HDRS}
  DESTINATION ${KASTEN_INCLUDE_INSTALL_DIR}
  COMPONENT   Devel
)

if(KDE4_BUILD_TESTS)
  add_subdirectory( tests )
endif(KDE4_BUILD_TESTS)
