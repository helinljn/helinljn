for f in `ls *.proto`
do
    echo $f
    ../../../.build/Release/protoc318 $f --cpp_out=..
done
