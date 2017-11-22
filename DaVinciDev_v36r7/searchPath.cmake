# Search path defined from lb-dev command line
if(NOT ENV{User_release_area})
  # use a default value
  set(ENV{User_release_area} "/afs/cern.ch/user/b/bmalecki/cmtuser")
endif()
list(INSERT CMAKE_PREFIX_PATH 0 "$ENV{User_release_area}")
