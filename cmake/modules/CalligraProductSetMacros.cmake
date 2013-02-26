# Copyright (c) 2013 Friedrich W. H. Kossebau <kossebau@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

macro(calligra_set_productset productset_name)
  # be gracefull and compare the productset name case insensitive, by lowercasing
  # also expects the productset definition filename in all lowercase
  string(TOLOWER "${productset_name}" lowercase_productset_name)
  set(productset_filename "${CMAKE_SOURCE_DIR}/cmake/productsets/${lowercase_productset_name}.cmake")

  if (NOT EXISTS "${productset_filename}")
     message(FATAL_ERROR "Unknown product set: ${productset_name}")
  endif (NOT EXISTS "${productset_filename}")

  # include the productset definition
  include(${productset_filename})

  message(STATUS "--------------------------------------------------------------------------" )
  message(STATUS "Configured with product set \"${productset_name}\"")
  message(STATUS "--------------------------------------------------------------------------" )

  # mark all products of the set as SHOULD_BUILD
  foreach(product ${CALLIGRA_SHOULD_BUILD_PRODUCTS})
    # check that this product is actually existing
    if (NOT DEFINED SHOULD_BUILD_${product})
      message(FATAL_ERROR "Unknown product: ${product}")
    endif (NOT DEFINED SHOULD_BUILD_${product})
    # TODO: check if product was initialized before
    set(SHOULD_BUILD_${product} TRUE)
  endforeach(product)
endmacro()

macro(calligra_init_should_build)
  foreach(product ${ARGV})
    set(SHOULD_BUILD_${product} FALSE)
  endforeach(product)
endmacro(calligra_init_should_build)

macro(calligra_log_should_build)
  message(STATUS "---------------- The following products will be built --------------------" )
  foreach(product ${ARGV})
    string(TOLOWER "${product}" lowercase_product_name)
    if (BUILD_${lowercase_product_name} AND SHOULD_BUILD_${product} MATCHES "[Tt][Rr][Uu][Ee]")
      message(STATUS ${lowercase_product_name} )
    endif (BUILD_${lowercase_product_name} AND SHOULD_BUILD_${product} MATCHES "[Tt][Rr][Uu][Ee]")
  endforeach(product)
  message(STATUS "--------------------------------------------------------------------------" )
endmacro(calligra_log_should_build)
