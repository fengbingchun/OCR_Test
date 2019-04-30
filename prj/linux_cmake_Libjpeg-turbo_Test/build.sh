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

# build libjpeg-turbo
libjpeg_turbo_path=${dir_name}/../../src/libjpeg-turbo
cd ${libjpeg_turbo_path}
mkdir build
cd build
cmake ..
make
ln -s ${libjpeg_turbo_path}/build/libturbojpeg.a ${new_dir_name}

cd -

cd ${new_dir_name}
cmake ..
make

cd -

