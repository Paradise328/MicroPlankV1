#include "InterfaceToGalil.h"

std::array<int, 2* portNumber> InterfaceToGalil::readDigitialInput() const{
    std::array<int, 2*portNumber> t_value;
    char tmpDigital[G_SMALL_BUFFER];
    GSize read_bytes = 4 * 16; //bytes read in GCommand float=4bytes

    // galil(GCommand(m_gcon, "MG @IN[1],@IN[2],@IN[3],@IN[4],@IN[5],@IN[6],@IN[7],@IN[8],@IN[9],@IN[10],@IN[11],@IN[12],@IN[13],@IN[14],@IN[15],@IN[16]",INTmp,sizeof (INTmp), &read_bytes));
    int returnVal = GCommand(m_gcon, "MG @IN[1],@IN[2],@IN[3],@IN[4],@IN[5],@IN[6],@IN[7],@IN[8],@IN[9],@IN[10],@IN[11],@IN[12],@IN[13],@IN[14],@IN[15],@IN[16]", tmpDigital, sizeof (tmpDigital), &read_bytes);
    if(returnVal != G_NO_ERROR){
        LOG(WARNING) << "Error code: " << returnVal;
        throw ExcpReadDigitalInput();
    }

    std::vector<std::string> result;
    std::stringstream s_stream(tmpDigital); //create string stream from the string
    while(s_stream.good()) {
        std::string substr;
        getline(s_stream, substr, ' '); //get first string delimited by comma
        result.emplace_back(substr);
    }
    for(int i = 0; i < 2 * portNumber; i++) {    //print all splitted strings
        t_value[i] = stoi(result.at(i+1));
    }
    return t_value;
}
 
std::array<double, portNumber> InterfaceToGalil::readAnalogInput() const{
    std::array<double, portNumber> t_value;
    char tmpAnalog[G_SMALL_BUFFER];
    GSize read_bytes = 4 * 8; 

    int returnVal = GCommand(m_gcon,"MG @AN[1],@AN[2],@AN[3],@AN[4],@AN[5],@AN[6],@AN[7],@AN[8]", tmpAnalog, sizeof(tmpAnalog), &read_bytes);
    if(returnVal != G_NO_ERROR){
        LOG(WARNING) << "Error code: " << returnVal;
        throw ExcpReadAnalogInput();
    }

    std::vector<std::string> result;
    std::stringstream s_stream(tmpAnalog); //create string stream from the string
    while(s_stream.good()) {
        std::string substr;
        getline(s_stream, substr, ' '); //get first string delimited by comma
        result.emplace_back(substr);
    }
    for(int i = 0; i < portNumber; i++) {    //print all splitted strings
        t_value[i] = stod(result.at(i+1));
    }
    return t_value;
}

std::array<int, portNumber> InterfaceToGalil::readSwitches(GDataRecord &t_record) const{
    std::array<int, portNumber> t_value;
    t_value[0] = t_record.dmc4000.axis_a_switches;
    t_value[1] = t_record.dmc4000.axis_b_switches;
    t_value[2] = t_record.dmc4000.axis_c_switches;
    t_value[3] = t_record.dmc4000.axis_d_switches;
    t_value[4] = t_record.dmc4000.axis_e_switches;
    t_value[5] = t_record.dmc4000.axis_f_switches;
    t_value[6] = t_record.dmc4000.axis_g_switches;
    t_value[7] = t_record.dmc4000.axis_g_switches;
    return t_value;
}


std::array<int, portNumber> InterfaceToGalil::readEncoderValue() const{
    std::array<int, portNumber> t_value;
    char tmpEncoder[G_SMALL_BUFFER];
    GSize read_bytes = 4 * 8; 

    int returnVal = GCommand(m_gcon,"TP", tmpEncoder, sizeof (tmpEncoder), &read_bytes);
    if(returnVal != G_NO_ERROR){
        LOG(WARNING) << "Error code: " << returnVal;
        throw ExcpReadEncoder();
    }

    std::vector<std::string> result;
    std::stringstream s_stream(tmpEncoder); 
    while(s_stream.good()) {
        std::string substr;
        getline(s_stream, substr, ' '); 
        result.emplace_back(substr);
    }
    for(int i = 0; i < portNumber; i++) {    
        t_value[i] = stoi(result.at(i+1));
    }
    return t_value;
}

