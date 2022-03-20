# https://github.com/Xilinx/Xilinx_Base_Runtime
# https://docs.docker.com/engine/reference/commandline/exec/
# xilinx/xilinx_runtime_base:alveo-2021.2-ubuntu-20.04

# docker run --name xilTest --rm -t -i xilinx/xilinx_runtime_base:alveo-2021.2-ubuntu-20.04 bash

# docker exec --rm -it $DEVICE xilinx/xilinx_runtime_base:alveo-2021.2-ubuntu-20.04 bash

## Build docker container
docker build -t u55c .

## Run container
DEVICE="--device=/dev/xclmgmt256:/dev/xclmgmt256 --device=/dev/dri/renderD128:/dev/dri/renderD128"
docker run --name xillTest --rm -t -i $DEVICE u55c bash

## Inside container load xrt
source /opt/xilinx/xrt_versions/xrt_202120.2.12.427/setup.sh

