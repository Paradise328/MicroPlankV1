#include "MotorDriver.h"

MotorDriver* MotorDriver::m_selfPointer = nullptr;

void MotorDriver::loadPDOMapping(){
    try {
            toml::table data = toml::parse_file(m_mappingPath);
            const auto& ZeroErrRxPDOData = data["ZeroErr_PDO"]["RxPDO"].as_table();
            for (auto it = ZeroErrRxPDOData->begin(); it != ZeroErrRxPDOData->end(); it++){
                m_config[0].RxPDO.variables[static_cast<std::string>(it->first)].type = *(data["ZeroErr_PDO"]["RxPDO"][it->first]["type"].value<std::string>());
                m_config[0].RxPDO.variables[static_cast<std::string>(it->first)].offset = *(data["ZeroErr_PDO"]["RxPDO"][it->first]["offset"].value<int>());
            }

            const auto& ZeroErrTxPDOData = data["ZeroErr_PDO"]["TxPDO"].as_table();
            for (auto it = ZeroErrTxPDOData->begin(); it != ZeroErrTxPDOData->end(); it++) {
                m_config[0].TxPDO.variables[static_cast<std::string>(it->first)].type = *(data["ZeroErr_PDO"]["TxPDO"][it->first]["type"].value<std::string>());
                m_config[0].TxPDO.variables[static_cast<std::string>(it->first)].offset = *(data["ZeroErr_PDO"]["TxPDO"][it->first]["offset"].value<int>());
            }

            const auto& MOONSRxPDOData = data["MOONS_PDO"]["RxPDO"].as_table();
            for (auto it = MOONSRxPDOData->begin(); it != MOONSRxPDOData->end(); it++) {
                m_config[1].RxPDO.variables[static_cast<std::string>(it->first)].type = *(data["MOONS_PDO"]["RxPDO"][it->first]["type"].value<std::string>());
                m_config[1].RxPDO.variables[static_cast<std::string>(it->first)].offset = *(data["MOONS_PDO"]["RxPDO"][it->first]["offset"].value<int>());
            }

            const auto& MOOONSTxPDOData = data["MOONS_PDO"]["TxPDO"].as_table();
            for (auto it = MOOONSTxPDOData->begin(); it != MOOONSTxPDOData->end(); it++) {
                m_config[1].TxPDO.variables[static_cast<std::string>(it->first)].type = *(data["MOONS_PDO"]["TxPDO"][it->first]["type"].value<std::string>());
                m_config[1].TxPDO.variables[static_cast<std::string>(it->first)].offset = *(data["MOONS_PDO"]["TxPDO"][it->first]["offset"].value<int>());
            }

            const auto& MaxonRxPDOData = data["Maxon_PDO"]["RxPDO"].as_table();
            for (auto it = MaxonRxPDOData->begin(); it != MaxonRxPDOData->end(); it++) {
                m_config[2].RxPDO.variables[static_cast<std::string>(it->first)].type = *(data["Maxon_PDO"]["RxPDO"][it->first]["type"].value<std::string>());
                m_config[2].RxPDO.variables[static_cast<std::string>(it->first)].offset = *(data["Maxon_PDO"]["RxPDO"][it->first]["offset"].value<int>());
            }

            const auto& MaxonTxPDOData = data["Maxon_PDO"]["TxPDO"].as_table();
            for (auto it = MaxonTxPDOData->begin(); it != MaxonTxPDOData->end(); it++) {
                m_config[2].TxPDO.variables[static_cast<std::string>(it->first)].type = *(data["Maxon_PDO"]["TxPDO"][it->first]["type"].value<std::string>());
                m_config[2].TxPDO.variables[static_cast<std::string>(it->first)].offset = *(data["Maxon_PDO"]["TxPDO"][it->first]["offset"].value<int>());
            }

            LOG(INFO) << "MAXON RxPDO are as follow: ";
            for (const auto& item : m_config[2].RxPDO.variables) {
               LOG(INFO) << item.first << ", " << item.second.type << ", " << item.second.offset;
            }
            LOG(INFO) << "MAXON TxPDO are as follow: ";
            for (const auto& item : m_config[2].TxPDO.variables) {
               LOG(INFO) << item.first << ", " << item.second.type << ", " << item.second.offset;
            }
    }
    catch(const toml::parse_error& err){
        LOG(ERROR) << "Failed to parse toml file: " << err.what();
    }
}


/*! Dumps a rcX packet to debug console
*   \param ptPacket Pointer to packed being dumped                           */
/*****************************************************************************/
void MotorDriver::dumpPacket(CIFX_PACKET* ptPacket)
{
#ifdef DEBUG
    printf("%s() called\n", __FUNCTION__);
#endif
  printf("Dest   : 0x%08lX      ID   : 0x%08lX\r\n",(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulDest),  (long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulId));
  printf("Src    : 0x%08lX      Sta  : 0x%08lX\r\n",(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulSrc),   (long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulState));
  printf("DestID : 0x%08lX      Cmd  : 0x%08lX\r\n",(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulDestId),(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulCmd));
  printf("SrcID  : 0x%08lX      Ext  : 0x%08lX\r\n",(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulSrcId), (long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulExt));
  printf("Len    : 0x%08lX      Rout : 0x%08lX\r\n",(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulLen),   (long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulRout));

  printf("Data:");
  dumpData(ptPacket->abData, HOST_TO_LE32(ptPacket->tHeader.ulLen));
}


void MotorDriver::dumpData(unsigned char* data, unsigned long dataLength){

#ifdef DEBUG
    printf("%s() called\n", __FUNCTION__);
#endif
  for(unsigned long ulIdx = 0; ulIdx < dataLength; ++ulIdx)
  {
    if(0 == (ulIdx % 16))
      printf("\r\n");

    printf("%02X ", data[ulIdx]);
  }
  printf("\r\n");
}