std::array<int, portNumber> InterfaceToGalil::readMixEncoderValue() const{
    std::array<int, portNumber> t_value;
    char tmpEncoder[G_SMALL_BUFFER], tmpEncoderDual[G_SMALL_BUFFER];
    GSize read_bytes = 4 * 8;

    int returnValBrush = GCommand(m_gcon,"TP", tmpEncoder, sizeof (tmpEncoder), &read_bytes);
    if(returnValBrush != G_NO_ERROR){
        LOG(WARNING) << "Error code: " << returnValBrush;
        throw ExcpReadEncoder();
    }

    std::vector<std::string> resultBrush;
    std::stringstream s_streamBrush(tmpEncoder);
    while(s_streamBrush.good()) {
        std::string substr;
        getline(s_streamBrush, substr, ' ');
        resultBrush.emplace_back(substr);
    }
    for(int i = 0; i < 4; i++) {
        t_value[i] = stoi(resultBrush.at(i+1));
    }

    int returnValStepper = GCommand(m_gcon, "TD", tmpEncoderDual, sizeof (tmpEncoderDual), &read_bytes);
    if(returnValStepper != G_NO_ERROR){
        LOG(WARNING) << "Error code: " << returnValStepper;
        throw ExcpReadEncoderDual();
    }

    std::vector<std::string> resultStepper;
    std::stringstream s_streamDual(tmpEncoderDual); //create string stream from the string
    while(s_streamDual.good()) {
        std::string substr;
        getline(s_streamDual, substr, ' '); //get first string delimited by comma
        resultStepper.emplace_back(substr);
    }
    for(int i = 0; i < portNumber; i++) {    //print all splitted strings
        t_value[i] = stoi(resultStepper.at(i+1));
    }
    return t_value;
}


std::array<int, portNumber> InterfaceToGalil::readEncoderValueDual() const{
    std::array<int, portNumber> t_value;
    char tmpEncoderDual[G_SMALL_BUFFER];
    GSize read_bytes = 4*8; 

    int returnVal = GCommand(m_gcon, "TD", tmpEncoderDual, sizeof (tmpEncoderDual), &read_bytes);
    if(returnVal != G_NO_ERROR){
        LOG(WARNING) << "Error code: " << returnVal;
        throw ExcpReadEncoderDual();
    }

    std::vector<std::string> result;
    std::stringstream s_stream(tmpEncoderDual); //create string stream from the string
    while(s_stream.good()) {
        std::string substr;
        getline(s_stream, substr, ' '); //get first string delimited by comma
        result.emplace_back(substr);
    }
    for(int i = 0; i < portNumber; i++) {    //print all splitted strings
        t_value[i] = stoi(result.at(i+1));
    }
    return t_value;
}

std::array<int, portNumber> InterfaceToGalil::readEncoderValue(GDataRecord &t_record) const{
    std::array<int, portNumber> t_value;
    t_value[0] = t_record.dmc4000.axis_a_motor_position;
    t_value[1] = t_record.dmc4000.axis_b_motor_position;
    t_value[2] = t_record.dmc4000.axis_c_motor_position;
    t_value[3] = t_record.dmc4000.axis_d_motor_position;
    t_value[4] = t_record.dmc4000.axis_e_motor_position;
    t_value[5] = t_record.dmc4000.axis_f_motor_position;
    t_value[6] = t_record.dmc4000.axis_g_motor_position;
    t_value[7] = t_record.dmc4000.axis_h_motor_position;
    return t_value;
}


std::array<int, portNumber> InterfaceToGalil::readDigitalInput(GDataRecord &t_record) const{
    const int digitalInput = t_record.dmc4000.input_bank_0;
    std::array<int, portNumber> t_value;
    t_value[0] = digitalInput >> 0 & 0x01;
    t_value[1] = digitalInput >> 1 & 0x01;
    t_value[2] = digitalInput >> 2 & 0x01;
    t_value[3] = digitalInput >> 3 & 0x01;
    t_value[4] = digitalInput >> 4 & 0x01;
    t_value[5] = digitalInput >> 5 & 0x01;
    t_value[6] = digitalInput >> 6 & 0x01;
    t_value[7] = digitalInput >> 7 & 0x01;
    return t_value;
}

std::array<int, portNumber> InterfaceToGalil::readAxisInfo() const{
    std::array<int, portNumber> t_value;
    char tmpAxisInfo[G_SMALL_BUFFER];
    GSize read_bytes = 4*8; //bytes read in GCommand int = 4bytes

    int returnVal = GCommand(m_gcon, "TS", tmpAxisInfo, sizeof(tmpAxisInfo), &read_bytes);
    if(returnVal != G_NO_ERROR){
        LOG(WARNING) << "Error code: " << returnVal;
        throw ExcpReadAxisInfo();
    }

    std::vector<std::string> result;
    std::stringstream s_stream(tmpAxisInfo); 

    while(s_stream.good()) {
        std::string substr;
        getline(s_stream, substr, ' '); 
        result.emplace_back(substr);
    }
    for(int i = 0; i < portNumber; i++) {    
        t_value[i] = stoi(result.at(i+1));
    }
    return t_value;
}

