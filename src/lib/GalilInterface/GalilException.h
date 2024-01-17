#ifndef GALILEXCEPTION_H  
#define GALILEXCEPTION_H 

#include <stdexcept>

class GalilException: public std::runtime_error{
public:
    explicit GalilException(const std::string& msg = ""): std::runtime_error(msg){

    }
};

class ExcpGalilGeneral: public GalilException{
public:
    explicit ExcpGalilGeneral(const std::string& msg = "Galil command error"): GalilException(msg){};
};

class ExcpReadDigitalInput: public GalilException{
public:
    explicit ExcpReadDigitalInput(const std::string& msg = "Galil Error: read digtial input"): GalilException(msg){};
};

class ExcpReadAnalogInput: public GalilException{
public:
    explicit ExcpReadAnalogInput(const std::string& msg = "Galil Error: read analog input"): GalilException(msg){};    
};

class ExcpReadEncoder: public GalilException{
public:
    explicit ExcpReadEncoder(const std::string& msg = "Galil Error: read encoder"): GalilException(msg){};
};

class ExcpReadEncoderDual: public GalilException{
public:
    explicit ExcpReadEncoderDual(const std::string& msg = "Galil Error: read auxiliary encoder"): GalilException(msg){};
};

class ExcpReadAxisInfo: public GalilException{
public:
    explicit ExcpReadAxisInfo(const std::string& msg = "Galil Error: read axis information"): GalilException(msg){};
};

class ExcpStopMotor: public GalilException{
public:
    explicit ExcpStopMotor(const std::string& msg = "Galil Error: ST command"): GalilException(msg){};
};

class ExcpMotorOff: public GalilException{
public:
    explicit ExcpMotorOff(const std::string& msg = "Galil Error: MO command"): GalilException(msg){};
};

class ExcpMotorOn: public GalilException{
public:
    explicit ExcpMotorOn(const std::string& msg = "Galil Error: BG command"): GalilException(msg){};
};

class ExcpMotionComplete: public GalilException{
public:
    explicit ExcpMotionComplete(const std::string& msg = "Galil Error: wait motion complete"): GalilException(msg){};
};

class ExcpPositionAbsolute: public GalilException{
public:
    explicit ExcpPositionAbsolute(const std::string& msg = "Galil Error: PA command"): GalilException(msg){};
};

class ExcpPositionRelative: public GalilException{
public:
    explicit ExcpPositionRelative(const std::string& msg = "Galil Error: PR command"): GalilException(msg){};
};

class ExcpJog: public GalilException{
public:
    explicit ExcpJog(const std::string& msg = "Galil Error: JG command"): GalilException(msg){};
};


#endif //

