echo "serial image compression using k-means clustering"
echo "compiling..."
g++ openmp_compression.cpp -fopenmp -lboost_system -lboost_serialization `pkg-config --cflags --libs opencv` -o compress.out

echo "enter the name of the file"

read filename
# filename="parrot.jpg"

./compress.out $filename

echo "compression done!"

name=${filename%.*}

initial_image=$name"_initial.bin"
initial_size=$(stat -c%s "$initial_image")
echo "Size of $initial_image = $initial_size bytes."

compressed_image=$name"_compressed.bin"
compressed_size=$(stat -c%s "$compressed_image")
echo "Size of $compressed_image = $compressed_size bytes."

echo -n "compressed by a factor of -> " 
echo "scale=2;$initial_size / ( $compressed_size )" | bc -l

read -p "Uncompress? y/n "
    if [[ ${REPLY,,} =~ ^y ]]; then
        
        echo "compiling..."
        g++ serial_uncompression.cpp -lboost_system -lboost_serialization `pkg-config --cflags --libs opencv` -o uncompress.out

        echo "running..."
        ./uncompress.out $compressed_image

        uncompressed_image=$name"_compressed.jpg"

        echo "uncompression done! File available at $uncompressed_image "
    fi




echo $name2


