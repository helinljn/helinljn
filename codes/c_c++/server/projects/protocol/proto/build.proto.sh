for f in `ls *.proto`
do
    echo $f
    ../../../.build/Release/protoc-exec $f --cpp_out=..
done
