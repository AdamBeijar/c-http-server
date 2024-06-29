for file in ./src/*.c; do
    base_name=$(basename $file .c)

    echo "Compiling $base_name.c..."

    gcc -g -c $file -o "./obj/$base_name.o"

    if [ $? -ne 0 ]; then
        echo "Compilation failed for $file"
        exit 1
    else
        echo "$base_name.c compiled successfully"
    fi
done

gcc obj/*.o -o bin/server