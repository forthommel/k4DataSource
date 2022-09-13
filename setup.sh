if [ "${0}" != "${BASH_SOURCE}" ]; then
  if [ -z "${KEY4HEP_STACK}" ]; then
    source /cvmfs/fcc.cern.ch/sw/latest/setup.sh
  else
    echo "INFO: Key4hep stack already set up."
  fi
  export PYTHONPATH=$PWD/install/python:$PYTHONPATH
  export LD_LIBRARY_PATH=$PWD/install/lib:$LD_LIBRARY_PATH
  export CMAKE_PREFIX_PATH=$PWD/install:$CMAKE_PREFIX_PATH
  export ROOT_INCLUDE_PATH=$PWD/install/include:$ROOT_INCLUDE_PATH
  export LOCAL_DIR=$PWD
  export LD_LIBRARY_PATH=`python -m awkward.config --libdir`:$LD_LIBRARY_PATH
  # podio-specific paths
  export PODIO_PATH=/afs/cern.ch/user/l/lforthom/public/forFCC/podio
  export LD_LIBRARY_PATH=${PODIO_PATH}/install/lib64:${PODIO_PATH}/build/tests:${LD_LIBRARY_PATH}
  export CMAKE_PREFIX_PATH=${PODIO_PATH}/cmake:$CMAKE_PREFIX_PATH
  export ROOT_INCLUDE_PATH=${PODIO_PATH}/install/include:$ROOT_INCLUDE_PATH
else
  echo "ERROR: This script is meant to be sourced!"
fi
