#! /bin/bash

real_path=$(realpath $0)
dir_name=`dirname "${real_path}"`
echo "real_path: ${real_path}, dir_name: ${dir_name}"

data_dir="test_data"
if [ -d ${dir_name}/${data_dir} ]; then
	rm -rf ${dir_name}/${data_dir}
fi

ln -s ${dir_name}/./../../${data_dir} ${dir_name}

new_dir_name=${dir_name}/build
mkdir -p ${new_dir_name}
cd ${new_dir_name}
echo "pos: ${new_dir_name}"
if [ "$(ls -A ${new_dir_name})" ]; then
	echo "directory is not empty: ${new_dir_name}"
	rm -r *
else
	echo "directory is empty: ${new_dir_name}"
fi

# build zlib
zlib_path=${dir_name}/../../src/zlib/zlib-1.2.8
cd ${zlib_path}
./configure
make
ln -s ${zlib_path}/libz.a ${new_dir_name}

# build libjpeg
libjpeg_path=${dir_name}/../../src/libjpeg/jpeg-9a
cd ${libjpeg_path}
./configure
make
ln -s ${libjpeg_path}/.libs/libjpeg.a ${new_dir_name}

# build libtiff
libtiff_path=${dir_name}/../../src/libtiff/tiff-4.0.3
cd ${libtiff_path}
./configure
make
ln -s ${libtiff_path}/libtiff/.libs/libtiff.a ${new_dir_name}

# build giflib
giflib_path=${dir_name}/../../src/giflib/giflib-5.1.1
cd ${giflib_path}
./configure
# it has a error in linux build util module, build lib module is correct:
# typedef int bool; error: two or more data types in declaration specifiers
make
ln -s ${giflib_path}/lib/.libs/libgif.a ${new_dir_name}

# build lept
liblept_path=${dir_name}/../../src/leptonica-1.71
cd ${liblept_path}
./configure
make
ln -s ${liblept_path}/src/.libs/liblept.a ${new_dir_name}

cd ${new_dir_name}
#cmake ..
#make

#cd -

