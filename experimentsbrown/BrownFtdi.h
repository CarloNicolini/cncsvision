#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ftdi.h>

using namespace std;

namespace BrownFtdi
{
    void switch_connect();
    void switch_on(int switch_number);
    void switch_off(int switch_number);
    void switch_disconnect();
    void errorHandlingFunction(); // in case the switch gets crazy
    void switches_on(); // custom function for realObj experiments
    void switches_off(); // custom function for realObj experiments
}