//stop all axes
GReturn InterfaceToGalil::motorOff() const{
    try{
        galil(GCmd(m_gcon,"ST ABCDEFG"));
        galil(GMotionComplete(m_gcon, "ABCDEFG"));
        galil(GCmd(m_gcon, "MO;"));

    }
    catch(const ExcpGalilGeneral& e){
        LOG(ERROR) << "Error motor off (all axises), catch expection: " << e.what();
    }
    return	GALIL_EXAMPLE_OK;
}

// stop a single axis
GReturn InterfaceToGalil::motorOff(const char& axis) const{
    char buf[G_SMALL_BUFFER];
    try{
        sprintf(buf, "ST%c", axis);
        galil(GCmd(m_gcon, buf));
        sprintf(buf, "%c", axis);
        galil(GMotionComplete(m_gcon, buf));
        sprintf(buf, "MO%c", axis);
        galil(GCmd(m_gcon, buf));
    }
    catch(const ExcpGalilGeneral& e){
        LOG(ERROR) << "Error motor off (single axis), catch expection: " << e.what();
    }
    return	GALIL_EXAMPLE_OK;
}

GReturn InterfaceToGalil::motorStop() const{
    char buf[G_SMALL_BUFFER];
    try{
        sprintf(buf, "ST ABCDEFG");
        galil(GCmd(m_gcon, buf));
        sprintf(buf, "ABCDEFG");
        galil(GMotionComplete(m_gcon, buf));
        galil(GCmd(m_gcon,"BG EFG;"));

    }
    catch(const ExcpGalilGeneral& e){
        LOG(ERROR) << "Error motor off (single axis), catch expection: " << e.what();
    }
    return	GALIL_EXAMPLE_OK;
}

// stop a single axis
GReturn InterfaceToGalil::motorStop(const char& axis) const{
    char buf[G_SMALL_BUFFER];
    try{
        sprintf(buf, "ST%c", axis);
        galil(GCmd(m_gcon, buf));
        sprintf(buf, "%c", axis);
        galil(GMotionComplete(m_gcon, buf));
//        sprintf(buf, "MO%c", axis);
//        galil(GCmd(m_gcon, buf));
    }
    catch(const ExcpGalilGeneral& e){
        LOG(ERROR) << "Error motor off (single axis), catch expection: " << e.what();
    }
    return	GALIL_EXAMPLE_OK;
}



GReturn InterfaceToGalil::goPosition(const char& axis, const int& t_encoder) const{

    char buf[G_SMALL_BUFFER];
    try{
        sprintf(buf, "DP%c=0;PR%c=%d;BG%c",axis, axis, t_encoder, axis);
        galil(GCmd(m_gcon, buf));
    }
    catch(const ExcpGalilGeneral& e){
        LOG(ERROR) << "Error go position (single axis), catch exception: " << e.what();
    }

    return	GALIL_EXAMPLE_OK;
}

GReturn InterfaceToGalil::goPosition(const int& t_encoderA, const int& t_encoderB, const int& t_encoderC) const{
    char buf[G_SMALL_BUFFER];
    try{
        sprintf(buf, "PA %d,%d,%d;BG",t_encoderA, t_encoderB, t_encoderC);
        galil(GCmd(m_gcon, buf)); // position absolute
        galil(GMotionComplete(m_gcon, "ABC"));
    }
    catch(const ExcpGalilGeneral& e){
        LOG(ERROR) << "Error go position (axis A/B/C), catch expection: " << e.what();
    }
    return	GALIL_EXAMPLE_OK;
}


GReturn InterfaceToGalil::jog(const char& axis, const int& t_speed) const{
    char buf[G_SMALL_BUFFER];
    try{
        sprintf(buf, "JG%c=%d",axis, t_speed);
        galil(GCmd(m_gcon, buf)); 
    }
    catch(const ExcpGalilGeneral& e){
        LOG(ERROR) << "Error jog (single axis), catch expection: " << e.what();
    }

    return	GALIL_EXAMPLE_OK;
}


GReturn InterfaceToGalil::jog(const char& axis1, const int& speed1, 
                              const char& axis2, const int& speed2, 
                              const char& axis3, const int& speed3) const{

    char buf[G_SMALL_BUFFER];
    try{
        sprintf(buf1, "JG%c=%d,JG%c=%d,JG%c=%d", axis1, speed1, axis2, speed2, axis3, speed3);

        galil(GCmd(m_gcon, buf)); 
    }
    catch(const ExcpGalilGeneral& e){
        LOG(ERROR) << "Error jog (axis 1/2/3), catch expection: " << e.what();
    }

    return	GALIL_EXAMPLE_OK;
}

