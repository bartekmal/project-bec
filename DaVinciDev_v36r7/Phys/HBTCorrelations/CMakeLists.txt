################################################################################
# Package: HBTCorrelations
################################################################################
gaudi_subdir(HBTCorrelations v1r0)

gaudi_depends_on_subdirs(GaudiConf
                        GaudiAlg
                        GaudiKernel
                        GaudiPython
                        GaudiSvc
                        LbcomSys
                        Event/HltEvent
			Event/LinkerEvent
                        Hlt/HltDAQ
                        Hlt/HltServices
                        Phys/DaVinci
                        Phys/DaVinciKernel
                        Kernel/LHCbMath
                        Phys/DaVinciInterfaces)
#
find_package(Boost)
find_package(pytools)

gaudi_add_module(HBTCorrelations
                 src/*.cpp
                 LINK_LIBRARIES Boost DaVinciKernelLib GaudiAlgLib HltEvent LinkerEvent LHCbMathLib DaVinciInterfacesLib)

gaudi_install_python_modules()
