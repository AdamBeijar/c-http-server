for file in ./src/*.c; do
    base_name=$(basename $file .c)

    echo "$base_name"
    echo "Compiling $file"

    gcc -c $file -o "./obj/$base_name.o"

    if [ $? -ne 0 ]; then
        echo "Compilation failed for $file"
        exit 1
    fi
done

gcc obj/*.o -o bin/server