GReturn InterfaceToGalil::jog(const char& axis1, const int& speed1, 
                              const char& axis2, const int& speed2, 
                              const char& axis3, const int& speed3,
                              const char& axis4, const int& speed4) const{

    char buf[G_SMALL_BUFFER];
    try{
        sprintf(buf1, "JG%c=%d,JG%c=%d,JG%c=%d,JG%c=%d", axis1, speed1, axis2, speed2, axis3, speed4, axis3, speed4);
    
        galil(GCmd(m_gcon, buf)); 
    }
    catch(const ExcpGalilGeneral& e){
        LOG(ERROR) << "Error jog (axis 1/2/3/4), catch expection: " << e.what();
    }
    return	GALIL_EXAMPLE_OK;
}

GReturn InterfaceToGalil::positionRelative(const char& axis, const int& t_encoder) const{
    char buf[G_SMALL_BUFFER];
    try{
        sprintf(buf, "PR%c=%d",axis, t_encoder);
        galil(GCmd(m_gcon, buf)); 
        sprintf(buf, "BG%c", axis);
        galil(GCmd(m_gcon, buf)); 
    }
    catch(const ExcpGalilGeneral& e){
        LOG(ERROR) << "Error position relative mode (single axis), catch expection: " << e.what();
    }

    return	GALIL_EXAMPLE_OK;

}

void InterfaceToGalil::setServo() const{
    try{
        galil(GCmd(m_gcon,"JG 0,0,0,,0;  BG ABCE"));
        galil(GCmd(m_gcon, "ACB=3000000;DCB=3000000"));   // acceleration
        galil(GCmd(m_gcon, "ACC=3000000;DCC=3000000"));   // acceleration
        galil(GCmd(m_gcon, "SPD=50000;ACD=500000;DCD=500000"));   // acceleration
        galil(GCmd(m_gcon, "PTD=1"));
        galil(GMotionComplete(m_gcon, "D")); //Wait for motion to complete
        galil(GCmd(m_gcon,"DED=0;DEC=0;DEF=0"));
    }
    catch(const ExcpGalilGeneral& e){
        LOG(ERROR) << "Error set servo, catch expection: " << e.what();
    }
    return;
}

int InterfaceToGalil::connectionTest() const{
    auto digitalInput = readDigitialInput();
    try{
//        auto digitalInput = readDigitialInput();
    }
    catch(const ExcpReadDigitalInput& e){
//        LOG(ERROR) << "Catch exception: " << e.what();
    }

    if(digitalInput[HANDLE_SIGNAL]){
        return CONNECTION_FAILED;
    }
    else{
    }

    if(digitalInput[ROBOT_SIGNAL]){
        return CONNECTION_FAILED;
    }
    else{
    }

    if(digitalInput[FOOT_SWITCH]){
        return CONNECTION_FAILED;
    }
    else{
    }

    if(digitalInput[ROBOT_POWER]){
        return CONNECTION_FAILED;
    }
    else{
    }

    if(digitalInput[DB44]){
    }
    else{
        return CONNECTION_FAILED;
    }
    return CONNECTION_SUCCEED;
}

int InterfaceToGalil::encoderTestbyAxis(const char& axis) const{
    double kp, kd, ki, oe, of, er;
    double o = 0.0, s = 0.1;
    int tv = 0;
    char buf[G_SMALL_BUFFER];

    try{
        // pre
        sprintf(buf, "KP%c=?",axis);
        galil(GCmdD(m_gcon, buf, &kp));

        sprintf(buf, "KI%c=?",axis);
        galil(GCmdD(m_gcon, buf, &ki));

        sprintf(buf, "KD%c=?",axis);
        galil(GCmdD(m_gcon, buf, &kd));

        sprintf(buf, "OE%c=?",axis);
        galil(GCmdD(m_gcon, buf, &oe));

        sprintf(buf, "OF%c=?",axis);
        galil(GCmdD(m_gcon, buf, &of));

        sprintf(buf, "ER%c=?",axis);
        galil(GCmdD(m_gcon, buf, &er));

        sprintf(buf, "KP%c=0;KI%c=0;KD%c=0;OF%c=0;", axis, axis, axis, axis);
        galil(GCmd(m_gcon, buf));

        sprintf(buf,"SH%c",axis);
        galil(GCmd(m_gcon, buf));

        while((std::abs(tv) < 1024) && (o < 2.4995)){
            sprintf(buf, "OF%c=%lf", axis, o);
            galil(GCmd(m_gcon, buf));
            o = o + s;
            usleep(10*1000);
            sprintf(buf, "MG _TV%c=%lf",axis, o);
            galil(GCmdI(m_gcon, buf, &tv));
        }

        // post
        sprintf(buf, "KP%c=%lf", axis, kp);
        galil(GCmd(m_gcon, buf));
        sprintf(buf, "KI%c=%lf", axis, ki);
        galil(GCmd(m_gcon, buf));
        sprintf(buf, "KD%c=%lf", axis, kd);
        galil(GCmd(m_gcon, buf));
        sprintf(buf, "OE%c=%lf", axis, oe);
        galil(GCmd(m_gcon, buf));
        sprintf(buf, "OF%c=%lf", axis, of);
        galil(GCmd(m_gcon, buf));
        sprintf(buf, "ER%c=%lf", axis, er);
        galil(GCmd(m_gcon, buf));
    }
    catch(const ExcpGalilGeneral& e){
        LOG(ERROR) << "Error encoder test (single axis), catch expection: " << e.what();
    }

    //error
    if(o > 2.4995){
        LOG(ERROR) << "Axis " << axis << "No sufficient motion";
        return EncoderTestbyAxisFail;
    }
    else{
        LOG(INFO) << "Axis "<< axis <<" encoder ok";
        return EncoderTestbyAxisSuccess;
    }
}

