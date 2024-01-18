#ifndef GALILSLAVE_H
#define GALILSLAVE_H

#include <iostream>

#include <gclibo.h>
#include <gclib.h>

#include "SlaveDevice.h"
#include "easylogging++.h"

//return code for correct code execution
#define GALIL_EXAMPLE_OK 0              
#define GALIL_EXAMPLE_ERROR -100 

// deal with the communication handle with galil
inline void galil(GReturn rc)
{
    if (rc != G_NO_ERROR)
        throw rc;
}

//! An example of error handling and debugging information.
inline void error(GCon g, GReturn rc)
{
    char buf[G_SMALL_BUFFER];
    GError(rc, buf, G_SMALL_BUFFER); //Get Error Information
    std::cout <<buf<<"\n";

    if (g)
    {
        GSize size = sizeof(buf);
        GUtility(g, G_UTIL_ERROR_CONTEXT, buf, &size);

        if (buf[0]){
            printf("%s",buf);
            printf("\n"); //further context
        }

        if ((rc == G_BAD_RESPONSE_QUESTION_MARK)
            && (GCommand(g, "TC1", buf, G_SMALL_BUFFER, 0) == G_NO_ERROR))
        {
            printf("%s",buf);
            printf("\n"); //Error code from controller
        }
    }
}

class GalilSlave : public SlaveDevice{

public:
    GalilSlave();

    // 
    int open(const char* address);

    int setServo(char* );
    int motorOff(char* );


private:
    // handle for communicate with galil controller
    GCon g = 0;
    GReturn rc = GALIL_EXAMPLE_OK;
    char buf[G_SMALL_BUFFER];

};

#endif 
