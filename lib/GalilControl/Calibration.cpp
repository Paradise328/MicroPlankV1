#include "GalilControl.h"

// go home for four axis together
GReturn GIMBAL_FindEdge(GCon g)
{
   // galil(GCmd(g,"TL ,,,,"));
    galil(GCmd(g,"SH EFG"));
    galil(GCmd(g,"AC ,,,,1000000,1000000,1000000;"
                 "DC ,,,,1000000,1000000,1000000;"
                 "SP ,,,,100000,100000,100000"));

    std::cout << "EFG Find Edge\n";

   
    galil(GCmd(g,"CN -1"));

    galil(GCmd(g,"FE EFG;BG EFG"));
    galil(GMotionComplete(g, "EFG"));
    galil(GCmd(g,"DP ,,,,0,0,0"));
    std::cout << " 1. EFG edge found"<<std::endl;
    

    return	GALIL_EXAMPLE_OK;
}

int GIMBAL_Calibration(GCon g,char side){
    std::cout << "Initialization Starts!\n";

    int connectionTest = 0;

    galil(GCmd(g,"SH EFG"));
    galil(GMotionComplete(g, "EFG"));
    galil(GCmd(g,"ST EFG"));
    galil(GMotionComplete(g, "EFG"));

    // galil(GCmd(g,"TL ,,,,"));
    galil(GCmd(g,"SH EFG"));
    galil(GCmd(g,"AC ,,,,1000000,1000000,1000000;"
                 "DC ,,,,1000000,1000000,1000000;"
                 "SP ,,,,100000,100000,100000"));

    std::cout << "EFG Find Edge\n";

    galil(GCmd(g,"CN -1,1"));
    galil(GCmd(g,"FE EFG;BG EFG"));
    galil(GMotionComplete(g, "EFG"));
    galil(GCmd(g,"DP ,,,,0,0,0"));
    std::cout << " 1. EFG edge found"<<std::endl;

    std::cout << "Go Home\n";
    galil(GCmd(g, "PR ,,,,1000000,1000000,1000000; BG EFG")); // position relative
    galil(GMotionComplete(g, "EFG"));

    std::cout << " 2. EFG Ready\n";

    return 1;
}

GReturn INSTRUMENT_FindEdge(GCon g)
{
    galil(GCmd(g,"ST ABCD"));
    galil(GMotionComplete(g, "ABCD"));
   // galil(GCmd(g,"TL ,,,,"));
    galil(GCmd(g,"SH ABCD"));
    galil(GCmd(g,"AC 100000,100000,100000,100000"));
    galil(GCmd(g,"DC 100000,100000,100000,100000"));
    galil(GCmd(g,"SP 10000,10000,10000,10000"));

    printf("ABCD Find Edge\n");

   
    galil(GCmd(g,"CN -1"));

    galil(GCmd(g,"FE ABCD;BG ABCD"));
    galil(GMotionComplete(g, "ABCD"));
    printf(" 3. ABCD edge found\n");
    

    return	GALIL_EXAMPLE_OK;
}

int INSTRUMENT_Calibration(GCon g,char side){
    printf("Initialization Starts!\n");


    galil(GCmd(g,"SH ABCD;ST ABCD"));
    galil(GMotionComplete(g, "ABCD"));

    INSTRUMENT_FindEdge(g);

    printf(" 4. ABCD\n");

    return 1;
}

int INSTRUMENT_GetInfo(GCon g){
    int counts = 0;
    std::cout << "Get Info!\n";

    galil(GCmd(g,"serial= 1"));
    usleep(10*1000);
    galil(GCmdI(g, "counts = ?",&counts));
    printf("instrument counts = %d\n",counts);
    printf("press c to continue...");
    while (1)
    {
        if(scanKeyboard()==99|scanKeyboard()==67){
            break;
        }
    }
    
    return 1;
}