int InterfaceToGalil::encoderTest() const{
    LOG(INFO) << "Encoder test starts:";

    try{
        galil(GCmd(m_gcon, "light=3"));
        galil(GCmd(m_gcon, "ST ABCDE"));
        galil(GMotionComplete(m_gcon, "ABCDE"));
        galil(GCmd(m_gcon, "MO ABCDE"));
    }
    catch(const ExcpGalilGeneral& e){
        LOG(ERROR) << "Error encoder test, catch expection: " << e.what();
    }

    if(encoderTestbyAxis('A') == EncoderTestbyAxisFail){
        LOG(ERROR) << "Error: Encoder A";
        return EncoderError;
    }
    usleep(1000*200);

    if(encoderTestbyAxis('B') == EncoderTestbyAxisFail){
        LOG(ERROR) << "Error: Encoder B";
        return EncoderError;
    }
    usleep(1000*200);

    if(encoderTestbyAxis('C') == EncoderTestbyAxisFail){
        LOG(ERROR) << "Error: Encoder C";
        return EncoderError;
    }
    usleep(1000*200);

    if(encoderTestbyAxis('D') == EncoderTestbyAxisFail){
        LOG(WARNING) << "Error: Encoder D";
        return EncoderError;
    }
    usleep(1000*200);

    if(encoderTestbyAxis('E') == EncoderTestbyAxisFail){
        LOG(WARNING) << "Error: Encoder E";
        return EncoderError;
    }
    usleep(1000*200);

    galil(GCmd(m_gcon,"light=2"));  // light indicating status
    LOG(INFO) << "Encoder test succeed!";
    return EncoderTestSuccess;
}



GReturn InterfaceToGalil::goTargetSpeedPosition(std::array<std::array<int,8>, 3> targetPosition) const
{
    //first colum: speed; second colum: position;
    char buf[G_SMALL_BUFFER]={0};
//        sprintf(buf,"PA %d,%d,%d,%d;SP %d,%d,%d,%d; JG ,,,,%d,%d,%d;"
//               ,targetPosition[1][0], targetPosition[1][1],
//                targetPosition[1][2], targetPosition[1][3],
////                targetPosition[0][0], targetPosition[0][1],
////                targetPosition[0][2], targetPosition[0][3],
//                    0,  0,  0,  0,
//                targetPosition[0][4], targetPosition[0][5],
//                targetPosition[0][6]);        
        sprintf(buf,"PA %d,%d,%d,%d;SP %d,%d,%d,%d; JG ,,,,%d,%d,%d;"
               ,targetPosition[1][0], targetPosition[1][1],
                targetPosition[1][2], targetPosition[1][3],
                targetPosition[0][0], targetPosition[0][1],
                targetPosition[0][2], targetPosition[0][3],
                targetPosition[0][4], targetPosition[0][5],
                targetPosition[0][6]);
         galil(GCmd(m_gcon, buf));
}

