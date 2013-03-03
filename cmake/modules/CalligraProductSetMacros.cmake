# Copyright (c) 2013 Friedrich W. H. Kossebau <kossebau@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

macro(calligra_set_shouldbuild_productdependencies _productset_id_dependencies _product_id)
  message(STATUS "-> ${_productset_id_dependencies}")
  foreach(_dep_product_id ${_productset_id_dependencies})
    message(STATUS "++++++++++++ ${_dep_product_id}")
    list(APPEND CALLIGRA_PRODUCT_${_dep_product_id}_dependents ${_product_id})
    # if not already enabled, enable
    if (NOT ${SHOULD_BUILD_${_dep_product_id}})
      set(SHOULD_BUILD_${_dep_product_id} TRUE)
      if (DEFINED CALLIGRA_PRODUCT_${_dep_product_id}_dependencies)
        calligra_set_shouldbuild_productdependencies("${CALLIGRA_PRODUCT_${_dep_product_id}_dependencies}" ${_dep_product_id})
      endif (DEFINED CALLIGRA_PRODUCT_${_dep_product_id}_dependencies)
    endif (NOT ${SHOULD_BUILD_${_dep_product_id}})
  endforeach(_dep_product_id)
endmacro()

macro(calligra_drop_incomplete_products)
# can assume calligra_all_products has products in down-up order
endmacro()

macro(calligra_set_productset _productset_id)
  # be gracefull and compare the productset id case insensitive, by lowercasing
  # also expects the productset definition filename in all lowercase
  string(TOLOWER "${_productset_id}" lowercase_productset_id)
  set(productset_filename "${CMAKE_SOURCE_DIR}/cmake/productsets/${lowercase_productset_id}.cmake")

  if (NOT EXISTS "${productset_filename}")
     message(FATAL_ERROR "Unknown product set: ${_productset_id}")
  endif (NOT EXISTS "${productset_filename}")

  # include the productset definition
  include(${productset_filename})

  message(STATUS "--------------------------------------------------------------------------" )
  message(STATUS "Configured with product set \"${_productset_id}\"")
  message(STATUS "--------------------------------------------------------------------------" )

  # mark all products of the set as SHOULD_BUILD
  foreach(_product_id ${CALLIGRA_SHOULD_BUILD_PRODUCTS})
    # check that this product is actually existing
    if (NOT DEFINED SHOULD_BUILD_${_product_id})
      message(FATAL_ERROR "Unknown product: ${_product_id}")
    endif (NOT DEFINED SHOULD_BUILD_${_product_id})

    # mark product as should build, also all dependencies
    set(SHOULD_BUILD_${_product_id} TRUE)
    message(STATUS "=========== ${_product_id} (CALLIGRA_PRODUCT_${_product_id}_dependencies:${CALLIGRA_PRODUCT_${_product_id}_dependencies})")
    if (DEFINED CALLIGRA_PRODUCT_${_product_id}_dependencies)
        calligra_set_shouldbuild_productdependencies("${CALLIGRA_PRODUCT_${_product_id}_dependencies}" ${_product_id})
    endif (DEFINED CALLIGRA_PRODUCT_${_product_id}_dependencies)
  endforeach(_product_id)
endmacro()

# Usage:
#   calligra_define_product(<product_id>
#         [NAME] <product_name>
#         [DEPENDS <product_id1> [<product_id2> ...]]
#       )
macro(calligra_define_product _product_id)
  # default product name to id, empty deps
  set(_product_name "${_product_id}")
  set(_dep_product_ids)

  # parse arguments: two states, either "name" or "depends"
  set(_current_arg_type "name")
  foreach(_arg ${ARGN})
    if(${_arg} STREQUAL "NAME")
      set(_current_arg_type "name")
    elseif(${_arg} STREQUAL "DEPENDS")
      set(_current_arg_type "depends")
    else(${_arg} STREQUAL "NAME")
      if(${_current_arg_type} STREQUAL "name")
        set(_product_name "${_arg}")
      else(${_current_arg_type} STREQUAL "name")
        # check that the dependency is actually existing
        if(NOT DEFINED SHOULD_BUILD_${_arg})
          message(FATAL_ERROR "Unknown product listed as dependency for ${_product_id}: ${_arg}")
        endif(NOT DEFINED SHOULD_BUILD_${_arg})
        list(APPEND _dep_product_ids "${_arg}")
      endif(${_current_arg_type} STREQUAL "name")
    endif(${_arg} STREQUAL "NAME")
  endforeach(_arg)

  # set product vars
  set(SHOULD_BUILD_${_product_id} FALSE)
  set(CALLIGRA_PRODUCT_${_product_id}_name "${_product_name}")
  set(CALLIGRA_PRODUCT_${_product_id}_dependencies ${_dep_product_ids})
  list(APPEND CALLIGRA_ALL_PRODUCTS ${_product_id})
  message(STATUS "**** ${_product_id}: ${CALLIGRA_PRODUCT_${_product_id}_dependencies} (CALLIGRA_PRODUCT_${_product_id}_dependencies)")
endmacro(calligra_define_product)


macro(calligra_log_should_build)
  message(STATUS "---------------- The following products will be built --------------------" )
  foreach(_product_id ${CALLIGRA_ALL_PRODUCTS})
    if (SHOULD_BUILD_${_product_id} MATCHES "[Tt][Rr][Uu][Ee]")
      if (DEFINED CALLIGRA_PRODUCT_${_product_id}_dependents)
        set(dependents " [[needed by: ${CALLIGRA_PRODUCT_${_product_id}_dependents}]]")
      else (DEFINED CALLIGRA_PRODUCT_${_product_id}_dependents)
        set(dependents "")
      endif (DEFINED CALLIGRA_PRODUCT_${_product_id}_dependents)
      message(STATUS "${_product_id}:  ${CALLIGRA_PRODUCT_${_product_id}_name}${dependents}" )
    endif (SHOULD_BUILD_${_product_id} MATCHES "[Tt][Rr][Uu][Ee]")
  endforeach(_product_id)
  message(STATUS "--------------------------------------------------------------------------" )
endmacro(calligra_log_should_build)
