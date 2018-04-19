#!/bin/bash -e

die() {
  echo "$*" >&2
  exit 1
}
[[ -z ${VOTCASHARE} ]] && die "${0##*/}: VOTCASHARE not set"
export VOTCASHARE
texfile=$1
shift

programs="ctp_map ctp_dump ctp_tools ctp_run ctp_parallel moo_overlap"
rm -f $texfile; touch $texfile

for program in $programs; do
	exe=$(echo "$@" | xargs -n1 echo | grep "$program")
	echo ${exe}
	$exe --tex >> $texfile 
done