void InterfaceToGalil::initGalil(char side)
{
    if(side == 'l')
    {
        galil(GCmd(m_gcon,"ST ABCDEFG;"));
        galil(GMotionComplete(m_gcon,"ABCDEFG"));
        qDebug()<<"Motion Complete";
        galil(GCmd(m_gcon,"MO ABCDEFG;"));
        qDebug()<<"Motor Off";
        galil(GCmd(m_gcon,"TL 1,1,1.5,1.5;"
                     "TK 1,1,1.5,1.5;"
                     "ER -1,-1,-1,-1,-1,-1,-1;"
                     "MT 1,1,1,-1,-2.5,-2.5,-2.5;"
                     "CE 0,0,0,0,0,0,0;"
                     "AG 0,0,0,0,1,1,1;"
                     "AU 0.5,0.5,0.5,0.5,10,10,10;"
                     "OE 0,0,0,0,0,0,0;"
                     "KS ,,,,7.5,9,7.5;"
                     "OE 1,1,1,1,1,1,1;"
                     "KPA=8;KDA=64; KPB=8;KDB=64; KPC=8;KDC=64; KPD=8;KDD=64;"
                     "KPE=20;KIE=0;KDE=64; "
                     "KPF=20;KIF=0;KDF=64; "
                     "KPG=20;KIG=0;KDG=64; "
                     "NBE=0.5;NZE=0.5;NFE=0;"
                     "AC 1000000,1000000,2000000,2000000,3500000,3500000,3500000,;"
                     "DC 1000000,1000000,2000000,2000000,3500000,3500000,3500000,;"
                     "SP 0,0,0,0,0,0,0,;"
//                     "DP 0,0,0,0,0,0,0,;"
                     "PT 1,1,1,1,0,0,0,;"
                     "CN -1,1;"));
        qDebug()<<"Configure Complete";
        galil(GCmd(m_gcon,"SH ABCDEFG;"));
//        galil(GCmd(m_gcon,"SH EFG;"));

        qDebug()<<"SH Complete";
        galil(GCmd(m_gcon,"JG ,,,,0,0,0,0;BGE;BGF;BGG"));
        qDebug()<<"BG Complete";
    }
    if(side == 'r')
    {
        galil(GCmd(m_gcon,"ST ABCDEFG;"));
        galil(GMotionComplete(m_gcon,"ABCDEFG"));
        qDebug()<<"Motion Complete";
        galil(GCmd(m_gcon,"MO ABCDEFG;"));
        qDebug()<<"Motor Off";

        galil(GCmd(m_gcon,"TL 1,1,1.5,1.5;"
                     "TK 1,1,1.5,1.5;"
                     "ER -1,-1,-1,-1,-1,-1,-1;"
                     "MT 1,1,1,-1,-2.5,-2.5,-2.5;"
                     "CE 0,0,0,0,0,0,0;"
                     "AG 0,0,0,0,1,1,1;"
                     "AU 0.5,0.5,0.5,0.5,10,10,10;"
                     "OE 0,0,0,0,0,0,0;"
                     "OE 1,1,1,1,1,1,1;"
                     "KS ,,,,7.5,9,7.5;"
                     "KPA=8;KDA=64; KPB=8;KDB=64; KPC=8;KDC=64; KPD=8;KDD=64;"
                     "KPE=20;KIE=0;KDE=64; "
                     "KPF=20;KIF=0;KDF=64; "
                     "KPG=20;KIG=0;KDG=64; "
                     "NBE=0.5;NZE=0.5;NFE=0;"
                     "AC 1000000,1000000,2000000,2000000,3500000,3500000,3500000,;"
                     "DC 1000000,1000000,2000000,2000000,3500000,3500000,3500000,;"
                     "SP 0,0,0,0,0,0,0,;"
//                     "DP 0,0,0,0,0,0,0,;"
                     "PT 1,1,1,1,1,1,1,;"
                     "CN -1,1;"));
        qDebug()<<"Configure Complete";
        galil(GCmd(m_gcon,"SH ABCDEFG;"));
        qDebug()<<"SH Complete";
        galil(GCmd(m_gcon,"JG ,,,,0,0,0,0;BGE;BGF;BGG"));
        qDebug()<<"BG Complete";
    }
}

void InterfaceToGalil::resetGalil(char side)

{
    galil(GCmd(m_gcon, "RS"));

}


GReturn InterfaceToGalil::GIMBAL_FindEdge(char side)
{
     galil(GCmd(m_gcon,"SH EFG"));
     galil(GCmd(m_gcon,
//     "AC ,,,,1000000,1000000,1000000;"
//                  "DC ,,,,1000000,1000000,1000000;"
                  "SP ,,,,100000,100000,100000;"));
     std::cout << "EFG Find Edge\n";
     galil(GCmd(m_gcon,"CN -1"));

     galil(GCmd(m_gcon,"FE EFG;BG EFG;"));
     galil(GMotionComplete(m_gcon, "EFG"));
     galil(GCmd(m_gcon,"DE ,,,,0,0,0;"));
     std::cout << " 1. EFG edge found"<<std::endl;
     return	GALIL_EXAMPLE_OK;
}


