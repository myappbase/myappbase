#!/bin/bash
set -eo pipefail
[[ -z $1 ]] && echo "Please provide the file to be hashed" && exit 1
FILE_NAME=$(basename $1 | awk '{split($0,a,/\.(d|s)/); print a[1] }')
export DETERMINED_HASH=$(sha1sum $1 | awk '{ print $1 }')
export HASHED_IMAGE_TAG="myapp-${FILE_NAME}-${DETERMINED_HASH}"
export FULL_TAG="myapp/ci:$HASHED_IMAGE_TAG"