#!/bin/bash
cd ${0%/*}

function get_preserve() {
    namelist=(index.html)
    for name in $namelist; do
        echo "-not -name $name"
    done
}
if [[ $# -ge 1 && "$1" -eq "clean" ]]; then
    flist=$(find out -type f -name '*.html' -and $(get_preserve))
    flist="$flist $(find index -type f)"
    [[ ! -z "$flist" ]] && rm $flist
    echo 1 > out/index.count.txt
    echo > index/1.md
    exit 0
fi

################################################################################
# Configurations and helpers
################################################################################
PANDOC_ARGS="--template=template.html -s --lua-filter=filter.lua -t html"
function to_output() {
    echo out/${*%.md}.html
}
function chkfile() {
    [[ -f "$2" && `stat -c %Y $1` -le `stat -c %Y $2` ]]
    return $?
}

################################################################################
# Iterate and compile all files under src/
################################################################################
for file in $(find src -type f -name '*.md'); do
    echo "Process $file"
    basefile=${file#*src/}
    outfile=$(to_output $basefile)
    chkfile $file $outfile && continue
    ENVS="basePath=${basefile%/*}"
    env $ENVS pandoc $file -o $outfile $PANDOC_ARGS
done

################################################################################
# Generate index
################################################################################
echo "Generating index..."
indices=$(find index -type f)
for file in $indices; do
    outfile=$(to_output $file)
    chkfile $file $outfile && continue
    pandoc $file -o $outfile
done
# Write count
echo ${indices} | wc -l > out/index.count.txt