uint16_t MotorDriver::getErrorCode(const MotorType& type, const int& index){

    if(m_config[static_cast<int>(type)].TxPDO.variables.count(ERRCODE) > 0){
        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[ERRCODE];

        if(type == MotorType::ZERO_ERR){
            const auto error = hex2Uint16(m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index],
                                          m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index + 1]);
            return error;
        }
        else if(type == MotorType::MOONS){
            const auto error = hex2Uint16(m_abRecvData[variable.offset + moons_sizeRecvData * index],
                                          m_abRecvData[variable.offset + moons_sizeRecvData * index + 1]);
            return error;
        }
        else if (type == MotorType::MAXON){
            auto error = hex2Uint16(m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + maxon_sizeRecvData * index],
                                    m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + maxon_sizeRecvData * index + 1]);
            return error;
        }
        else{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
    }
    else{
        LOG(ERROR) << "Error: there is no ErrorCode in TxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}


uint16_t MotorDriver::getStatusWord(const MotorType& type, const int& index){
    if(m_config[static_cast<int>(type)].TxPDO.variables.count(STATUSWORD) > 0){
        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[STATUSWORD];
        if(type == MotorType::ZERO_ERR){

            const auto statusWord = hex2Uint16(m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index],
                                               m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index + 1]);
            return statusWord;
        }
        else if(type == MotorType::MOONS){
            const auto statusWord = hex2Uint16(m_abRecvData[variable.offset + moons_sizeRecvData * index],
                                               m_abRecvData[variable.offset + moons_sizeRecvData * index + 1]);
            return statusWord;
        }
        else if(type == MotorType::MAXON){
            const auto statusWord = hex2Uint16(m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + maxon_sizeRecvData * index],
                                               m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + maxon_sizeRecvData * index + 1]);
            return statusWord;
        }
        else{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
    }
    else{
        LOG(ERROR) << "Error: there is no StatusWord in TxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}


int16_t MotorDriver::getOperationMode(const MotorType& type, const int& index){
    if(m_config[static_cast<int>(type)].TxPDO.variables.count(MODEDISPLAY) > 0){
        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[MODEDISPLAY];

        if(type == MotorType::ZERO_ERR){
            const auto operationMode = hex2Int8(m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index]);
            return operationMode;
        }
        else if(type == MotorType::MOONS){
             const auto operationMode = hex2Int8(m_abRecvData[variable.offset + moons_sizeRecvData * index]);
             return operationMode;
        }
        else if(type == MotorType::MAXON){
            const auto operationMode = hex2Int8(m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + maxon_sizeRecvData * index]);
            return operationMode;
        }
        else{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
    }
    else{
        LOG(ERROR) << "Error: there is no OperationMode in TxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}


int32_t MotorDriver::getActualPos(const MotorType& motorType, const int& index){
    if(m_config[static_cast<int>(motorType)].TxPDO.variables.count(ACTPOS) > 0){

        const auto& variable = m_config[static_cast<int>(motorType)].TxPDO.variables[ACTPOS];
        if(motorType == MotorType::ZERO_ERR){
            const auto actualPos = hex2Int32(m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index],
                                             m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index + 1],
                                             m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index + 2],
                                             m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index + 3]);

            return actualPos;
        }
        else if (motorType == MotorType::MOONS){
            const auto actualPos = hex2Int32(m_abRecvData[variable.offset + moons_sizeRecvData * index],
                                             m_abRecvData[variable.offset + moons_sizeRecvData * index + 1],
                                             m_abRecvData[variable.offset + moons_sizeRecvData * index + 2],
                                             m_abRecvData[variable.offset + moons_sizeRecvData * index + 3]);
            return actualPos;
        }
        else if (motorType == MotorType::MAXON){
            const auto actualPos = hex2Int32(m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + maxon_sizeRecvData * index],
                                             m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + maxon_sizeRecvData * index + 1],
                                             m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + maxon_sizeRecvData * index + 2],
                                             m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + maxon_sizeRecvData * index + 3]);
            return actualPos;
        }
        else{ 
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
    }
    else{
        LOG(ERROR) << "Error: there is no ActualPosition in TxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}


int32_t MotorDriver::getActualVel(const MotorType& type, const int& index){
    if(m_config[static_cast<int>(type)].TxPDO.variables.count(ACTVEL) > 0){
        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[ACTVEL];
        if(type == MotorType::ZERO_ERR){
            const auto actualVel = hex2Int32(m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index],
                                             m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index + 1],
                                             m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index + 2],
                                             m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index + 3]);
            return actualVel;
        }
        else if(type ==MotorType::MOONS){
            const auto actualVel = hex2Int32(m_abRecvData[variable.offset + moons_sizeRecvData * index],
                                             m_abRecvData[variable.offset + moons_sizeRecvData * index + 1],
                                             m_abRecvData[variable.offset + moons_sizeRecvData * index + 2],
                                             m_abRecvData[variable.offset + moons_sizeRecvData * index + 3]);
            return actualVel;
        }        
        else if (type == MotorType::MAXON){
            const auto actualPos = hex2Int32(m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + maxon_sizeRecvData * index],
                                             m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + maxon_sizeRecvData * index + 1],
                                             m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + maxon_sizeRecvData * index + 2],
                                             m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + maxon_sizeRecvData * index + 3]);
            return actualPos;
        }
        else{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
    }
    else{
        LOG(ERROR) << "Error: there is no ActualVelocity in TxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}


int16_t MotorDriver::getActualTrq(const MotorType& type, const int& index){
    if(m_config[static_cast<int>(type)].TxPDO.variables.count(ACTTRQ) > 0){
        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[ACTTRQ];
        const auto actualTrq = hex2Int16(m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index],
                                         m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index + 1]);
        return actualTrq;
    }
    else{
        LOG(ERROR) << "Error: there is no ActualTorque in TxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}


int16_t MotorDriver::getActualCur(const MotorType& type, const int& index){
    if(m_config[static_cast<int>(type)].TxPDO.variables.count(ACTCUR) > 0){
        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[ACTCUR];
        const auto actualCur = hex2Int16(m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index],
                                         m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index + 1]);
        return actualCur;
    }
    else{
        LOG(ERROR) << "Error: there is no ActualCurrent in TxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}


std::array<int, 8> MotorDriver::getDigitalInputs(const MotorType& type, const int& index){
    std::array<int, 8> tmpVal{0};
    if(m_config[static_cast<int>(type)].TxPDO.variables.count(DIGITALINPUT) > 0){
        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[DIGITALINPUT];
        if(type == MotorType::ZERO_ERR){
            const auto digitalInputs = hex2Uint32(m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index],
                                                  m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index + 1],
                                                  m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index + 2],
                                                  m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index + 3]);

            tmpVal[0] = (digitalInputs >> 16) & 0x1;
            tmpVal[1] = (digitalInputs >> 17) & 0x1;
            tmpVal[2] = (digitalInputs >> 18) & 0x1;
            tmpVal[3] = (digitalInputs >> 19) & 0x1;
            tmpVal[4] = (digitalInputs >> 20) & 0x1;
            tmpVal[5] = (digitalInputs >> 21) & 0x1;
            tmpVal[6] = (digitalInputs >> 22) & 0x1;
            tmpVal[7] = (digitalInputs >> 23) & 0x1;

            return tmpVal;

        }
        else if(type == MotorType::MOONS){
            const auto digitalInputs = hex2Uint32(m_abRecvData[variable.offset + moons_sizeRecvData * index],
                                                  m_abRecvData[variable.offset + moons_sizeRecvData * index + 1],
                                                  m_abRecvData[variable.offset + moons_sizeRecvData * index + 2],
                                                  m_abRecvData[variable.offset + moons_sizeRecvData * index + 3]);
            tmpVal[0] = (digitalInputs >> 16) & 0x1;
            tmpVal[1] = (digitalInputs >> 17) & 0x1;
            tmpVal[2] = (digitalInputs >> 18) & 0x1;
            tmpVal[3] = (digitalInputs >> 19) & 0x1;
            tmpVal[4] = (digitalInputs >> 20) & 0x1;
            tmpVal[5] = (digitalInputs >> 21) & 0x1;
            tmpVal[6] = (digitalInputs >> 22) & 0x1;
            tmpVal[7] = (digitalInputs >> 23) & 0x1;

            return tmpVal;
        }
        else{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return tmpVal;
        }
    }
    else{
        LOG(ERROR) << "Error: there is no DigitalInput in TxPDO mapping under this motor type, please check PDO setting!";
        return tmpVal;
    }
}


