#! /bin/bash

real_path=$(realpath $0)
dir_name=`dirname "${real_path}"`
echo "real_path: ${real_path}"
echo "dir_name: ${dir_name}"

data_dir="test_data"
if [[ ! -d ${dir_name}/${data_dir} ]]; then
	echo "data directory does not exist: ${data_dir}"
	ln -s ${dir_name}/./../../${data_dir} ${dir_name}
else
	echo "data directory already exists: ${data_dir}"
fi

new_dir_name=${dir_name}/build
mkdir -p ${new_dir_name}
if [ "$(ls -A ${new_dir_name})" ]; then
	echo "directory is not empty: ${new_dir_name}"
else
	echo "directory is empty: ${new_dir_name}"
fi

## Note: use configure build library, it will search system path, so lept will depend system library, for example, libz.so, libjpeg.so, libpng.so. all libraries will build make cmakelists.txt in feature.
# $ readelf -d build/Liblept_Test : depends libpng12.so.0, libjpeg.so.8
# build zlib
echo "========== start build zlib =========="
zlib_path=${dir_name}/../../src/zlib/zlib-1.2.8
if [ -e ${zlib_path}/libz.a ]; then
	echo "libz has been builded"
else
	echo "libz has not been builded yet, now start build"
	cd ${zlib_path}
	./configure
	make
	cd -
fi

cp -a ${zlib_path}/libz.a ${new_dir_name}
echo "========== finish build zlib =========="

# build libjpeg
echo "========== start build libjpeg =========="
libjpeg_path=${dir_name}/../../src/libjpeg/jpeg-9a
if [ -e ${libjpeg_path}/.libs/libjpeg.a ]; then
	echo "libjpeg has been builded"
else
	echo "ligjpeg has not been builded yet, now start build"
	cd ${libjpeg_path}
	./configure
	make
	cd -
fi

cp -a ${libjpeg_path}/.libs/libjpeg.a ${new_dir_name}
echo "========== finish build libjpeg =========="

# build libtiff
echo "========== start build libtiff =========="
libtiff_path=${dir_name}/../../src/libtiff/tiff-4.0.3
if [ -e ${libtiff_path}/libtiff/.libs/libtiff.a ]; then
	echo "libtiff has been builded"
else
	echo "libtiff has not been builded yet, now start build"
	cd ${libtiff_path}
	./configure --disable-jbig --disable-lzma
	make
	cd -
fi

cp -a ${libtiff_path}/libtiff/.libs/libtiff.a ${new_dir_name}
echo "========== finish build libtiff =========="

# build giflib
echo "========== start build giflib =========="
giflib_path=${dir_name}/../../src/giflib/giflib-5.1.1
if [ -e ${giflib_path}/lib/.libs/libgif.a ]; then
	echo "giflib has been builded"
else
	echo "giflib has not been builded yet, now start build"
	cd ${giflib_path}
	./configure
	# it has a error in linux build util module, build lib module is correct:
	# typedef int bool; error: two or more data types in declaration specifiers
	make
	cd -
fi

cp -a ${giflib_path}/lib/.libs/libgif.a ${new_dir_name}
echo "========== finish build giflib =========="

# build_libpng
echo "========== start build libpng =========="
libpng_path=${dir_name}/../../src/libpng/lpng1616
if [ -e ${libpng_path}/build/libpng.a ]; then
	echo "libpng has been builded"
else
	echo "libpng has not been builded yet, now start build"
	cd ${libpng_path}
	mkdir build
	cd build
	cmake ..
	make
	cd -
fi

cp -a ${libpng_path}/build/libpng.a ${new_dir_name}
echo "========== finish build libpng =========="

# build lept
echo "========== start build liblept =========="
liblept_path=${dir_name}/../../src/leptonica-1.71
if [ -e ${liblept_path}/src/.libs/liblept.a ]; then
	echo "liblept has been builded"
else
	echo "liblept has not been builded yet, now start build"
	cd ${liblept_path}
	./configure
	make
	cd -
fi

cp -a ${liblept_path}/src/.libs/liblept.a ${new_dir_name}
echo "========== finish build liblept =========="

rc=$?
if [[ ${rc} != 0 ]]; then
	echo "##### Error: some of thess commands have errors above, please check"
	exit ${rc}
fi

cd ${new_dir_name}
cmake ..
make

cd -

