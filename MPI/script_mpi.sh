echo "parallel image compression using MPI"
echo "compiling..."
mpic++ kmeans_new_mod.cpp
echo "enter the name of the file"
read filename
# echo "enter dimensions"
# read width
# read height
width=$(identify -format "%w" $filename)
height=$(identify -format "%h" $filename)
python2 imageConverter.py $filename
name=${filename%.*}
mpirun ./a.out $name".raw" $name"out.raw" $width $height 16
python2 imageConverter.py $name"out.raw" $width $height
