dname='temp_build'

#/bin/ash
if [ ! -d "temp_build" ]; then 
    mkdir $dname
fi
cp -r pympofl $dname
cd $dname
if [ -d "pympofl" ]; then 
    cd pympofl
    rm -r __pycache__
    cd ..
    sudo cp -rv pympofl $(python3 -c 'import site; print(site.getsitepackages()[0])')
else
    echo "Something went wrong: Failed to copy pympofl"
fi
cd ..
rm -r $dname