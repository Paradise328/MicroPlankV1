#include "GalilControl.h"
#include <iostream>


int scanKeyboard()
{
    int input;
    struct termios new_settings;
    struct termios stored_settings;
    tcgetattr(0,&stored_settings);
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
    tcgetattr(0,&stored_settings);
    new_settings.c_cc[VMIN] = 0;
    tcsetattr(0,TCSANOW,&new_settings);

    input = getchar();

    tcsetattr(0,TCSANOW,&stored_settings);
    return input;
}

double * tellAN(GCon g){
    static double AN[8];
    char ANTmp[G_SMALL_BUFFER];
    GSize read_bytes = 4*8; //bytes read in GCommand float=4bytes

    galil(GCommand(g,"MG @AN[1],@AN[2],@AN[3],@AN[4],@AN[5],@AN[6],@AN[7],@AN[8]",ANTmp,sizeof (ANTmp),&read_bytes));

    std::vector<std::string> result;
    std::stringstream s_stream(ANTmp); //create string stream from the string

    while(s_stream.good()) {
      std::string substr;
      getline(s_stream, substr, ' '); //get first string delimited by comma
      result.push_back(substr);
    }
    for(int i = 0; i<8; i++) {    //print all splitted strings
      AN[i] = stod(result.at(i+1));
    }
    return AN;
}

double * tellIN(GCon g){
    static double IN[8];
    char INTmp[G_SMALL_BUFFER];
    GSize read_bytes = 4*8; //bytes read in GCommand float=4bytes

    galil(GCommand(g,"MG @IN[1],@IN[2],@IN[3],@IN[4],@IN[5],@IN[6],@IN[7],@IN[8]",INTmp,sizeof (INTmp),&read_bytes));

    std::vector<std::string> result;
    std::stringstream s_stream(INTmp); //create string stream from the string
    while(s_stream.good()) {
      std::string substr;
      getline(s_stream, substr, ' '); //get first string delimited by comma
      result.push_back(substr);
    }
    for(int i = 0; i<8; i++) {    //print all splitted strings
      IN[i] = stod(result.at(i+1));
    }
    return IN;
}

double * tellIN2(GCon g){
    static double IN[8];
    char INTmp[G_SMALL_BUFFER];
    GSize read_bytes = 4*8; //bytes read in GCommand float=4bytes

    galil(GCommand(g,"MG @IN[9],@IN[10],@IN[11],@IN[12],@IN[13],@IN[14],@IN[15],@IN[16]",INTmp,sizeof (INTmp),&read_bytes));

    std::vector<std::string> result;
    std::stringstream s_stream(INTmp); //create string stream from the string
    while(s_stream.good()) {
      std::string substr;
      getline(s_stream, substr, ' '); //get first string delimited by comma
      result.push_back(substr);
    }
    for(int i = 0; i<8; i++) {    //print all splitted strings
      IN[i] = stod(result.at(i+1));
    }
    return IN;
}

double * tellPos(GCon g){
    static double Pos[8];
    char INTmp[G_SMALL_BUFFER];
    GSize read_bytes = 4*8; //bytes read in GCommand float=4bytes

    galil(GCommand(g,"TP",INTmp,sizeof (INTmp),&read_bytes));

    std::vector<std::string> result;
    std::stringstream s_stream(INTmp); //create string stream from the string
    while(s_stream.good()) {
      std::string substr;
      getline(s_stream, substr, ' '); //get first string delimited by comma
      result.push_back(substr);
    }
    for(int i = 0; i<8; i++) {    //print all splitted strings
      Pos[i] = stod(result.at(i+1));
    }
    return Pos;
}

GReturn MotorOff(GCon g){
    galil(GCmd(g,"ST ABCDEFG"));
    galil(GMotionComplete(g,"ABCDEFG"));
    galil(GCmd(g,"MO ABCDEFG"));
    return	GALIL_EXAMPLE_OK;
}

