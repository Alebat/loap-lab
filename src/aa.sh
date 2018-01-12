cd ../spam
make clean
make all
if [ -f rxeflash.sh ]; then
    ./rxeflash.sh
    cd ../src
    ./run.sh
    python plots.py
else
    echo "File not found!"
fi
