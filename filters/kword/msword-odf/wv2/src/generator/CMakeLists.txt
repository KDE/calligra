# The generated files will be in the source directory ( src )
# IMHO polluting the source directory is a bad idea UNLESS the only
# point of the "make generated" target is to create word9{5,7}_generated.{h,cpp}
# for "make dist" (i. e. for a tarball)
# - Pau

ADD_CUSTOM_TARGET( scanner 
		  ${CMAKE_COMMAND} 
		  -DGENERATOR_DIR:PATH=${wvWare_SOURCE_DIR}/src/generator 
		  -P ${wvWare_SOURCE_DIR}/cmake/generate_scanner.cmake 
		)

ADD_CUSTOM_TARGET( converter 
		  ${CMAKE_COMMAND} 
		  -DGENERATOR_DIR:PATH=${wvWare_SOURCE_DIR}/src/generator 
		  -P ${wvWare_SOURCE_DIR}/cmake/generate_converter.cmake 
		)


ADD_CUSTOM_TARGET( generated DEPENDS scanner converter )
