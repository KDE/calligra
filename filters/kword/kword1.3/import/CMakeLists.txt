




########### next target ###############

set(kwordkword1dot3import_PART_SRCS 
   kword13document.cpp 
   kword13parser.cpp 
   kword13frameset.cpp 
   kword13layout.cpp 
   kword13paragraph.cpp 
   kword13utils.cpp 
   kword13import.cpp 
   kword13oasisgenerator.cpp 
   kword13format.cpp 
   kword13formatone.cpp 
   kword13formatother.cpp 
   kword13picture.cpp 
   kword13postparsing.cpp )

kde4_add_plugin(kwordkword1dot3import WITH_PREFIX ${kwordkword1dot3import_PART_SRCS})



target_link_libraries(kwordkword1dot3import komain ${QT_QT3SUPPORT_LIBRARY})

install(TARGETS kwordkword1dot3import  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############


