# Search path defined from lb-dev command line
if ( ! $?User_release_area ) then
  # use a default value
  setenv User_release_area "/afs/cern.ch/user/b/bmalecki/cmtuser"
endif
if ( $?CMTPROJECTPATH ) then
  setenv CMTPROJECTPATH "${User_release_area}:${CMTPROJECTPATH}"
else
  setenv CMTPROJECTPATH "${User_release_area}"
endif
