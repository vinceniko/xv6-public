FROM ubuntu:14.04
RUN apt-get -qq update
RUN apt-get install -y git nasm build-essential qemu gdb
CMD ["bash"]