GReturn MotorOff(GCon g, char axis){
    char buf[G_SMALL_BUFFER];
    sprintf(buf, "ST%c", axis);
    galil(GCmd(g, buf));

    sprintf(buf, "%c", axis);
    galil(GMotionComplete(g, buf));

    sprintf(buf, "MO%c", axis);
    galil(GCmd(g, buf));

    return	GALIL_EXAMPLE_OK;
}


GReturn GoPosition(GCon g, char axis, int position,int speed){

    char buf[G_SMALL_BUFFER];

    sprintf(buf, "DP%c=0;PR%c=%d;BG%c",axis,axis,position,axis);
    galil(GCmd(g, buf)); // position relative

    return	GALIL_EXAMPLE_OK;
}

GReturn GoPosition(GCon g, int enc1,int enc2,int enc3){

    char buf[G_SMALL_BUFFER];
    std::cout << "GP"<<std::endl;
 //   galil(GMotionComplete(g, "ABC"));
    sprintf(buf, "PA %d,%d,%d;BG",enc1,enc2,enc3);
    galil(GCmd(g, buf)); // position absolute
    galil(GMotionComplete(g, "ABC"));

    return	GALIL_EXAMPLE_OK;
}

GReturn Jog(GCon g, char axis, int speed=0){
    char buf[G_SMALL_BUFFER];

    sprintf(buf, "JG%c=%d",axis, speed);
    galil(GCmd(g, buf)); // Set speed

    return	GALIL_EXAMPLE_OK;
}

GReturn Jog(GCon g, char axis1, int speed1, char axis2, int speed2, char axis3, int speed3){

    char buf1[G_SMALL_BUFFER];
    char buf2[G_SMALL_BUFFER];
    char buf3[G_SMALL_BUFFER];

    sprintf(buf1, "JG%c=%d",axis1, speed1);
    sprintf(buf2, "JG%c=%d",axis2, speed2);
    sprintf(buf3, "JG%c=%d",axis3, speed3);

    galil(GCmd(g, buf1)); // Set speed
    galil(GCmd(g, buf2)); // Set speed
    galil(GCmd(g, buf3)); // Set speed

    return	GALIL_EXAMPLE_OK;
}

GReturn Jog(GCon g, char axis1, int speed1, char axis2, int speed2, char axis3, int speed3, char axis4, int speed4){
    char buf[G_SMALL_BUFFER];
    char buf1[G_SMALL_BUFFER];
    char buf2[G_SMALL_BUFFER];
    char buf3[G_SMALL_BUFFER];
    char buf4[G_SMALL_BUFFER];

    sprintf(buf1, "JG%c=%d",axis1, speed1);
    sprintf(buf2, "JG%c=%d",axis2, speed2);
    sprintf(buf3, "JG%c=%d",axis3, speed3);
    sprintf(buf4, "JG%c=%d",axis4, speed4);

    galil(GCmd(g, buf1)); // Set speed
    galil(GCmd(g, buf2)); // Set speed
    galil(GCmd(g, buf3)); // Set speed
    galil(GCmd(g, buf4)); // Set speed

    return	GALIL_EXAMPLE_OK;
}

GReturn InitMotor(GCon g){
    galil(GCmd(g,"MO;AG 0,0,0,0"));
    galil(GCmd(g,"AU 0.5,0.5,0.5,0.5"));
    galil(GCmd(g,"TL 3,3,3,3"));
    galil(GCmd(g,"TK 8,8,8,8"));
    galil(GCmd(g,"BR 1,1,1,1;SH"));

    return	GALIL_EXAMPLE_OK;
}

GReturn SetDefaultSpeed(GCon g){
    galil(GCmd(g,"SP 20000,200000,200000,2000,2000,200000,200000,200000"));

    return	GALIL_EXAMPLE_OK;
}

GReturn PositionRelative(GCon g, char axis, int position){
    char buf[G_SMALL_BUFFER];

    sprintf(buf, "PR%c=%d",axis,position);
    galil(GCmd(g, buf)); // position relative

    sprintf(buf, "BG%c", axis);
    galil(GCmd(g, buf)); // begin

    return	GALIL_EXAMPLE_OK;
}


