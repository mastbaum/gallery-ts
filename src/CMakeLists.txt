include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${ROOT_INCLUDE_DIRS})
include_directories($ENV{NUTOOLS_INC})
include_directories($ENV{LARSIM_INC})
include_directories($ENV{FHICLCPP_INC})
include_directories($ENV{CANVAS_INC})
include_directories($ENV{CANVAS_ROOT_IO_INC})
include_directories($ENV{CETLIB_INC})
include_directories($ENV{CETLIB_EXCEPT_INC})
include_directories($ENV{GALLERY_INC})
include_directories($ENV{LARCOREOBJ_INC})
include_directories($ENV{LARDATAOBJ_INC})
include_directories($ENV{NUSIMDATA_INC})
include_directories($ENV{JSONCPP_INC})
include_directories($ENV{MESSAGEFACILITY_INC})

link_directories(
  $ENV{CANVAS_LIB}
  $ENV{CETLIB_LIB}
  $ENV{GALLERY_LIB}
  $ENV{NUSIMDATA_LIB}
  $ENV{LARCOREOBJ_LIB}
  $ENV{LARDATAOBJ_LIB}
  $ENV{LARSIM_LIB}
  $ENV{JSONCPP_LIB}
  $ENV{CLHEP_LIB_DIR}
  $ENV{MESSAGEFACILITY_LIB}
)

ROOT_GENERATE_DICTIONARY(ts_eventdict Event.hh LINKDEF linkdef.h)

find_library(LARSIM_BASE_LIBRARY larsim_EventWeight_Base HINTS $ENV{LARSIM_LIB})
find_library(LARSIM_BASE_DICT larsim_EventWeight_Base_dict HINTS $ENV{LARSIM_LIB})

add_library(ts_Event ts_eventdict.cxx)
target_link_libraries(
  ts_Event
  ${LARSIM_BASE_LIBRARY}
  ${LARSIM_BASE_DICT}
  ${ROOT_LIBRARIES}
)

add_library(ts_Processor SHARED ProcessorBase.cxx ProcessorBlock.cxx Config.cxx)
target_link_libraries(
  ts_Processor
  ts_Event
  canvas
  cetlib
  gallery
  nusimdata_SimulationBase
  larcoreobj_SummaryData
  lardataobj_RecoBase
  larsim_EventWeight_Base
  larsim_EventWeight_Base_dict
  ${LARSIM_BASE_LIBRARY}
  ${LARSIM_BASE_DICT}
  ${ROOT_LIBRARIES}
)

add_library(ts_Selection SHARED TruthSelection.cxx Selections.cxx)
target_link_libraries(
  ts_Processor
  ts_Event
  canvas
  cetlib
  gallery
  nusimdata_SimulationBase
  larcoreobj_SummaryData
  lardataobj_RecoBase
  larsim_EventWeight_Base
  larsim_EventWeight_Base_dict
  MF_MessageLogger
  MF_Utilities
  ${CLHEP_LIB}
  ${LARSIM_BASE_LIBRARY}
  ${LARSIM_BASE_DICT}
  ${ROOT_LIBRARIES}
)

file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib)
add_custom_command(TARGET ts_Event POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  libts_eventdict_rdict.pcm ${CMAKE_INSTALL_PREFIX}/lib)

add_executable(selection Main.cxx)
target_link_libraries(
  selection
  canvas
  ts_Processor
  ts_Event
  ts_Selection
  jsoncpp
  MF_MessageLogger
  MF_Utilities
  ${CLHEP_LIB}
  ${LARSIM_BASE_LIBRARY}
  ${LARSIM_BASE_DICT}
)

install(TARGETS ts_Event DESTINATION lib)
install(TARGETS ts_Processor DESTINATION lib)
install(TARGETS ts_Selection DESTINATION lib)
install(TARGETS selection DESTINATION bin)

