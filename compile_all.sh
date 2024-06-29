handle_folders() {
    for item in $1/*; do
        if [ -d $item ]; then
            handle_folders $item
        elif [ -f $item ]; then
            if [[ $item == *.c ]]; then
                echo "Compiling $item..."
                gcc -g -c $item -o "./obj/$(basename $item .c).o"

                if [ $? -ne 0 ]; then
                    echo "Error occurred while compiling $item"
                    exit 1
                else 
                    echo "Successfully compiled $item"
                fi
            fi
        fi
    done
}

rm -rf obj/*.o
handle_folders src

gcc obj/*.o -o bin/server