GReturn Galil_control_l(GCon g,int target_position_l[])
{
    char buf[G_SMALL_BUFFER]={0};
    char buf1[G_SMALL_BUFFER]={0};
    for(int i=0;i<3;i++){
        if(abs(target_position_l[i+12])>900000){
            target_position_l[i+12]=target_position_l[i+12]/abs(target_position_l[i+12])*900000;
            //target_position_l[i+4]=0;
        }
    }

//    for(int i=0;i<7;i++){
//        if(abs(target_position_l[i])>500000){
//            target_position_l[i]=target_position_l[i]/abs(target_position_l[i])*500000;
//        }
//    }
//    if(target_position_l[16]==0)
//    {
//        sprintf(buf,"PA %d,%d,%d,%d,%d,%d,%d;SP %d,%d,%d,%d,%d,%d,%d;"
//               ,target_position_l[8],target_position_l[9],target_position_l[10],target_position_l[11],target_position_l[12],target_position_l[13]
//               ,target_position_l[14],target_position_l[0],target_position_l[1],target_position_l[2],target_position_l[3],target_position_l[4]
//               ,target_position_l[5],target_position_l[6]);
//        galil(GCmd(g, buf));
//    }
//    else if(target_position_l[16]==1)
//    {
//        sprintf(buf,"PA %d,%d,,%d,%d,%d,%d;SP %d,%d,,%d,%d,%d,%d;PTC=0;KDC=0;KPC=0;KIC=0;OFC=0.05;SHC;"
//               ,target_position_l[8],target_position_l[9],target_position_l[11],target_position_l[12],target_position_l[13]
//               ,target_position_l[14],target_position_l[0],target_position_l[1],target_position_l[3],target_position_l[4]
//               ,target_position_l[5],target_position_l[6]);
//        galil(GCmd(g, buf));
//        galil(GMotionComplete(g, "C"));
//    }
//    else if(target_position_l[16]==3)
//    {
//        sprintf(buf,"PA %d,%d,,%d,%d,%d,%d;SP %d,%d,,%d,%d,%d,%d;SHC"
//               ,target_position_l[8],target_position_l[9],target_position_l[11],target_position_l[12],target_position_l[13]
//               ,target_position_l[14],target_position_l[0],target_position_l[1],target_position_l[3],target_position_l[4]
//               ,target_position_l[5],target_position_l[6]);
//        galil(GCmd(g, buf));
//    }
//    else
//    {
//        sprintf(buf,"PA %d,%d,,%d,%d,%d,%d;SP %d,%d,,%d,%d,%d,%d;OFC=0;STC;"
//               ,target_position_l[8],target_position_l[9],target_position_l[11],target_position_l[12],target_position_l[13]
//               ,target_position_l[14],target_position_l[0],target_position_l[1],target_position_l[3],target_position_l[4]
//               ,target_position_l[5],target_position_l[6]);
//        galil(GCmd(g, buf));
//        galil(GMotionComplete(g, "C"));
//        sprintf(buf1,"DPC=0;KDC=64;KPC=8;KIC=0;ACC=500000;DCC=500000;PTC=1;DPC=%d;",target_position_l[10]);
//        galil(GCmd(g, buf1));
//    }

        sprintf(buf,"PA %d,%d,%d,%d,%d,%d,%d;SP %d,%d,%d,%d,%d,%d,%d;"
               ,target_position_l[8],  target_position_l[9],
                target_position_l[10], target_position_l[11],
                target_position_l[12], target_position_l[13],
                target_position_l[14],
                target_position_l[0], target_position_l[1],
                target_position_l[2], target_position_l[3],
                target_position_l[4], target_position_l[5],
                target_position_l[6]);
         galil(GCmd(g, buf));

    // position relative
//        sprintf(buf,"PA %d,%d,%d,%d,%d,%d,%d;SP %d,%d,%d,%d,%d,%d,%d;"
//               ,target_position_l[8],target_position_l[9],target_position_l[10],target_position_l[11],target_position_l[12],target_position_l[13]
//               ,target_position_l[14],target_position_l[0],target_position_l[1],target_position_l[2],target_position_l[3],target_position_l[4],target_position_l[5],target_position_l[6]);
//         galil(GCmd(g, buf)); // position relative
//   memset(buf,0,G_SMALL_BUFFER);
}

