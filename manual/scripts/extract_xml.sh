#!/bin/bash -e

xml_dir=${1}
tex_dir=${2}

echo ${tex_dir}

xmls=`ls "$xml_dir"/*.xml`
echo ${xmls}

for xml in $xmls; do
	echo ${xml} "--" ${tex_dir}/${xml}.tex
	name=$(basename ${xml})
	rm ${tex_dir}/${name}.tex
	votca_property --file ${xml} --format TEX --level 2 > ${tex_dir}/${name}.tex
done
