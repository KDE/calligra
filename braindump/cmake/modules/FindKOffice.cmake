FIND_LIBRARY(KOMAIN_LIBRARY NAMES komain
  PATHS
  $ENV{KDEDIR}/lib
  ${KDE4PREFIX}/lib
  ${CMAKE_INSTALL_PREFIX}/lib
  /opt/kde4/lib
  /opt/kde/lib
  NO_DEFAULT_PATH
)

FIND_LIBRARY(PIGMENT_LIBRARY NAMES pigmentcms
  PATHS
  $ENV{KDEDIR}/lib
  ${KDE4PREFIX}/lib
  ${CMAKE_INSTALL_PREFIX}/lib
  /opt/kde4/lib
  /opt/kde/lib
  NO_DEFAULT_PATH
)


FIND_PATH(KOFFICE_INCLUDE_DIR NAMES kofficeversion.h
          PATHS
          ${INCLUDE_INSTALL_DIR}
          /usr/include
          /usr/local/include
            )