GReturn Galil_control2_l(GCon g,int target_position_l[])
{
    char buf[G_SMALL_BUFFER]={0};
    char buf1[G_SMALL_BUFFER]={0};
    for(int i=0;i<2;i++){
        if(abs(target_position_l[i+12])>957330){
            target_position_l[i+4]=0;
        }
    }

        sprintf(buf,"PA %d,%d,%d,%d;SP %d,%d,%d,%d; JG ,,,,%d,%d,%d; BG EFG;"
               ,target_position_l[8],  target_position_l[9],
                target_position_l[10], target_position_l[11],
                target_position_l[0], target_position_l[1],
                target_position_l[2], target_position_l[3],
                target_position_l[4], target_position_l[5],
                target_position_l[6]);
         galil(GCmd(g, buf));

}


GReturn Galil_control_r(GCon g,int target_position_r[])
{
    char buf[G_SMALL_BUFFER]={0};
    char buf1[G_SMALL_BUFFER]={0};
    for(int i=0;i<3;i++){
        if(abs(target_position_r[i+12])>900000){
            target_position_r[i+12]=target_position_r[i+12]/abs(target_position_r[i+12])*900000;
            //target_position_l[i+4]=0;
        }
    }

//    if(target_position_r[16]==0)
//    {
//        sprintf(buf,"PA %d,%d,%d,%d,%d,%d,%d;SP %d,%d,%d,%d,%d,%d,%d;"
//               ,target_position_r[8],target_position_r[9],target_position_r[10],target_position_r[11],target_position_r[12],target_position_r[13]
//               ,target_position_r[14],target_position_r[0],target_position_r[1],target_position_r[2],target_position_r[3],target_position_r[4]
//               ,target_position_r[5],target_position_r[6]);
//        galil(GCmd(g, buf));
//    }
//    else if(target_position_r[16]==1)
//    {
//        sprintf(buf,"PA %d,%d,,%d,%d,%d,%d;SP %d,%d,,%d,%d,%d,%d;PTC=0;KDC=0;KPC=0;KIC=0;OFC=-0.2;SHC;"
//               ,target_position_r[8],target_position_r[9],target_position_r[11],target_position_r[12],target_position_r[13]
//               ,target_position_r[14],target_position_r[0],target_position_r[1],target_position_r[3],target_position_r[4]
//               ,target_position_r[5],target_position_r[6]);
//        galil(GCmd(g, buf));
//        galil(GMotionComplete(g, "C"));
//    }
//    else if(target_position_r[16]==3)
//    {
//        sprintf(buf,"PA %d,%d,,%d,%d,%d,%d;SP %d,%d,,%d,%d,%d,%d;SHC"
//               ,target_position_r[8],target_position_r[9],target_position_r[11],target_position_r[12],target_position_r[13]
//               ,target_position_r[14],target_position_r[0],target_position_r[1],target_position_r[3],target_position_r[4]
//               ,target_position_r[5],target_position_r[6]);
//        galil(GCmd(g, buf));
//    }
//    else
//    {
//        sprintf(buf,"PA %d,%d,,%d,%d,%d,%d;SP %d,%d,,%d,%d,%d,%d;OFC=0;STC;"
//               ,target_position_r[8],target_position_r[9],target_position_r[11],target_position_r[12],target_position_r[13]
//               ,target_position_r[14],target_position_r[0],target_position_r[1],target_position_r[3],target_position_r[4]
//               ,target_position_r[5],target_position_r[6]);
//        galil(GCmd(g, buf));
//        galil(GMotionComplete(g, "C"));
//        sprintf(buf1,"DPC=0;KDC=64;KPC=8;KIC=0;ACC=500000;DCC=500000;PTC=1;DPC=%d;",target_position_r[10]);
//        galil(GCmd(g, buf1));
//    }

    //POSITION SOFT LIMITS AND SPEED LIMITS
    for(int i=0;i<3;i++){
        if(abs(target_position_r[i+12])>450000){
            target_position_r[i+12]=target_position_r[i+12]/abs(target_position_r[i+12])*900000;
            //target_position_l[i+4]=0;
        }
    }

    for(int i=0;i<7;i++){
        if(abs(target_position_r[i])>400000){
            target_position_r[i]=target_position_r[i]/abs(target_position_r[i])*1000000;
        }
    }

    sprintf(buf,"PA %d,%d,%d,%d,%d,%d,%d;SP %d,%d,%d,%d,%d,%d,%d;"
           ,target_position_r[8],target_position_r[9],target_position_r[10],target_position_r[11],target_position_r[12],target_position_r[13]
           ,target_position_r[14],target_position_r[0],target_position_r[1],target_position_r[2],target_position_r[3],target_position_r[4]
           ,target_position_r[5],target_position_r[6]);
    galil(GCmd(g, buf)); // position relative
}

