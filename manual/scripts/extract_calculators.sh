#!/bin/bash -e

die() {
  echo "$*" >&2
  exit 1
}
[[ -z ${VOTCASHARE} ]] && die "${0##*/}: VOTCASHARE not set"
[[ -z ${VOTCA_PROPERTY} ]] && die "${0##*/}: VOTCA_PROPERTY not set"
export VOTCASHARE

texfile=$1
rm -f $texfile; touch $texfile
shift

for package in ctp_tools ctp_run ctp_parallel ctp_dump; do
	exe=$(echo "$@" | xargs -n1 echo | grep "$package")
	calculators="$(${exe} --list | sed -ne 's/^\s\+\([a-z,0-9]*\)\s*\(.*\)/\1/p')"
	echo $calculators

	# loop over all calculators
	for calculator in ${calculators}; do
		xmlfile=${VOTCASHARE}/xml/$calculator.xml
		echo $calculator
		echo $xmlfile
		if [ ! -f "$xmlfile" ]; then 
			continue
		fi
		echo "${VOTCA_PROPERTY} --file $xmlfile --format TEX --level 2"
		${VOTCA_PROPERTY} --file $xmlfile --format TEX --level 2 >> $texfile

	done
done
