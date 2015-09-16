// Check if this bug happens:
// https://bugs.launchpad.net/ubuntu/+source/nvidia-graphics-drivers-319/+bug/1248642
// The solution is to put
// export LD_PRELOAD=/usr/lib/x86_64-linux-gnu/mesa/libGL.so.1
// in .bashrc

#include <string>
#include <GL/gl.h>

int main(int argc, char *argv[])
{
    //std::string s;
    glEnable(GL_LIGHT0);
    return 0;
}