GReturn Galil_control2_r(GCon g,int target_position_r[])
{
    char buf[G_SMALL_BUFFER]={0};
    char buf1[G_SMALL_BUFFER]={0};
    for(int i=0;i<3;i++){
        if(abs(target_position_r[i+12])>957330){
            target_position_r[i+4]=0;
        }
    }

//    if(target_position_r[16]==0)
//    {
//        sprintf(buf,"PA %d,%d,%d,%d,%d,%d,%d;SP %d,%d,%d,%d,%d,%d,%d;"
//               ,target_position_r[8],target_position_r[9],target_position_r[10],target_position_r[11],target_position_r[12],target_position_r[13]
//               ,target_position_r[14],target_position_r[0],target_position_r[1],target_position_r[2],target_position_r[3],target_position_r[4]
//               ,target_position_r[5],target_position_r[6]);
//        galil(GCmd(g, buf));
//    }
//    else if(target_position_r[16]==1)
//    {
//        sprintf(buf,"PA %d,%d,,%d,%d,%d,%d;SP %d,%d,,%d,%d,%d,%d;PTC=0;KDC=0;KPC=0;KIC=0;OFC=-0.2;SHC;"
//               ,target_position_r[8],target_position_r[9],target_position_r[11],target_position_r[12],target_position_r[13]
//               ,target_position_r[14],target_position_r[0],target_position_r[1],target_position_r[3],target_position_r[4]
//               ,target_position_r[5],target_position_r[6]);
//        galil(GCmd(g, buf));
//        galil(GMotionComplete(g, "C"));
//    }
//    else if(target_position_r[16]==3)
//    {
//        sprintf(buf,"PA %d,%d,,%d,%d,%d,%d;SP %d,%d,,%d,%d,%d,%d;SHC"
//               ,target_position_r[8],target_position_r[9],target_position_r[11],target_position_r[12],target_position_r[13]
//               ,target_position_r[14],target_position_r[0],target_position_r[1],target_position_r[3],target_position_r[4]
//               ,target_position_r[5],target_position_r[6]);
//        galil(GCmd(g, buf));
//    }
//    else
//    {
//        sprintf(buf,"PA %d,%d,,%d,%d,%d,%d;SP %d,%d,,%d,%d,%d,%d;OFC=0;STC;"
//               ,target_position_r[8],target_position_r[9],target_position_r[11],target_position_r[12],target_position_r[13]
//               ,target_position_r[14],target_position_r[0],target_position_r[1],target_position_r[3],target_position_r[4]
//               ,target_position_r[5],target_position_r[6]);
//        galil(GCmd(g, buf));
//        galil(GMotionComplete(g, "C"));
//        sprintf(buf1,"DPC=0;KDC=64;KPC=8;KIC=0;ACC=500000;DCC=500000;PTC=1;DPC=%d;",target_position_r[10]);
//        galil(GCmd(g, buf1));
//    }

    //POSITION SOFT LIMITS AND SPEED LIMITS

    for(int i=0;i<4;i++){
        if(abs(target_position_r[i])>400000){
            target_position_r[i]=target_position_r[i]/abs(target_position_r[i])*1000000;
        }
    }

    sprintf(buf,"PA %d,%d,%d,%d;SP %d,%d,%d,%d;JG ,,,,%d,%d,%d; BG EFG;"
           ,target_position_r[8],target_position_r[9],target_position_r[10],target_position_r[11],
            target_position_r[0],target_position_r[1],target_position_r[2],target_position_r[3],
            target_position_r[4],target_position_r[5],target_position_r[6]);
    galil(GCmd(g, buf)); // position relative
}