int32_t MotorDriver::getFollowingPosErr(const MotorType& type, const int& index){
    if(m_config[static_cast<int>(type)].TxPDO.variables.count(FOLLOWINGPOSERR) > 0){
        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[FOLLOWINGPOSERR];
        if(type == MotorType::ZERO_ERR){
            const auto followingPosErr = hex2Int32(m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index],
                                                   m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index + 1],
                                                   m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index + 2],
                                                   m_abRecvData[moons_sizeRecvData * m_motorNum + variable.offset + zeroErr_sizeRecvData * index + 3]);
            return followingPosErr;
        }
        else if(type == MotorType::MOONS){
            const auto followingPosErr = hex2Int32(m_abRecvData[variable.offset + zeroErr_sizeRecvData * index],
                                                   m_abRecvData[variable.offset + zeroErr_sizeRecvData * index + 1],
                                                   m_abRecvData[variable.offset + zeroErr_sizeRecvData * index + 2],
                                                   m_abRecvData[variable.offset + zeroErr_sizeRecvData * index + 3]);
            return followingPosErr;
        }
        else{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
    }
    else{
        LOG(ERROR) << "Error: there is no PosFollowingErr in TxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}


int MotorDriver::setControlWord(const MotorType& type, const int& index, const ControlCommand& cmd){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(CONTROLWORD) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[CONTROLWORD];
        if(type == MotorType::ZERO_ERR){
            m_abSendData[moons_sizeSendData * m_motorNum + variable.offset + zeroErr_sizeSendData * index] = static_cast<unsigned char>(cmd);
            return T_NOERROR;
        }
        else if(type == MotorType::MOONS){
            m_abSendData[variable.offset + moons_sizeSendData * index] = static_cast<unsigned char>(cmd);
            return T_NOERROR;
        }
        else if(type == MotorType::MAXON){
            m_abSendData[moons_sizeSendData * m_motorNum + variable.offset + maxon_sizeSendData * index] = static_cast<unsigned char>(cmd);
            return T_NOERROR;
        }
        else{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
    }
    else{
        LOG(ERROR) << "Error: there is no ControlWord in RxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}


int MotorDriver::setOperationMode(const MotorType& type, const int& index, const OperationMode& mode){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(OPMODE) > 0){

        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[OPMODE];

        if(type == MotorType::ZERO_ERR){
            m_abSendData[moons_sizeSendData * m_motorNum + variable.offset + zeroErr_sizeSendData * index] = static_cast<unsigned char>(mode);
            return T_NOERROR;
        }
        else if(type == MotorType::MOONS){
            m_abSendData[variable.offset + moons_sizeSendData * index] = static_cast<unsigned char>(mode);
            return T_NOERROR;
        }
        else if(type == MotorType::MAXON){
            m_abSendData[moons_sizeSendData * m_motorNum + variable.offset + maxon_sizeSendData * index] = static_cast<unsigned char>(mode);
            usleep(50*1000);
            return T_NOERROR;
        }
        else{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
    }
    else{
        LOG(ERROR) << "Error: there is no OperationMode in RxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}

int MotorDriver::setTargetPos(const MotorType& type, const int& index, const int32_t& targetPos){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(TARGETPOS) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[TARGETPOS];
        if(type == MotorType::ZERO_ERR){
            int32ToBytes(targetPos, m_abSendData + moons_sizeSendData * m_motorNum + variable.offset + zeroErr_sizeSendData * index);
            return T_NOERROR;
        }
        else if(type == MotorType::MOONS){
            int32ToBytes(targetPos, m_abSendData + variable.offset + moons_sizeSendData * index);
            return T_NOERROR;
        }
        else if(type == MotorType::MAXON)
        {
            int32ToBytes(targetPos, m_abSendData + moons_sizeSendData * m_motorNum  + variable.offset + maxon_sizeSendData * index);
            return T_NOERROR;
        }
        else{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
    }
    else{
        LOG(ERROR) << "Error: there is no TargetPosition in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}


int MotorDriver::setTargetVel(const MotorType& type, const int& index, const int32_t& targetVel){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(TARGETVEL) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[TARGETVEL];
        if(type == MotorType::ZERO_ERR){
            int32ToBytes(targetVel, m_abSendData + moons_sizeSendData * m_motorNum + variable.offset + zeroErr_sizeSendData * index);
            return T_NOERROR;
        }
        else if(type == MotorType::MOONS){
            int32ToBytes(targetVel, m_abSendData + variable.offset + moons_sizeSendData * index);
            return T_NOERROR;
        }
        else if(type == MotorType::MAXON){
            int32ToBytes(targetVel, m_abSendData  + moons_sizeSendData * m_motorNum + variable.offset + maxon_sizeSendData * index);
            return T_NOERROR;
        }
        else{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
    }
    else{
        LOG(ERROR) << "Error: there is no TargetVelocity in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}


int MotorDriver::setTargetTrq(const MotorType& type, const int& index, const int16_t& targetTrq){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(TARGETTRQ) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[TARGETTRQ];
        int16ToBytes(targetTrq, m_abSendData + moons_sizeSendData * m_motorNum + variable.offset + zeroErr_sizeSendData * index);
        return T_NOERROR;
    }
    else{
        LOG(ERROR) << "Error: there is no TargetTorque in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}


int MotorDriver::setVelOffset(const MotorType& type, const int& index, const int32_t& velOffset){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(VELOFFSET) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[VELOFFSET];
        int32ToBytes(velOffset, m_abSendData + moons_sizeSendData * m_motorNum + variable.offset + zeroErr_sizeSendData * index);
        return T_NOERROR;
    }
    else{
        LOG(ERROR) << "Error: there is no VelocityOffset in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}


int MotorDriver::setTrqOffset(const MotorType& type, const int& index, const int16_t& trqOffset){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(TRQOFFSET) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[TRQOFFSET];
        int16ToBytes(trqOffset, m_abSendData + moons_sizeSendData * m_motorNum + variable.offset + zeroErr_sizeSendData * index);
        return T_NOERROR;
    }
    else{
        LOG(ERROR) << "Error: there is no TorqueOffset in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

int MotorDriver::setDigitalOutputs(const MotorType& type, const int& index, const uint32_t& digitalOutputs){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(DIGITALOUT) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[DIGITALOUT];
        if(type == MotorType::ZERO_ERR){
            uint32ToBytes(digitalOutputs, m_abSendData + moons_sizeSendData * m_motorNum + variable.offset + zeroErr_sizeSendData * index);
            return T_NOERROR;
        }
        else if(type == MotorType::MOONS){
            uint32ToBytes(digitalOutputs, m_abSendData + variable.offset + moons_sizeSendData * index);
            return T_NOERROR;
        }
        else{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
    }
    else{
        LOG(ERROR) << "Error: there is no DigitialOutputs in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}



int MotorDriver::setProfileVel(const MotorType& type, const int& index, const uint32_t& profileVel){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(PROFILEVEL) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[PROFILEVEL];
        uint32ToBytes(profileVel, m_abSendData + moons_sizeSendData * m_motorNum + variable.offset + zeroErr_sizeSendData * index);
        return T_NOERROR;
    }
    else{
        LOG(ERROR) << "Error: there is no ProfileVelocity in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}


int MotorDriver::setProfileAcc(const MotorType& type, const int& index, const uint32_t& profileAcc){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(PROFILEACC) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[PROFILEACC];
        if(type == MotorType::MOONS)
        {
            uint32ToBytes(profileAcc, m_abSendData + moons_sizeSendData * m_motorNum + variable.offset + zeroErr_sizeSendData * index);
            return T_NOERROR;
        }
        if(type == MotorType::MAXON)
        {
            uint32ToBytes(profileAcc, moons_sizeSendData * m_motorNum  + m_abSendData + variable.offset + maxon_sizeSendData * index);
            return T_NOERROR;
        }
    }

    else{
        LOG(ERROR) << "Error: there is no ProfileAcceleration in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

int MotorDriver::setProfileDec(const MotorType& type, const int& index, const uint32_t& profileDec){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(PROFILEDEC) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[PROFILEDEC];
        if(type == MotorType::MAXON)
        {
            uint32ToBytes(profileDec, m_abSendData + moons_sizeSendData * m_motorNum + variable.offset + maxon_sizeSendData * index);
            return T_NOERROR;
        }
    }
    else{
        LOG(ERROR) << "Error: there is no ProfileDeceleration in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}


int MotorDriver::setMaxProfileVel(const MotorType& type, const int& index, const uint32_t& maxProfileVel){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(MAXPROFILEVEL) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[MAXPROFILEVEL];
        uint32ToBytes(maxProfileVel, m_abSendData + moons_sizeSendData * m_motorNum + variable.offset + zeroErr_sizeSendData * index);
        return T_NOERROR;
    }
    else{
        LOG(ERROR) << "Error: there is no max ProfileVelocity in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}


int MotorDriver::setTrqPosLimit(const MotorType& type, const int& index, const uint16_t& trqPosLimit){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(POSTRQLIMIT) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[POSTRQLIMIT];
        uint16ToBytes(trqPosLimit, m_abSendData + moons_sizeSendData * m_motorNum + variable.offset + zeroErr_sizeSendData * index);
        return T_NOERROR;
    }
    else{
        LOG(ERROR) << "Error: there is no positive TorqueLimit in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}


int MotorDriver::setTrqNegLimit(const MotorType& type, const int& index, const uint16_t& trqNegLimit){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(NEGTRQLIMIT) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[NEGTRQLIMIT];
        uint16ToBytes(trqNegLimit, m_abSendData + moons_sizeSendData * m_motorNum + variable.offset + zeroErr_sizeSendData * index);
        return T_NOERROR;
    }
    else{
        LOG(ERROR) << "Error: there is no negative TorqueLimit in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

int MotorDriver::setHomeMethod(const MotorType& type, const int& index, const int& homeMethod){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(HOMEMETHOD) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[HOMEMETHOD];
        m_abSendData[variable.offset + moons_sizeSendData * index] = static_cast<unsigned char>(homeMethod);
        return T_NOERROR;
    }
    else{
        LOG(ERROR) << "Error: there is no HomeMethod in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

int MotorDriver::setHomeOffset(const MotorType& type, const int& index, const int32_t& homeOffset){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(HOMEOFFSET) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[HOMEOFFSET];
        int32ToBytes(homeOffset, &m_abSendData[variable.offset + moons_sizeSendData * index]);
        return T_NOERROR;
    }
    else{
        LOG(ERROR) << "Error: there is no HomingOffset in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}


int MotorDriver::setHomeVel(const MotorType& type, const int& index, const int32_t& homeVel){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(HOMEVEL) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[HOMEVEL];
        int32ToBytes(homeVel, &m_abSendData[variable.offset + moons_sizeSendData * index]);
        return T_NOERROR;
    }
    else{
        LOG(ERROR) << "Error: there is no HomingSpeed in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

int MotorDriver::setHomeAcc(const MotorType& type, const int& index, const int32_t& homeAcc){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(HOMEACC) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[HOMEACC];
        int32ToBytes(homeAcc, &m_abSendData[variable.offset + moons_sizeSendData * index]);
        return T_NOERROR;
    }
    else{
        LOG(ERROR) << "Error: there is no HomingAcc in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}


/*STO Brake, set torque off, this command cannot be operated in enabled state*/
int MotorDriver::setBrake(const int& jointIndex, const SDO_COMMAND& sdoCmd){

    //set tHeader information.
    m_tSendPkt.tHeader.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
    m_tSendPkt.tHeader.ulCmd = static_cast<uint32_t>(UlCmd::ECM_IF_CMD_COE_SDO_DOWNLOAD_REQ);
    m_tSendPkt.tHeader.ulLen = lengthSDODataHeader + sizeof(uint32_t);  // which is 18+4
    m_tSendPkt.tHeader.ulId = PROCESSID;  // id of source process of the packet

    //set SDO data info.
    // Motor Address
    m_tSendPkt.abData[0] = jointIndex;
    m_tSendPkt.abData[1] = 0x01;
    // Transport Type, 0: CoE, 1: AoE
    m_tSendPkt.abData[2] = 0x00;
    m_tSendPkt.abData[3] = 0x00;
    // usAOEPort, only used if AOE transport
    m_tSendPkt.abData[4] = 0x00;
    m_tSendPkt.abData[5] = 0x00;
    // object Idx, SDO value
    m_tSendPkt.abData[6] = static_cast<short int>(MotorServoObjectIndex::STO) & 0xFF;
    m_tSendPkt.abData[7] = static_cast<short int>(MotorServoObjectIndex::STO) >> 8 & 0xFF;
    // subIdx, in this case, it is 0x00
    m_tSendPkt.abData[8] = 0x00;
    // fCompleteAccess, True: complete Access, False: single subindex is accessed
    m_tSendPkt.abData[9] = 0x00;
    // totalBytes, how many bytes does the written variable has
    m_tSendPkt.abData[10] = sizeof (uint32_t);
    m_tSendPkt.abData[11] = 0x00;
    m_tSendPkt.abData[12] = 0x00;
    m_tSendPkt.abData[13] = 0x00;
    // timeout in ms, recommend value: 1000
    m_tSendPkt.abData[14] = ulTimeout & 0xFF;
    m_tSendPkt.abData[15] = ulTimeout >> 8 & 0xFF;
    m_tSendPkt.abData[16] = ulTimeout >> 16 & 0xFF;
    m_tSendPkt.abData[17] = ulTimeout >> 24 & 0xFF;

    // data needs to be written, based on brakeCommand, the motor brakes or releases brake

    m_tSendPkt.abData[18] = static_cast<unsigned char>(sdoCmd);
    m_tSendPkt.abData[19] = 0x00;
    m_tSendPkt.abData[20] = 0x00;
    m_tSendPkt.abData[21] = 0x00;
    //    usleep(1000);  // is sleep necessary here?
    std::lock_guard<std::mutex> lock(m_myMutex);
    m_flagSDO = true;
    return T_NOERROR;
}


int MotorDriver::setMaxPosErr(const int& jointIndex, const SDO_COMMAND& sdoCmd){
    return 0;
    // to be implemented
}

int MotorDriver::setMaxVelErr(const int& jointIndex, const SDO_COMMAND& sdoCmd){
    //set tHeader information.
    m_tSendPkt.tHeader.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
    m_tSendPkt.tHeader.ulCmd = static_cast<uint32_t>(UlCmd::ECM_IF_CMD_COE_SDO_DOWNLOAD_REQ);
    m_tSendPkt.tHeader.ulLen = lengthSDODataHeader + sizeof(uint32_t);  // which is 18+4
    m_tSendPkt.tHeader.ulId = PROCESSID;

    //set SDO data info.
    // Motor Address
    m_tSendPkt.abData[0] = jointIndex;
    m_tSendPkt.abData[1] = 0x01;
    // Transport Type, 0: CoE, 1: AoE
    m_tSendPkt.abData[2] = 0x00;
    m_tSendPkt.abData[3] = 0x00;
    // usAOEPort, only used if AOE transport
    m_tSendPkt.abData[4] = 0x00;
    m_tSendPkt.abData[5] = 0x00;
    // object Idx, SDO value
    m_tSendPkt.abData[6] = static_cast<short int>(MotorServoObjectIndex::VEL_FOLLOWING_ERR_WINDOW) & 0xFF;
    m_tSendPkt.abData[7] = static_cast<short int>(MotorServoObjectIndex::VEL_FOLLOWING_ERR_WINDOW) >> 8 & 0xFF;
    // subIdx, in this case, it is 0x00
    m_tSendPkt.abData[8] = 0x00;
    // fCompleteAccess, True: complete Access, False: single subindex is accessed
    m_tSendPkt.abData[9] = 0x00;
    // totalBytes, how many bytes does the written variable has
    m_tSendPkt.abData[10] = sizeof (uint32_t);
    m_tSendPkt.abData[11] = 0x00;
    m_tSendPkt.abData[12] = 0x00;
    m_tSendPkt.abData[13] = 0x00;
    // timeout in ms, recommend value: 1000
    m_tSendPkt.abData[14] = ulTimeout & 0xFF;
    m_tSendPkt.abData[15] = ulTimeout >> 8 & 0xFF;
    m_tSendPkt.abData[16] = ulTimeout >> 16 & 0xFF;
    m_tSendPkt.abData[17] = ulTimeout >> 24 & 0xFF;

    // data needs to be written, based on brakeCommand, the motor brakes or releases brake

    m_tSendPkt.abData[18] = static_cast<uint32_t>(sdoCmd) & 0xFF;
    m_tSendPkt.abData[19] = static_cast<uint32_t>(sdoCmd) >> 8 & 0xFF;
    m_tSendPkt.abData[20] = static_cast<uint32_t>(sdoCmd) >> 16 & 0xFF;
    m_tSendPkt.abData[21] = static_cast<uint32_t>(sdoCmd) >> 24 & 0xFF;

    std::lock_guard<std::mutex> lock(m_myMutex);
    m_flagSDO = true;

    return T_NOERROR;
}

int MotorDriver::setMotorLock(const MotorType& type, const MotorServoObjectIndex& index){
    if(type == MotorType::MOONS){
        m_tSendPkt.tHeader.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
        m_tSendPkt.tHeader.ulCmd = static_cast<uint32_t>(UlCmd::ECM_IF_CMD_COE_SDO_DOWNLOAD_REQ);
        m_tSendPkt.tHeader.ulLen = lengthSDODataHeader + sizeof(uint32_t);  // which is 18+4
        m_tSendPkt.tHeader.ulId = PROCESSID;

        //set SDO data info.
        // Motor Address
        m_tSendPkt.abData[0] = 0x00;
        m_tSendPkt.abData[1] = 0x01;
        // Transport Type, 0: CoE, 1: AoE
        m_tSendPkt.abData[2] = 0x00;
        m_tSendPkt.abData[3] = 0x00;
        // usAOEPort, only used if AOE transport
        m_tSendPkt.abData[4] = 0x00;
        m_tSendPkt.abData[5] = 0x00;
        // object Idx, SDO value
        m_tSendPkt.abData[6] = static_cast<short int>(index) & 0xFF;
        m_tSendPkt.abData[7] = static_cast<short int>(index) >> 8 & 0xFF;
        // subIdx, in this case, it is 0x00
        m_tSendPkt.abData[8] = 0x02;
        // fCompleteAccess, True: complete Access, False: single subindex is accessed
        m_tSendPkt.abData[9] = 0x00;

        // totalBytes, how many bytes does the written variable has
        m_tSendPkt.abData[10] = sizeof (uint32_t);
        m_tSendPkt.abData[11] = 0x00;
        m_tSendPkt.abData[12] = 0x00;
        m_tSendPkt.abData[13] = 0x00;

        // timeout in ms, recommend value: 1000
        m_tSendPkt.abData[14] = ulTimeout & 0xFF;
        m_tSendPkt.abData[15] = ulTimeout >> 8 & 0xFF;
        m_tSendPkt.abData[16] = ulTimeout >> 16 & 0xFF;
        m_tSendPkt.abData[17] = ulTimeout >> 24 & 0xFF;

        // data needs to be written, based on brakeCommand, the motor brakes or releases brake
        m_tSendPkt.abData[18] = 0x00;
        m_tSendPkt.abData[19] = 0x00;
        m_tSendPkt.abData[20] = 0x02;
        m_tSendPkt.abData[21] = 0x00;

        // sleep to ensure the word is sent to slave through mailbox,
        std::lock_guard<std::mutex> lock(m_myMutex);
        m_flagSDO = true;
        return T_NOERROR;
    }
    else{
        LOG(ERROR) << "Error, this function is not supported for this motor type";
        return T_ERROR;
    }
}

void MotorDriver::motorLockOpen(const MotorType& type, const int& index, const MotorServoObjectIndex& sdo_index){
    if(MotorType::MOONS == type){
        setMotorLock(type, sdo_index);
        usleep(50*1000);

        if(setDigitalOutputs(type, index, 0x20000) != T_NOERROR){
            LOG(ERROR) << "Error set DigitalOutputs  error!" ;
            return;
        }
        usleep(50*1000);

    }else{
        LOG(ERROR) << "Error: This function is not supported for this motor type.";
    }
}

void MotorDriver::motorLockClosed(const MotorType& type, const int& index, const MotorServoObjectIndex& sdo_index){

    setMotorLock(type, sdo_index);
    usleep(50*1000);

    if(MotorType::MOONS == type){
        if(setDigitalOutputs(type, index, 0) != T_NOERROR){
            LOG(ERROR) << "Error set DigitalOutputs!" ;
            return;
        }
        usleep(50*1000);
    }
    else{
        LOG(ERROR) << "Error: This function is not supported for this motor type.";
    }
}

int MotorDriver::setECatMasterState(const MasterState& targetState){
    int lRet;
    if(targetState != MasterState::ECM_IF_STATE_INIT &&  targetState != MasterState::ECM_IF_STATE_PREOP && targetState != MasterState::ECM_IF_STATE_SAFEOP
        && targetState != MasterState::ECM_IF_STATE_OP){
            LOG(ERROR) << "Failed to set target state for EtherCAT master!";
            return T_ERROR;
    }
    else{
        CIFX_PACKET tSendPacket = {{0}};
        CIFX_PACKET tRecvPacket = {{0}};
        tSendPacket.tHeader.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
        tSendPacket.tHeader.ulCmd  = static_cast<uint32_t>(UlCmd::ECM_IF_CMD_SET_MASTER_TARGET_STATE_REQ);
        tSendPacket.tHeader.ulLen  = 1;  // only one byte represents the status. 
        tSendPacket.tHeader.ulId   = PROCESSID;
        tSendPacket.tHeader.ulExt  = 0;
        tSendPacket.abData[0] = static_cast<unsigned char>(targetState);
        

        if(CIFX_NO_ERROR != (lRet = xChannelPutPacket(m_hChannel, &tSendPacket, timeWaitMailBoxFree))){
            LOG(ERROR) << "Error sending packet to device! Error code: 0x" << std::hex << lRet ;
            motorDriverExit();
            return T_ERROR;
        }
        else{
            LOG(INFO) << "Send Packet" ;
            dumpPacket(&tSendPacket);

            if(CIFX_NO_ERROR != (lRet = xChannelGetPacket(m_hChannel, sizeof(tRecvPacket), &tRecvPacket, timeWaitMessage))){
                LOG(ERROR) << "Error getting packet from device! Error code: 0x" << std::hex << lRet ;
                motorDriverExit();
                return T_ERROR;
            }
            else{
                LOG(INFO) << "Received Packet" ;
                dumpPacket(&tRecvPacket);
            }
        }

        return T_NOERROR;
    }
}

int MotorDriver::getECatMasterState(){
    int lRet;
    CIFX_PACKET tSendPacket = {{0}};
    CIFX_PACKET tRecvPacket = {{0}};
    tSendPacket.tHeader.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
    tSendPacket.tHeader.ulCmd  = static_cast<uint32_t>(UlCmd::ECM_IF_CMD_GET_MASTER_CURRENT_STATE_REQ);
    tSendPacket.tHeader.ulLen  = 0;  // only one byte represents the status. 
    tSendPacket.tHeader.ulId   = PROCESSID;
    tSendPacket.tHeader.ulExt  = 0;

    if(CIFX_NO_ERROR != (lRet = xChannelPutPacket(m_hChannel, &tSendPacket, timeWaitMailBoxFree))){
        LOG(ERROR) << "Error sending packet to device! Error code: 0x" << std::hex << lRet ;
        return T_ERROR;
    }
    else{
        if(CIFX_NO_ERROR != (lRet = xChannelGetPacket(m_hChannel, sizeof(tRecvPacket), &tRecvPacket, timeWaitMessage))){
            LOG(ERROR) << "Error getting packet from device! Error code: 0x" << std::hex << lRet ;
            return T_ERROR;
        }
        else{ 
            return hex2Int8(tRecvPacket.abData[1]);
        }
    }
}

int MotorDriver::setECatSlaveState(const SlaveState& targetState, const int& slaveIdx){
    int lRet;
    if(slaveIdx > m_slaveNumber - 1){
        LOG(ERROR) << "The index of the slave is out of range!";
        return T_ERROR;
    }

    if(targetState != SlaveState::ECM_IF_STATE_INIT &&  targetState != SlaveState::ECM_IF_STATE_PREOP && targetState != SlaveState::ECM_IF_STATE_SAFEOP
        && targetState != SlaveState::ECM_IF_STATE_OP && targetState != SlaveState::ECM_IF_STATE_BOOT){
            LOG(ERROR) << "Failed to set target state for EtherCAT slave!";
            return T_ERROR; 
    }
    else{
        CIFX_PACKET tSendPacket = {{0}};
        CIFX_PACKET tRecvPacket = {{0}};
        tSendPacket.tHeader.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
        tSendPacket.tHeader.ulCmd  = static_cast<uint32_t>(UlCmd::ECM_IF_CMD_SET_SLAVE_TARGET_STATE_REQ);
        tSendPacket.tHeader.ulLen  = 3;  // only one byte represents the status. 
        tSendPacket.tHeader.ulId   = PROCESSID;
        tSendPacket.tHeader.ulExt  = 0;
        tSendPacket.abData[0] = static_cast<unsigned char>(slaveIdx);
        tSendPacket.abData[1] = 0x01;
        tSendPacket.abData[2] = static_cast<unsigned char>(targetState);
        

        if(CIFX_NO_ERROR != (lRet = xChannelPutPacket(m_hChannel, &tSendPacket, timeWaitMailBoxFree))){
            LOG(ERROR) << "Error sending packet to device! Error code: 0x" << std::hex << lRet ;
            motorDriverExit();
            return T_ERROR;
        }
        else{
            LOG(INFO) << "Send Packet" ;
            dumpPacket(&tSendPacket);

            if(CIFX_NO_ERROR != (lRet = xChannelGetPacket(m_hChannel, sizeof(tRecvPacket), &tRecvPacket, timeWaitMessage))){
                LOG(ERROR) << "Error getting packet from device! Error code: 0x" << std::hex << lRet ;
                motorDriverExit();
                return T_ERROR;
            }
            else{
                LOG(INFO) << "Received Packet" ;
                dumpPacket(&tRecvPacket);
            }
        }

        return T_NOERROR;
    }
}

int MotorDriver::getECatSlaveState(const int& slaveIdx){
    int lRet; 
    // std::cout << "slaveIdx: " << slaveIdx << std::endl;
    if(slaveIdx > m_slaveNumber - 1){
        LOG(ERROR) << "The index of the slave is out of range!";
        return T_ERROR;
    }
    CIFX_PACKET tSendPacket = {{0}};
    CIFX_PACKET tRecvPacket = {{0}};
    tSendPacket.tHeader.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
    tSendPacket.tHeader.ulCmd  = static_cast<uint32_t>(UlCmd::ECM_IF_CMD_GET_SLAVE_CURRENT_STATE_REQ);
    tSendPacket.tHeader.ulLen  = 2;  // only one byte represents the status. 
    tSendPacket.tHeader.ulId   = PROCESSID;
    tSendPacket.tHeader.ulExt  = 0;
    tSendPacket.abData[0] = static_cast<unsigned char>(slaveIdx);
    tSendPacket.abData[1] = 0x01;

    if(CIFX_NO_ERROR != (lRet = xChannelPutPacket(m_hChannel, &tSendPacket, timeWaitMailBoxFree))){
        LOG(ERROR) << "Error sending packet to device! Error code: 0x" << std::hex << lRet ;
        return T_ERROR;
    }
    else{
        if(CIFX_NO_ERROR != (lRet = xChannelGetPacket(m_hChannel, sizeof(tRecvPacket), &tRecvPacket, timeWaitMessage))){
            LOG(ERROR) << "Error getting packet from device! Error code: 0x" << std::hex << lRet ;
            return T_ERROR;
        }
        else{
            return hex2Int8(tRecvPacket.abData[2]);
        }
    }
}

// initial Driver, Channel and logout Channel information.
int MotorDriver::motorDriverInit(){

    int lRet = cifXDriverInit(&m_init);
    if(lRet != CIFX_NO_ERROR){
        LOG(ERROR) << "Error initializing driver! Error code: 0x" << std::hex << lRet ;
        return lRet;
    }

    if (CIFX_NO_ERROR != (lRet = xDriverOpen(&m_hDriver)) ){
        LOG(ERROR) << "Error opening cifX driver! Error code: 0x" << std::hex << lRet ;
        return lRet;
    }

    if(CIFX_NO_ERROR != (lRet = xChannelOpen(m_hDriver, CIFX_DEV, 0, &m_hChannel))){
        LOG(ERROR) << "Error opening channel! Error code: 0x" << std::hex << lRet ;
        return lRet;
    }

    CHANNEL_INFORMATION tChannelInfo = {{0}};
    if(CIFX_NO_ERROR != (lRet = xChannelInfo(m_hChannel, sizeof(CHANNEL_INFORMATION), &tChannelInfo))){
        std::cerr << "Error querying system information block! Error code: " << lRet ;
        return lRet;
    }

    printf("Communication Channel Info:\r\n");
    printf("Device Number    : %lu\r\n",(long unsigned int)tChannelInfo.ulDeviceNumber);
    printf("Serial Number    : %lu\r\n",(long unsigned int)tChannelInfo.ulSerialNumber);
    printf("Firmware         : %s\r\n", tChannelInfo.abFWName);
    printf("FW Version       : %u.%u.%u build %u\r\n",
            tChannelInfo.usFWMajor,
            tChannelInfo.usFWMinor,
            tChannelInfo.usFWRevision,
            tChannelInfo.usFWBuild);
    printf("FW Date          : %02u/%02u/%04u\r\n",
            tChannelInfo.bFWMonth,
            tChannelInfo.bFWDay,
            tChannelInfo.usFWYear);

    printf("Mailbox Size     : %lu\r\n",(long unsigned int)tChannelInfo.ulMailboxSize);

    lRet = openBusConnection();
    return lRet;
}

int MotorDriver::mailboxPacketTransfer(){
    int lRet;
    uint ulSendPktCount = 0;
    uint ulRecvPktCount = 0;
    xChannelGetMBXState(m_hChannel, (uint32_t*)&ulRecvPktCount, (uint32_t*)&ulSendPktCount);
    LOG(INFO) << "Channel Mailbox State: MaxSend = " << ulSendPktCount << ", Pending Receive = " << ulRecvPktCount ;

    if(CIFX_NO_ERROR != (lRet = xChannelPutPacket(m_hChannel, &m_tSendPkt, timeWaitMailBoxFree))){
        LOG(ERROR) << "Error sending packet to device! Error code: 0x" << std::hex << lRet ;
        return T_ERROR;
    }
    else{
        LOG(INFO) << "Send Packet" ;
        dumpPacket(&m_tSendPkt);

        if(CIFX_NO_ERROR != (lRet = xChannelGetPacket(m_hChannel, sizeof(m_tRecvPkt), &m_tRecvPkt, timeWaitMessage))){
            LOG(ERROR) << "Error getting packet from device! Error code: 0x" << std::hex << lRet ;
            return T_ERROR;
        }
        else{
            LOG(INFO) << "Received Packet" ;
            dumpPacket(&m_tRecvPkt);
        }
    }
    return T_NOERROR;
}


// call this function before starting cyclic Data Transfer
int MotorDriver::openBusConnection(){
    int lRet;
    unsigned long ulState;
    if(CIFX_NO_ERROR != (lRet = xChannelBusState(m_hChannel, CIFX_BUS_STATE_ON, (uint32_t*) &ulState, timeOutOpenBus))){
        LOG(ERROR) << "Error opening Bus connection! Error Code: 0x" << std::hex << lRet ;
        xChannelClose(m_hChannel);
        xDriverClose(m_hDriver);
    }
    return lRet;
}

// Before shutting down channel and drive, set Bus connection off
int MotorDriver::closeBusConnection(){
    int lRet;
    unsigned long ulState;
    if(CIFX_NO_ERROR != (lRet = xChannelBusState(m_hChannel, CIFX_BUS_STATE_OFF, (uint32_t*) &ulState, ulTimeout))){
        LOG(ERROR) << "Error shutting down Bus connection! Error Code: 0x" << std::hex << lRet ;
        xChannelClose(m_hChannel);
        xDriverClose(m_hDriver);
    }
    return lRet;
}

// This method should be called after openBusConnection()
int MotorDriver::cyclicDataTransfer(){
    int lRet;

    m_counter ++;
    if(CIFX_NO_ERROR != (lRet = xChannelIORead(m_hChannel, 0, 0, sizeof(m_abRecvData), m_abRecvData, timeOutCyclicIO))){
        LOG(ERROR) << "Error reading IO Data area! Error Code: 0x" << std::hex << lRet ;
        return T_ERROR;
        
    }
    else{
        if (CIFX_NO_ERROR != (lRet = xChannelIOWrite(m_hChannel, 0, 0, sizeof(m_abSendData), m_abSendData, timeOutCyclicIO))){
            LOG(ERROR) << "Error writing IO Data area! Error Code: 0x" << std::hex << lRet ;
            return T_ERROR;
        }
    }
    return T_NOERROR;
}


void MotorDriver::enableMotor(const MotorType& type, const int& index, const bool& flagPPMode){

    int prefix = 0;
    if(type == MotorType::MAXON){
        prefix = 0;
    }

    if (m_jointEnabled[prefix + index]){
        LOG(INFO) << "This joint is already enabled." ;
        if(flagPPMode){
            if(setControlWord(type, index, ControlCommand::ENABLE) != T_NOERROR){
                LOG(ERROR) << "Error switching on motor!" ;
            }
            usleep(50*1000);
        }
        return;
    }

    LOG(INFO) << "Starting initialize Motor Type " << static_cast<int>(type) << ": joint " << index + 1;

    if(setControlWord(type, index, ControlCommand::CLEAR_ERROR) != T_NOERROR){
        LOG(ERROR) << "Error clearing error!" ;
        return;
    }
    usleep(50*1000);
    LOG(INFO) << "1: the control word is: " << static_cast<int>(ControlCommand::CLEAR_ERROR) <<  " " << "status word is: 0x" << std::hex << getStatusWord(type, index);

    if(setControlWord(type, index, ControlCommand::SHUT_DOWN) != T_NOERROR){
        LOG(ERROR) << "Error shutting down motor!" ;
        return;
    }
    usleep(50*1000);
    LOG(INFO) << "2: the control word is: " << static_cast<int>(ControlCommand::SHUT_DOWN) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

    if(setControlWord(type, index, ControlCommand::SWITCH_ON) != T_NOERROR){
        LOG(ERROR) << "Error switching on motor!";
        return;
    }
    usleep(50*1000);
    LOG(INFO) << "3: the control word is: " << static_cast<int>(ControlCommand::SWITCH_ON) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

    if(setControlWord(type, index, ControlCommand::ENABLE) != T_NOERROR){
        LOG(ERROR) << "Error shutting down motor!" << std::endl;
        return;
    }
    usleep(50*1000);
    LOG(INFO) << "4: the control word is: " << static_cast<int>(ControlCommand::ENABLE) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

    m_jointEnabled[prefix + index] = true;
    LOG(INFO) << "successfully enable Motor Type" << static_cast<int>(type) << ": joint " << index + 1 ;
    std::cout << std::endl;
}

void MotorDriver::operationCSP(const MotorType& type, const int& index){

    if(type == MotorType::MOONS){
        LOG(WARNING) << "This function is not implemented for MOONS Motor.";
        return;
    }

    // LOG(INFO) << "Start setting operation mode to CSP for ZeroErr." ;

    // if(setOperationMode(type, index, OperationMode::CSP) != T_NOERROR){
    //     LOG(ERROR) << "Failed to set operation mode to CSP for ZeroErr!" ;
    //     return;
    // }

    // const auto actualPos = getActualPos(type, index);
    // if(setTargetPos(type, index, actualPos) != T_NOERROR){
    //     LOG(ERROR) << "Failed to set actual pos to target pos for ZeroErr!" ;
    //     return;
    // }

    // if(setMaxProfileVel(type, index, MAX_PROFILE_VEL) != T_NOERROR){
    //     LOG(ERROR) << "Failed to set max profile vel for ZeroErr!" ;
    //     return;
    // }

    // if(setProfileAcc(type, index, PROFILE_ACC) != T_NOERROR){
    //     LOG(ERROR) << "Failed to set profile acceleration for ZeroErr!" ;
    //     return;
    // }

    // if(setProfileDec(type, index, PROFILE_DEC) != T_NOERROR){
    //     LOG(ERROR) << "Failed to set profile deceleration for ZeroErr!" ;
    //     return;
    // }

    // if (setMaxVelErr(index, SDO_COMMAND::MAX_VEL_ERR) != T_NOERROR){
    //     LOG(ERROR) << " Failed to set SDO 0x3B60: Max Velocity Error for ZeroErr!" ;
    //     return;
    // }
    // else{
    //     m_flagSDO = false;
    // }

    // usleep(50*1000);
    // enableMotor(type, index, false);

    if(type == MotorType::MAXON)
    {
        LOG(INFO) << "Start setting operation mode to CSP for MAXON." ;

        if(setOperationMode(type, index, OperationMode::CSP) != T_NOERROR){
            LOG(ERROR) << "Failed to set operation mode to CSP for MAXON!" ;
            return;
        }

        const auto actualPos = getActualPos(type, index);
        if(setTargetPos(type, index, actualPos) != T_NOERROR){
            LOG(ERROR) << "Failed to set actual pos to target pos for MAXON!" ;
            return;
        }

        // if(setMaxProfileVel(type, index, MAX_PROFILE_VEL) != T_NOERROR){
        //     LOG(ERROR) << "Failed to set max profile vel for ZeroErr!" ;
        //     return;
        // }

        if(setProfileAcc(type, index, PROFILE_ACC) != T_NOERROR){
            LOG(ERROR) << "Failed to set profile acceleration for MAXON!" ;
            return;
        }

        if(setProfileDec(type, index, PROFILE_DEC) != T_NOERROR){
            LOG(ERROR) << "Failed to set profile deceleration for MAXON!" ;
            return;
        }

        if (setMaxVelErr(index, SDO_COMMAND::MAX_VEL_ERR) != T_NOERROR){
            LOG(ERROR) << " Failed to set SDO 0x3B60: Max Velocity Error for ZeroErr!" ;
            return;
        }else{
            m_flagSDO = false;
        }
        usleep(100*1000);
        enableMotor(type, index, false);
    }
}

void MotorDriver::operationCSV(const MotorType& motorType, const int& index){

    if(MotorType::MOONS == motorType)
    {
        LOG(INFO) << "Start setting operation mode to CSV for MOONS." ;
        if(setOperationMode(motorType, index, OperationMode::CSV) != T_NOERROR){
            LOG(ERROR) << "Failed to set operation mode to CSV for MOONS!";
            return;
        }
        if(setTargetVel(motorType, index, zeroVel) != T_NOERROR){
            LOG(ERROR) << "Failed to set current vel to 0 for MOONS!";
            return;
        }
        if(setProfileAcc(motorType, index, PROFILE_ACC) != T_NOERROR){
            LOG(ERROR) << "Failed to set profile acceleration for MOONS!" << std::endl;
            return;
        }
        if(setProfileDec(motorType, index, PROFILE_DEC) != T_NOERROR){
            LOG(ERROR) << "Failed to set profile deceleration for MOONS!" << std::endl;
            return;
        }
        enableMotor(motorType, index, false);
        usleep(50*1000);
    }
    else if(MotorType::ZERO_ERR == motorType)
    {
        LOG(INFO) << "Start setting operation mode to CSV for ZeroErr." ;
        if(setOperationMode(motorType, index, OperationMode::CSV) != T_NOERROR){
            LOG(ERROR) << "Failed to set operation mode to CSV for ZeroErr!" << std::endl;
            return;
        }
        if(setTargetVel(motorType, index, zeroVel) != T_NOERROR){
            LOG(ERROR) << "Failed to set current vel to 0 for ZeroErr!" << std::endl;
            return;
        }
        if(setMaxProfileVel(motorType, index, MAX_PROFILE_VEL) != T_NOERROR){
            LOG(ERROR)<< "Failed to set max profile vel for ZeroErr!" << std::endl;
            return;
        }
        if(setProfileAcc(motorType, index, PROFILE_ACC) != T_NOERROR){
            LOG(ERROR) << "Failed to set profile acceleration for ZeroErr!" << std::endl;
            return;
        }
        if(setProfileDec(motorType, index, PROFILE_DEC) != T_NOERROR){
            LOG(ERROR) << "Failed to set profile deceleration for ZeroErr!" << std::endl;
            return;
        }
        if (setMaxVelErr(index, SDO_COMMAND::MAX_VEL_ERR) != T_NOERROR){
            LOG(ERROR) << " Failed to set SDO 0x3B60: Max Velocity Error for ZeroErr!" << std::endl;
            return;
        } 
        else{
            m_flagSDO = false;
        }
        usleep(50*1000);
        enableMotor(motorType, index, false);
    }
    else if(MotorType::MAXON == motorType)
    {
        LOG(INFO) << "Start setting operation mode to CSV for Maxon." ;
        dumpData(m_abRecvData, sizeof(m_abRecvData));
        if(setOperationMode(motorType, index, OperationMode::CSV) != T_NOERROR){
            std::cout << "Failed to set operation mode to CSV" << std::endl;
            return;
        }
        
        if(setTargetVel(motorType, index, zeroVel) != T_NOERROR){
            std::cerr << "Failed to set current vel to 0" << std::endl;
            return;
        }
        if(setProfileAcc(motorType, index, PROFILE_ACC) != T_NOERROR){
            LOG(ERROR) << "Failed to set profile acceleration for Maxon!" << std::endl;
            return;
        }
        if(setProfileDec(motorType, index, PROFILE_DEC) != T_NOERROR){
            LOG(ERROR) << "Failed to set profile deceleration for Maxon!" << std::endl;
            return;
        }
        else{
            m_flagSDO = false;
        }
        usleep(100*1000);
        enableMotor(motorType, index, false);
        // LOG(DEBUG)<<"ERROR";
    }
}

void MotorDriver::operationCST(const MotorType& type, const int& index){

    if(type == MotorType::MOONS){
        LOG(WARNING) << "This function is not implemented for MOONS Motor.";
        return;
    }

    LOG(INFO) << "Starting set operation mode to CST for ZeroErr";

    if(setOperationMode(type, index, OperationMode::CST) != T_NOERROR){
        LOG(ERROR) << "Failed to set operation mode to CST for ZeroErr!" ;
        return;
    }

    const int16_t trq = 0;
    if(setTargetTrq(type, index, trq) != T_NOERROR){
        LOG(ERROR) << "Failed to set current torque to 0 for ZeroErr!" ;
        return;
    }

    if(setMaxProfileVel(type, index, MAX_PROFILE_VEL) != T_NOERROR){
        LOG(ERROR) << "Failed to set max profile vel for ZeroErr!" ;
        return;
    }

    /* Following items need to be confirmed in order to use CST */
    /* ----------------------------------------------------------
     * 1. set max allowed torque ???
     * -------------------------------------------------------- */

    usleep(50*1000);
    enableMotor(type, index, false);
}

void MotorDriver::operationPP(const MotorType& type, const int& index){

    if(type == MotorType::MOONS){
        LOG(WARNING) << "This function is not implemented for MOONS Motor.";
        return;
    }
    LOG(INFO) << "Starting set operation mode to PP for ZeroErr." ;

    const auto actualPos = getActualPos(type, index);

    if(setTargetPos(type, index, actualPos) != T_NOERROR){
        LOG(ERROR) << "Failed to set actual pos to target pos for ZeroErr!" ;
        return;
    }

    if(setOperationMode(type, index, OperationMode::PP) != T_NOERROR){
        LOG(ERROR) << "Failed to set operation mode to PP for ZeroErr!" ;
        return;
    }

    if(setMaxProfileVel(type, index, MAX_PROFILE_VEL) != T_NOERROR){
        LOG(ERROR) << "Failed to set max profile vel for ZeroErr!";
        return;
    }
}

void MotorDriver::operationHOME(const MotorType& type, const int& index){

    if(type == MotorType::ZERO_ERR){
        LOG(WARNING) << "This function is not implemented for ZeroErr Motor.";
        return;
    }

    LOG(INFO) << "Starting set operation mode to HOME for MOONS";

    if(setOperationMode(type, index, OperationMode::HOME) != T_NOERROR){
        LOG(ERROR)<< "Failed to set operation mode to HomeMode for MOONS";
        return;
    }

    LOG(INFO) << "OP Mode is: 0x" << std::hex << getOperationMode(type, index);

    int homeMode = 3;
    if (setHomeMethod(type, index, homeMode)){
        LOG(ERROR) << "Failed to set profile HomeMethod for MOONS!";
        return;
    }

    if (setHomeVel(type, index, zeroVel)){
        LOG(ERROR) << "Failed to set profile SearchZeroVel for MOONS!";
        return;
    }

    if (setHomeAcc(type, index, PROFILE_ACC) != T_NOERROR){
        LOG(ERROR) << "Failed to set profile HomingAcc for MOONS!";
        return;
    }

    const int32_t velOffset = 0;
    if (setHomeOffset(type, index, velOffset)){
        LOG(ERROR) << "Failed to set profile HomingOffset!";
        return;
    }
}

void MotorDriver::motorDriverThread(std::promise<bool> &promiseCommunication){
    auto lRet = m_selfPointer->motorDriverInit();
    if(lRet != CIFX_NO_ERROR){
        LOG(FATAL) << "cifX driver cannot be initialized, hence, motor driver thread cannot be started.";
        m_selfPointer->m_flagThreadTerminated.store(true, std::memory_order_release);
        return;
    }

    long int loopCounter = 0;
    while(true){

        if((lRet = m_selfPointer->getECatMasterState()) != static_cast<int>(MasterState::ECM_IF_STATE_OP)){
            LOG(ERROR) << "Connection lost, EtherCAT master is not in OP state, current state is: 0x" << std::hex << lRet;
            break; 
        }

        if((lRet = m_selfPointer->getECatSlaveState(0)) != static_cast<int>(SlaveState::ECM_IF_STATE_OP)){
            LOG(ERROR) << "Connection lost, EtherCAT slave 1 is not in OP state, current state is: 0x" << std::hex << lRet;
            break; 
        }

        // if((lRet = m_selfPointer->getECatSlaveState(1)) != static_cast<int>(SlaveState::ECM_IF_STATE_OP)){
        //     LOG(ERROR) << "Connection lost, EtherCAT slave 2 is not in OP state, current state is: 0x" << std::hex << lRet;
        //     break; 
        // }

        // if((lRet = m_selfPointer->getECatSlaveState(2)) != static_cast<int>(SlaveState::ECM_IF_STATE_OP)){
        //     LOG(ERROR) << "Connection lost, EtherCAT slave 3 is not in OP state, current state is: 0x" << std::hex << lRet;
        //     break; 
        // }

        // if((lRet = m_selfPointer->getECatSlaveState(3)) != static_cast<int>(SlaveState::ECM_IF_STATE_OP)){
        //     LOG(ERROR) << "Connection lost, EtherCAT slave 4 is not in OP state, current state is: 0x" << std::hex << lRet;
        //     break; 
        // }

        if(m_selfPointer->m_flagSDO){
            std::lock_guard<std::mutex> lock(m_selfPointer->m_myMutex);
            if (T_NOERROR != (lRet = m_selfPointer->mailboxPacketTransfer())){
                LOG(ERROR) << "SDO service is offline, connection may be lost." ;
                break;
            }
            m_selfPointer->m_flagSDO = false;
            LOG(INFO) << "already set SDO ! " ;
        }

        if (T_NOERROR != (lRet = m_selfPointer->cyclicDataTransfer())){
            LOG(ERROR) << "PDO service is offline, connection may be lost." ;
            break;
        }

        if (loopCounter == 0){
            promiseCommunication.set_value(true);
            LOG(INFO) <<" Set promise value to true!";
        }
        loopCounter ++;
    }
    m_selfPointer->motorDriverExit();
    m_selfPointer->m_flagThreadTerminated.store(true, std::memory_order_release);
}

void MotorDriver::startThread(std::promise<bool> &promiseCommunication){
    m_etherCatThread = std::thread(motorDriverThread, std::ref(promiseCommunication));
    LOG(INFO) << "EtherCat thread ID: " << m_etherCatThread.get_id();
    m_etherCatThread.detach();
}

bool MotorDriver::isMotorDriverThreadTerminated(){
    const auto isMotorDriverThreadTerminated = m_flagThreadTerminated.load(std::memory_order_acquire);
    return isMotorDriverThreadTerminated;
}

void MotorDriver::motorDriverExit(){
    closeBusConnection();
    xChannelClose(m_hChannel);
    xDriverClose(m_hDriver);
    cifXDriverDeinit();
}

 MotorDriver* MotorDriver::getInstance(const std::string pathMappingPDO, const int& endMotorNum, const int& motorNumber, const int& slaveNumber){
     if(m_selfPointer == nullptr){
         m_selfPointer = new MotorDriver(pathMappingPDO, endMotorNum, motorNumber, slaveNumber);
     }
     return m_selfPointer;
 } 
