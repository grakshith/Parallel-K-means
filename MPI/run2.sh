#! /bin/bash
k="$1"
shift
args=$#

for arg in "$@"
do
width=$(identify -format "%w" $arg)
height=$(identify -format "%h" $arg)
python2 imageConverter.py $arg
name=${arg%.*}
echo "----------------"$arg"------------------"
mpirun ./a.out $name".raw" $name"_"$k"_out.raw" $width $height $k
python2 imageConverter.py $name"_"$k"_out.raw" $width $height
echo "----------------END OF "$arg" -------------"
done

