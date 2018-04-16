#!/bin/bash -e

VOTCASHARE="$(csg_call --show-share)"
texfile=$1

programs="ctp_map ctp_dump ctp_tools ctp_run ctp_parallel moo_overlap kmc_run"
rm -f $texfile; touch $texfile

for program in $programs; do
	$program --tex >> $texfile 
done