GReturn InterfaceToGalil::GIMBAL_Calibration(char side){
    std::cout << "Initialization Starts!\n";
    galil(GCmd(m_gcon,"SH EFG;"));
    galil(GCmd(m_gcon,"ST EFG;"));
    galil(GMotionComplete(m_gcon, "EFG"));
    std::cout << "Set the speed and acceleration\n";
    galil(GCmd(m_gcon,"SH EFG"));
    galil(GCmd(m_gcon, "SP ,,,,300000,300000,300000;"));
    qDebug()<<"already set speed in Calibration";

    std::cout << "EFG Find Edge\n";

    galil(GCmd(m_gcon,"CN -1,1"));
    galil(GCmd(m_gcon,"FE EFG;BG EFG;"));
    galil(GMotionComplete(m_gcon, "EFG"));
//    galil(GCmd(m_gcon,"FE E;BG E;"));
//    galil(GMotionComplete(m_gcon, "E"));
//    galil(GCmd(m_gcon,"FE F;BG F;"));
//    galil(GMotionComplete(m_gcon, "F"));
//    galil(GCmd(m_gcon,"FE G;BG G;"));
    galil(GMotionComplete(m_gcon, "G"));
    galil(GCmd(m_gcon,"DP ,,,,0,0,0;"));
    galil(GCmd(m_gcon,"DE ,,,,0,0,0;"));
    std::cout << " 1. EFG edge found"<<std::endl;

    std::cout << "Go Home\n";
    if(side=='l')
    {
        galil(GCmd(m_gcon, "PR ,,,,900000,1200000,1200000; BG EFG;")); // position relative
        galil(GMotionComplete(m_gcon, "EFG"));
//        galil(GCmd(m_gcon, "PR ,,,,900000,0,0; BG E;")); // position relative
//        galil(GMotionComplete(m_gcon, "EFG"));
//        galil(GCmd(m_gcon, "PR ,,,,0,1200000,0; BG F;")); // position relative
//        galil(GMotionComplete(m_gcon, "EFG"));
//        galil(GCmd(m_gcon, "PR ,,,,0,0,1200000; BG G;")); // position relative
        galil(GMotionComplete(m_gcon, "EFG"));
    }
    if(side=='r')
    {
        galil(GCmd(m_gcon, "PR ,,,,900000,1200000,1200000; BG EFG;")); // position relative
        galil(GMotionComplete(m_gcon, "EFG"));
    }
    std::cout << " 2. EFG Ready\n";
    initGalil(side);

    return	GALIL_EXAMPLE_OK;
}

GReturn InterfaceToGalil::INSTRUMENT_FindEdge(char side)
{
    if (side == 'l')
    {
        galil(GCmd(m_gcon,"ST ABCD;"));
        galil(GMotionComplete(m_gcon, "ABCD"));
        galil(GCmd(m_gcon,"SH ABCD;"));
        printf("Set parameters\n");
        galil(GCmd(m_gcon,"AC 100000,100000,100000,100000;"));
        galil(GCmd(m_gcon,"DC 100000,100000,100000,100000;"));
        galil(GCmd(m_gcon,"SP 10000,10000,10000,10000;"));

        galil(GCmd(m_gcon,"CN -1,1,1"));
        printf(" Left ABCD Find Edge\n");
        galil(GCmd(m_gcon,"FE ABCD;BG ABCD;"));
        galil(GMotionComplete(m_gcon, "ABCD"));

        galil(GCmd(m_gcon,"CN -1,1,-1"));
        printf(" 3. ABCD edge found\n");
        return	GALIL_EXAMPLE_OK;
    }
    if (side == 'r')
    {
        galil(GCmd(m_gcon,"ST ABCD;"));
        galil(GMotionComplete(m_gcon, "ABCD"));
        galil(GCmd(m_gcon,"SH ABCD;"));
        galil(GCmd(m_gcon,"AC 100000,100000,100000,100000;"));
        galil(GCmd(m_gcon,"DC 100000,100000,100000,100000;"));
        galil(GCmd(m_gcon,"SP 10000,10000,10000,10000;"));
        printf(" Right ABCD Find Edge\n");
        galil(GCmd(m_gcon,"CN -1,1,1"));
        galil(GCmd(m_gcon,"FE ABCD;BG ABCD;"));
        galil(GMotionComplete(m_gcon, "ABCD"));
        galil(GCmd(m_gcon,"CN -1,1,-1;"));
        printf(" 3. ABCD edge found\n");
        return	GALIL_EXAMPLE_OK;
    }  
}

