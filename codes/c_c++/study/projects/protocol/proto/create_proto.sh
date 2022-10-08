for f in `ls *.proto`
do
    echo $f
    ../../../build/Release/protoc315 $f --cpp_out=..
done