void exitPTmode(GCon g){
    galil(GCmd(g,"PT 0,0,0,0,0,0,0,0;"));
    galil(GMotionComplete(g,"ABCDEFG"));

}






void initGali(GCon g){
    //                 "AC 1000000000,1000000000,1000000000,1000000000,1000000000,1000000000,1000000000,1000000000;"
  //  "DC 1000000000,1000000000,1000000000,1000000000,1000000000,1000000000,1000000000,1000000000;"
    galil(GCmd(g,"ST ABCDEFG;"));
    galil(GMotionComplete(g,"ABCDEFG"));
    galil(GCmd(g,"SH ABCDEFG"));
  //  galil(GMotionComplete(g,"ABCDEFG"));
    galil(GCmd(g,"TL 1,1,0.5,0.5,5.0,5.0,5.0;"
                 "TK 1,1,0.5,0.5,9.9,9.9,2;"
                 "ER -1,-1,-1,-1,-1,-1,-1;"
                 "OE 1,1,1,1,1,1,1,;"
                 "KPA=8;KDA=64; KPB=8;KDB=64; KPC=8;KDC=64; KPD=8;KDD=64;"
                 "KPE=12;KIE=0.2;KDE=120; "
                 "KPF=12;KIF=0.2;KDF=120; "
                 "KPG=12;KIG=0.2;KDG=120; "
                 "NBE=0.5;NZE=0.5;NFE=0;"
                 "AC 1000000,1000000,1000000,1000000,2000000,2000000,1500000,;"
                 "DC 1000000,1000000,1000000,1000000,2000000,2000000,1500000,;"
                 "SP 0,0,0,0,0,0,0,;"
                 "DP 0,0,0,0,0,0,0,;"
                 "PT 1,1,1,1,1,1,1,;"));
}


void initGali_l(GCon g){
    //                 "AC 1000000000,1000000000,1000000000,1000000000,1000000000,1000000000,1000000000,1000000000;"
  //  "DC 1000000000,1000000000,1000000000,1000000000,1000000000,1000000000,1000000000,1000000000;"
    galil(GCmd(g,"ST ABCDEFG;"));
    galil(GMotionComplete(g,"ABCDEFG"));
    galil(GCmd(g,"SH ABCDEFG"));
  //  galil(GMotionComplete(g,"ABCDEFG"));
    galil(GCmd(g,"TL 1,1,0.8,0.8,2.5,2.5,2.5;"
                 "TK 1,1,1.5,1.5,4.9,4.9,4.9;"
                 "ER -1,-1,-1,-1,-1,-1,-1;"
                 "OE 1,1,1,1,1,1,1,;"
                 "KPA=8;KDA=64; KPB=8;KDB=64; KPC=8;KDC=64; KPD=8;KDD=64;"
//                 "KPE=12;KIE=1.2;KDE=120; "
//                 "KPF=12;KIF=1.2;KDF=120; "
//                 "KPG=12;KIG=1.2;KDG=120; "
                 "KPE=20;KIE=0;KDE=64; "
                 "KPF=20;KIF=0;KDF=64; "
                 "KPG=20;KIG=0;KDG=64; "
                 "NBE=0.5;NZE=0.5;NFE=0;"
                 "AC 1000000,1000000,1000000,1000000,1500000,1500000,1500000,;"
                 "DC 1000000,1000000,1000000,1000000,1500000,1500000,1500000,;"
                 "SP 0,0,0,0,0,0,0,;"
                 "DP 0,0,0,0,0,0,0,;"
                 "PT 1,1,1,1,0,0,0,;"
                 "CN -1;"));
    galil(GCmd(g,"JG ,,,,0,0,0,0;BGE;BGF;BGG"));
}