GReturn InterfaceToGalil::INSTRUMENT_Calibration2(char side){

    if(side == 'l')
    {
    LOG(INFO)<<"Left Instrument Calibration Starts";
    galil(GCmd(m_gcon,"SH ABCD;ST ABCD"));
    LOG(INFO)<<"Left Instrument SH";
    galil(GMotionComplete(m_gcon, "ABCD"));

    char buf1[G_SMALL_BUFFER];
    sprintf(buf1, "JG 10000,10000,5000,-5000;BG ABCD;");
    LOG(INFO)<<"Left Instrument BG";

    galil(GCmd(m_gcon, buf1));
    std::array <int,8> FindEdgeStatus = {0};
    GDataRecord dataRecord;
    while(1){
        galil(GRecord(m_gconDataRecord, &dataRecord, G_DR));

        auto switchesDataCur = readSwitches(dataRecord);

        if(FindEdgeStatus[0]*FindEdgeStatus[1]*FindEdgeStatus[2]*FindEdgeStatus[3] ==1)
        {break;}
        if(switchesDataCur[0] == 110 && m_switchesDataPreL[0] == 108)
        {
            motorStop('A');
            FindEdgeStatus[0] = 1;
            qDebug()<<"A Find Edge";
        }
        if(switchesDataCur[1] == 110 && m_switchesDataPreL[1] == 108)
        {
            motorStop('B');
            FindEdgeStatus[1] = 1;
            qDebug()<<"B Find Edge";
        }
        if(switchesDataCur[2] == 110 && m_switchesDataPreL[2] == 108)
        {
            motorStop('C');
            FindEdgeStatus[2] = 1;
            qDebug()<<"C Find Edge";
        }
        if(switchesDataCur[3] == 110 && m_switchesDataPreL[3] == 108)
        {
            motorStop('D');
            FindEdgeStatus[3] = 1;
            qDebug()<<"D Find Edge";
        }
        m_switchesDataPreL = switchesDataCur;

    }
    }


    if(side == 'r')
    {
    galil(GCmd(m_gcon,"SH ABCD;ST ABCD"));
    galil(GMotionComplete(m_gcon, "ABCD"));

    char buf1[G_SMALL_BUFFER];
    sprintf(buf1, "JG 10000,10000,5000,-5000;BG ABCD;");
    galil(GCmd(m_gcon, buf1));
    std::array <int,8> FindEdgeStatus = {0};
    GDataRecord dataRecord;
    while(1){
        galil(GRecord(m_gconDataRecord, &dataRecord, G_DR));
        auto switchesDataCur = readSwitches(dataRecord);
//        LOG(INFO)<<"switchesDataCur: "<<switchesDataCur[0]<<" "<<switchesDataCur[1]<<
//                   " "<<switchesDataCur[2]<<" "<<switchesDataCur[3];
        if(FindEdgeStatus[0]*FindEdgeStatus[1]*FindEdgeStatus[2]*FindEdgeStatus[3] ==1)
        {break;}
//        LOG(INFO)<<m_switchesDataPreL;
        if(switchesDataCur[0] == 110 && m_switchesDataPreR[0] == 108)
        {
            motorStop('A');
            FindEdgeStatus[0] = 1;
            qDebug()<<"A Find Edge";
        }
        if(switchesDataCur[1] == 110 && m_switchesDataPreR[1] == 108)
        {
            motorStop('B');
            FindEdgeStatus[1] = 1;
            qDebug()<<"B Find Edge";
        }
        if(switchesDataCur[2] == 110 && m_switchesDataPreR[2] == 108)
        {
            motorStop('C');
            FindEdgeStatus[2] = 1;
            qDebug()<<"C Find Edge";
        }
        if(switchesDataCur[3] == 110 && m_switchesDataPreR[3] == 108)
        {
            motorStop('D');
            FindEdgeStatus[3] = 1;
            qDebug()<<"D Find Edge";
        }
        m_switchesDataPreR = switchesDataCur;

    }
    }

    //    galil(GCmd(m_gcon,"SH ABCD;ST ABCD"));
    galil(GCmd(m_gcon,"DP 0,0,0,0;"));

        return	GALIL_EXAMPLE_OK;
}


GReturn InterfaceToGalil::INSTRUMENT_Calibration(char side){
    printf("Initialization Starts!\n");
    galil(GCmd(m_gcon,"SH ABCD;ST ABCD"));
    galil(GMotionComplete(m_gcon, "ABCD"));
    INSTRUMENT_FindEdge(side);
    initGalil(side);

    printf(" 4. ABCD\n");
}
GReturn InterfaceToGalil::readRecordData(GDataRecord &t_record) const{
    try{
        std::lock_guard<std::mutex> lock(m_mtx);
        galil(GRecord(m_gconDataRecord, &t_record, G_DR));
    }
    catch(const ExcpGalilGeneral& e){
        LOG(ERROR) << "Error motor off (all axises), catch expection: " << e.what();
    }
    return	GALIL_EXAMPLE_OK;
}










