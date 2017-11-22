# Search path defined from lb-dev command line
if [ -z "$User_release_area" ] ; then
  # use a default value
  export User_release_area="/afs/cern.ch/user/b/bmalecki/cmtuser"
fi
export CMTPROJECTPATH="${User_release_area}${CMTPROJECTPATH:+:${CMTPROJECTPATH}}"