void initGali_r(GCon g){
    //                 "AC 1000000000,1000000000,1000000000,1000000000,1000000000,1000000000,1000000000,1000000000;"
  //  "DC 1000000000,1000000000,1000000000,1000000000,1000000000,1000000000,1000000000,1000000000;"
    galil(GCmd(g,"ST ABCDEFG;"));
    galil(GMotionComplete(g,"ABCDEFG"));
    galil(GCmd(g,"SH ABCDEFG"));
  //  galil(GMotionComplete(g,"ABCDEFG"));
    galil(GCmd(g,"TL 1,1,0.8,0.8,2.5,2.5,2.5;"
                 "TK 1,1,1.5,1.5,4.9,4.9,4.9;"
                 "ER -1,-1,-1,-1,-1,-1,-1;"
                 "OE 1,1,1,1,1,1,1,;"
                 "KPA=8;KDA=64; KPB=8;KDB=64; KPC=8;KDC=64; KPD=8;KDD=64;"
//                 "KPE=12;KIE=1.2;KDE=120; "
//                 "KPF=12;KIF=1.2;KDF=120; "
//                 "KPG=12;KIG=1.2;KDG=120; "
                 "KPE=20;KIE=0;KDE=64; "
                 "KPF=20;KIF=0;KDF=64; "
                 "KPG=20;KIG=0;KDG=64; "
                 "NBE=0.5;NZE=0.5;NFE=0;"
                 "AC 1000000,1000000,1000000,1000000,1500000,1500000,1500000,;"
                 "DC 1000000,1000000,1000000,1000000,1500000,1500000,1500000,;"
                 "SP 0,0,0,0,0,0,0,;"
                 "DP 0,0,0,0,0,0,0,;"
                 "PT 1,1,1,1,0,0,0,;"
                 "CN -1;"));
}

//void initGali(GCon g){
//    //                 "AC 1000000000,1000000000,1000000000,1000000000,1000000000,1000000000,1000000000,1000000000;"
//  //  "DC 1000000000,1000000000,1000000000,1000000000,1000000000,1000000000,1000000000,1000000000;"
//    galil(GCmd(g,"ST ABCDEFG;"));
//    galil(GMotionComplete(g,"ABCDEFG"));
//    galil(GCmd(g,"SH ABCDEFG"));
//    galil(GMotionComplete(g,"ABCDEFG"));
//    galil(GCmd(g,"TL 1,1,0.5,0.5,5.0,5.0,5.0;"
//                 "TK 1,1,0.5,0.5,9.9,9.9,2;"
//                 "LD 3,3,3,3,3,3,3,3;"
//                 "ER -1,-1,-1,-1,-1,-1,-1;"
//                 "OE 1,1,1,1,1,1,1,1;"
//                 "KPA=8;KDA=64; KPB=8;KDB=64; KPC=8;KDC=64; KPD=8;KDD=64;"
//                 "KPE=12;KIE=0.2;KDE=120; "
//                 "KPF=12;KIF=0.2;KDF=120; "
//                 "KPG=12;KIG=0.2;KDG=120; "
//                 "NBE=0.5;NZE=0.5;NFE=0;"
//                 "AC 1000000,1000000,1000000,1000000,1000000,1000000,1000000,100000;"
//                 "DC 1000000,1000000,1000000,1000000,1000000,1000000,1000000,100000;"
//                 "SP 0,0,0,0,0,0,0,0;"
//                 "DP 0,0,0,0,0,0,0,0;"
//                 "PT 1,1,1,1,0,0,0,0;"));
//}


void resetGalil(GCon g)
{
    galil(GCmd(g, "RS"));

}


//void initPos()
//{
//    galil(GCmd(g, ""));
//}
