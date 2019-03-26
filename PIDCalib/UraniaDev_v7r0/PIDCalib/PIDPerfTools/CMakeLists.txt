################################################################################
# Package: PIDPerfTools
################################################################################
gaudi_subdir(PIDPerfTools v9r0)

find_package(Boost COMPONENTS regex)
find_package(ROOT COMPONENTS Core RIO RooFitCore RooFit RooStats Hist Gpad Graf Tree)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

gaudi_add_dictionary(PIDPerfTools
                     dict/PIDPerfToolsDict.h
                     dict/PIDPerfToolsDict.xml
                     INCLUDE_DIRS Boost ROOT
                     LINK_LIBRARIES Boost ROOT
                     OPTIONS "-U__MINGW32__"
                     SPLIT_CLASSDEF)

gaudi_add_library(PIDPerfToolsLib
                  src/*.cpp
                  PIDPerfToolsDict_classdef.cpp
                  PUBLIC_HEADERS PIDPerfTools
                  INCLUDE_DIRS Boost ROOT
                  LINK_LIBRARIES Boost ROOT)

# ensure that genreflex is not called twice (in parallel builds)
add_dependencies(PIDPerfToolsLib PIDPerfToolsGen)
