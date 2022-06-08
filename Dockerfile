# Get the base Ubuntu image from Docker Hub
FROM ubuntu:xenial

# Update apps on the base image
RUN apt-get -y update && apt-get install -y

# Install the Clang compiler
RUN apt-get -y install build-essential libboost-all-dev freeglut3-dev libglew-dev  libglu1-mesa-dev libgl1-mesa-glx libgl1-mesa-dev mesa-common-dev libxi-dev libxmu-dev libxmuu-dev qt4-dev-tools cmake x11-apps

# Copy the current folder which contains C++ source code to the Docker image under /usr/src
COPY . .

# Specify the working directory
WORKDIR .

ENV QT_DEBUG_PLUGINS=1
ENV DISPLAY :0
RUN cd build; cmake -DQT_SUPPORT=True ..; make -j 8 QtStaircase;
CMD ["./build/QtStaircase"]
