#include "MotorDriver.h"

MotorDriver* MotorDriver::m_selfPointer = nullptr;

void MotorDriver::loadPDOMapping(){
    try {
            toml::table data = toml::parse_file(m_mappingPath);

        const auto& MOONSRxPDOData = data["MOONS_PDO"]["RxPDO"].as_table();
        for (auto it = MOONSRxPDOData->begin(); it != MOONSRxPDOData->end(); it++) {
            m_config[0].RxPDO.variables[static_cast<std::string>(it->first)].type = *(data["MOONS_PDO"]["RxPDO"][it->first]["type"].value<std::string>());
            m_config[0].RxPDO.variables[static_cast<std::string>(it->first)].offset = *(data["MOONS_PDO"]["RxPDO"][it->first]["offset"].value<int>());
        }
        const auto& MOOONSTxPDOData = data["MOONS_PDO"]["TxPDO"].as_table();
        for (auto it = MOOONSTxPDOData->begin(); it != MOOONSTxPDOData->end(); it++) {
            m_config[0].TxPDO.variables[static_cast<std::string>(it->first)].type = *(data["MOONS_PDO"]["TxPDO"][it->first]["type"].value<std::string>());
            m_config[0].TxPDO.variables[static_cast<std::string>(it->first)].offset = *(data["MOONS_PDO"]["TxPDO"][it->first]["offset"].value<int>());
        }
        LOG(INFO) << "Moons RxPDO are as follow: ";
        for (const auto& item : m_config[0].RxPDO.variables) {
            LOG(INFO) << item.first << ", " << item.second.type << ", " << item.second.offset;
        }
        LOG(INFO) << "Moons TxPDO are as follow: ";
        for (const auto& item : m_config[0].TxPDO.variables) {
            LOG(INFO) << item.first << ", " << item.second.type << ", " << item.second.offset;
        }

        const auto& ZeroErrRxPDOData = data["ZeroErr_PDO"]["RxPDO"].as_table();
        for (auto it = ZeroErrRxPDOData->begin(); it != ZeroErrRxPDOData->end(); it++){
            m_config[1].RxPDO.variables[static_cast<std::string>(it->first)].type = *(data["ZeroErr_PDO"]["RxPDO"][it->first]["type"].value<std::string>());
            m_config[1].RxPDO.variables[static_cast<std::string>(it->first)].offset = *(data["ZeroErr_PDO"]["RxPDO"][it->first]["offset"].value<int>());
        }
        const auto& ZeroErrTxPDOData = data["ZeroErr_PDO"]["TxPDO"].as_table();
        for (auto it = ZeroErrTxPDOData->begin(); it != ZeroErrTxPDOData->end(); it++) {
            m_config[1].TxPDO.variables[static_cast<std::string>(it->first)].type = *(data["ZeroErr_PDO"]["TxPDO"][it->first]["type"].value<std::string>());
            m_config[1].TxPDO.variables[static_cast<std::string>(it->first)].offset = *(data["ZeroErr_PDO"]["TxPDO"][it->first]["offset"].value<int>());
        }
        LOG(INFO) << "ZeroErr RxPDO are as follow: ";
        for (const auto& item : m_config[1].RxPDO.variables) {
            LOG(INFO) << item.first << ", " << item.second.type << ", " << item.second.offset;
        }
        LOG(INFO) << "MAXON TxPDO are as follow: ";
        for (const auto& item : m_config[1].TxPDO.variables) {
            LOG(INFO) << item.first << ", " << item.second.type << ", " << item.second.offset;
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

        const auto& KunweiTxPDOData = data["Kunwei_PDO"]["TxPDO"].as_table();
        for (auto it = KunweiTxPDOData->begin(); it != KunweiTxPDOData->end(); it++) {
            m_config[3].TxPDO.variables[static_cast<std::string>(it->first)].type = *(data["Kunwei_PDO"]["TxPDO"][it->first]["type"].value<std::string>());
            m_config[3].TxPDO.variables[static_cast<std::string>(it->first)].offset = *(data["Kunwei_PDO"]["TxPDO"][it->first]["offset"].value<int>());
        }
        const auto& KunweiRxPDOData = data["Kunwei_PDO"]["RxPDO"].as_table();
        for (auto it = KunweiRxPDOData->begin(); it != KunweiRxPDOData->end(); it++) {
            m_config[3].RxPDO.variables[static_cast<std::string>(it->first)].type = *(data["Kunwei_PDO"]["RxPDO"][it->first]["type"].value<std::string>());
            m_config[3].RxPDO.variables[static_cast<std::string>(it->first)].offset = *(data["Kunwei_PDO"]["RxPDO"][it->first]["offset"].value<int>());
        }
        LOG(INFO) << "Kunwei RxPDO are as follow: ";
        for (const auto& item : m_config[3].RxPDO.variables) {
            LOG(INFO) << item.first << ", " << item.second.type << ", " << item.second.offset;
        }
        LOG(INFO) << "Kunwei TxPDO are as follow: ";
        for (const auto& item : m_config[3].TxPDO.variables) {
            LOG(INFO) << item.first << ", " << item.second.type << ", " << item.second.offset;
        }

        const auto& ATITxPDOData = data["ATI_PDO"]["TxPDO"].as_table();
        for (auto it = ATITxPDOData->begin(); it != ATITxPDOData->end(); it++) {
            m_config[4].TxPDO.variables[static_cast<std::string>(it->first)].type = *(data["ATI_PDO"]["TxPDO"][it->first]["type"].value<std::string>());
            m_config[4].TxPDO.variables[static_cast<std::string>(it->first)].offset = *(data["ATI_PDO"]["TxPDO"][it->first]["offset"].value<int>());
        }
        const auto& ATIRxPDOData = data["ATI_PDO"]["RxPDO"].as_table();
        for (auto it = ATIRxPDOData->begin(); it != ATIRxPDOData->end(); it++) {
            m_config[4].RxPDO.variables[static_cast<std::string>(it->first)].type = *(data["ATI_PDO"]["RxPDO"][it->first]["type"].value<std::string>());
            m_config[4].RxPDO.variables[static_cast<std::string>(it->first)].offset = *(data["ATI_PDO"]["RxPDO"][it->first]["offset"].value<int>());
        }
        LOG(INFO) << "ATI RxPDO are as follow: ";
        for (const auto& item : m_config[4].RxPDO.variables) {
            LOG(INFO) << item.first << ", " << item.second.type << ", " << item.second.offset;
        }
        LOG(INFO) << "ATI TxPDO are as follow: ";
        for (const auto& item : m_config[4].TxPDO.variables) {
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

uint16_t MotorDriver::getErrorCode(const MotorType& type, const int& index, const int& armNum){

    if(m_config[static_cast<int>(type)].TxPDO.variables.count(ERRCODE) > 0){
        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[ERRCODE];
        switch(type){
            case MotorType::MOONS:{
                const auto errorCode = hex2Uint16(m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                           + variable.offset
                                                           + armNum * m_abRecvDataPerArm
                                                           + m_abRecvDataGuiding],
                                               m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                           + variable.offset
                                                           + armNum * m_abRecvDataPerArm
                                                           + m_abRecvDataGuiding + 1]);
                return errorCode;
            }
            case MotorType::ZERO_ERR:{
                if(armNum == -1){
                    const auto errorCode = hex2Uint16(m_abRecvData[guidingJointMotor_sizeRecvData * index + variable.offset],
                                                  m_abRecvData[guidingJointMotor_sizeRecvData * index + variable.offset + 1]);
                    return errorCode;
                }else{
                    const auto errorCode = hex2Uint16(m_abRecvData[m_abRecvDataGuiding
                                                               + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                               + endJointMotor_sizeRecvData * index
                                                               + armNum * m_abRecvDataPerArm
                                                               + variable.offset],
                                                  m_abRecvData[m_abRecvDataGuiding
                                                               + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                               + endJointMotor_sizeRecvData * index
                                                               + armNum * m_abRecvDataPerArm
                                                               + variable.offset + 1]);
                    return errorCode;
                }
            }
            case MotorType::MAXON:{
                const auto errorCode = hex2Uint16(m_abRecvData[m_abRecvDataGuiding
                                                           + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                           + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                           + endInstrumentMotor_sizeRecvData * index
                                                           + armNum * m_abRecvDataPerArm
                                                           + variable.offset],
                                              m_abRecvData[m_abRecvDataGuiding
                                                           + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                           + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                           + endInstrumentMotor_sizeRecvData * index
                                                           + armNum * m_abRecvDataPerArm
                                                           + variable.offset + 1]);
                return errorCode;
            }
            default:{
                LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
                return T_ERROR;
            }
        }
    }
    else{
        LOG(ERROR) << "Error: there is no ErrorCode in TxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}

uint16_t MotorDriver::getStatusWord(const MotorType& type, const int& index, const int& armNum){

    if(m_config[static_cast<int>(type)].TxPDO.variables.count(STATUSWORD) > 0){
        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[STATUSWORD];
        switch(type){
            case MotorType::MOONS:{
                const auto statusWord = hex2Uint16(m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                            + variable.offset
                                                            + armNum * m_abRecvDataPerArm
                                                            + m_abRecvDataGuiding],
                                                    m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                            + variable.offset
                                                            + armNum * m_abRecvDataPerArm
                                                            + m_abRecvDataGuiding + 1]);return statusWord;
            }
            case MotorType::ZERO_ERR:{
                if(armNum == -1){
                    const auto statusWord = hex2Uint16(m_abRecvData[guidingJointMotor_sizeRecvData * index + variable.offset],
                                                       m_abRecvData[guidingJointMotor_sizeRecvData * index + variable.offset + 1]);
                    return statusWord;
                }else{
                    const auto statusWord = hex2Uint16(m_abRecvData[m_abRecvDataGuiding
                                                                    + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                                    + endJointMotor_sizeRecvData * index
                                                                    + armNum * m_abRecvDataPerArm
                                                                    + variable.offset],
                                                       m_abRecvData[m_abRecvDataGuiding
                                                                    + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                                    + endJointMotor_sizeRecvData * index
                                                                    + armNum * m_abRecvDataPerArm
                                                                    + variable.offset + 1]);
                    return statusWord;
                }
            }
            case MotorType::MAXON:{
                const auto statusWord = hex2Uint16(m_abRecvData[m_abRecvDataGuiding
                                                                + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                                + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                                + endInstrumentMotor_sizeRecvData * index
                                                                + armNum * m_abRecvDataPerArm
                                                                + variable.offset],
                                                   m_abRecvData[m_abRecvDataGuiding
                                                                + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                                + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                                + endInstrumentMotor_sizeRecvData * index
                                                                + armNum * m_abRecvDataPerArm
                                                                + variable.offset + 1]);
                return statusWord;
            }
            default:{
                LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
                return T_ERROR;
            }
        }
    }
    else{
        LOG(ERROR) << "Error: There is no StatusWord in TxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}


int16_t MotorDriver::getOperationMode(const MotorType& type, const int& index,const int& armNum){
    if(m_config[static_cast<int>(type)].TxPDO.variables.count(MODEDISPLAY) > 0){
        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[MODEDISPLAY];
        switch(type)
        {
            case MotorType::ZERO_ERR:
            {
                if(armNum == -1){
                    const auto operationMode = hex2Int8(m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                                     + variable.offset]);
                    return operationMode;
                }else{
                    const auto operationMode = hex2Int8(m_abRecvData[m_abRecvDataGuiding + armNum * m_abRecvDataPerArm
                                                                     + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                                     + endJointMotor_sizeRecvData * index
                                                                     + variable.offset]);
                    return operationMode;
                }
            }
            case MotorType::MOONS:
            {
                const auto operationMode = hex2Int8(m_abRecvData[m_abRecvDataGuiding + armNum * m_abRecvDataPerArm
                                                                 + endGimbalMotor_sizeRecvData * index
                                                                 + variable.offset]);
                return operationMode;
            }
            case MotorType::MAXON:
            {
                const auto operationMode = hex2Int8(m_abRecvData[m_abRecvDataGuiding + armNum * m_abRecvDataPerArm
                                                                 + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                                 + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                                 + endInstrumentMotor_sizeRecvData * index
                                                                 + variable.offset]);
                return operationMode;
            }
            default:{
                LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
                return T_ERROR;
            }
        }
    }
    else{
        LOG(ERROR) << "Error: There is no ActualPosition in TxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}

int32_t MotorDriver::getActualPos(const MotorType& type, const int& index, const int& armNum){
    if(m_config[static_cast<int>(type)].TxPDO.variables.count(ACTPOS) > 0){

        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[ACTPOS];
        switch(type){
        case MotorType::MOONS:{
            const auto actualPos = hex2Int32(m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                          + variable.offset
                                                          + armNum * m_abRecvDataPerArm
                                                          + m_abRecvDataGuiding],
                                             m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                          + variable.offset
                                                          + armNum * m_abRecvDataPerArm
                                                          + m_abRecvDataGuiding + 1],
                                             m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                          + variable.offset
                                                          + armNum * m_abRecvDataPerArm
                                                          + m_abRecvDataGuiding + 2],
                                             m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                          + variable.offset
                                                          + armNum * m_abRecvDataPerArm
                                                          + m_abRecvDataGuiding + 3]);
            return actualPos;

        }
        case MotorType::ZERO_ERR:{
            if(armNum == -1){
                const auto actualPos = hex2Int32(m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                              + variable.offset],
                                                 m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                              + variable.offset + 1],
                                                 m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                              + variable.offset + 2],
                                                 m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                              + variable.offset + 3]);
                return actualPos;
            }else{
                const auto actualPos = hex2Int32(m_abRecvData[m_abRecvDataGuiding
                                                              + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                              + endJointMotor_sizeRecvData * index
                                                              + armNum * m_abRecvDataPerArm
                                                              + variable.offset],
                                                 m_abRecvData[m_abRecvDataGuiding
                                                              + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                              + endJointMotor_sizeRecvData * index
                                                              + armNum * m_abRecvDataPerArm
                                                              + variable.offset + 1],
                                                 m_abRecvData[m_abRecvDataGuiding
                                                              + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                              + endJointMotor_sizeRecvData * index
                                                              + armNum * m_abRecvDataPerArm
                                                              + variable.offset + 2],
                                                 m_abRecvData[m_abRecvDataGuiding
                                                              + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                              + endJointMotor_sizeRecvData * index
                                                              + armNum * m_abRecvDataPerArm
                                                              + variable.offset + 3]);
                return actualPos;
            }
        }
        case MotorType::MAXON:{
            const auto actualPos = hex2Int32(m_abRecvData[m_abRecvDataGuiding
                                                          + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                          + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                          + endInstrumentMotor_sizeRecvData * index
                                                          + armNum * m_abRecvDataPerArm
                                                          + variable.offset],
                                             m_abRecvData[m_abRecvDataGuiding
                                                          + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                          + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                          + endInstrumentMotor_sizeRecvData * index
                                                          + armNum * m_abRecvDataPerArm
                                                          + variable.offset + 1],
                                             m_abRecvData[m_abRecvDataGuiding
                                                          + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                          + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                          + endInstrumentMotor_sizeRecvData * index
                                                          + armNum * m_abRecvDataPerArm
                                                          + variable.offset + 2],
                                             m_abRecvData[m_abRecvDataGuiding
                                                          + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                          + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                          + endInstrumentMotor_sizeRecvData * index
                                                          + armNum * m_abRecvDataPerArm
                                                          + variable.offset + 3]);

            return actualPos;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
        }
    }
    else{
        LOG(ERROR) << "Error: There is no ActualPosition in TxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}


int32_t MotorDriver::getActualVel(const MotorType& type, const int& index, const int& armNum){
    if(m_config[static_cast<int>(type)].TxPDO.variables.count(ACTVEL) > 0){
        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[ACTVEL];
        switch(type){
        case MotorType::MOONS:{
            const auto actualVel = hex2Int32(m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                          + variable.offset
                                                          + armNum * m_abRecvDataPerArm
                                                          + m_abRecvDataGuiding],
                                             m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                          + variable.offset
                                                          + armNum * m_abRecvDataPerArm
                                                          + m_abRecvDataGuiding + 1],
                                             m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                          + variable.offset
                                                          + armNum * m_abRecvDataPerArm
                                                          + m_abRecvDataGuiding + 2],
                                             m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                          + variable.offset
                                                          + armNum * m_abRecvDataPerArm
                                                          + m_abRecvDataGuiding + 3]);
            return actualVel;

        }
        case MotorType::ZERO_ERR:{
            if(armNum == -1){
                const auto actualVel = hex2Int32(m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                              + variable.offset],
                                                 m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                              + variable.offset + 1],
                                                 m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                              + variable.offset + 2],
                                                 m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                              + variable.offset + 3]);
                return actualVel;
            }else{
                const auto actualVel = hex2Int32(m_abRecvData[m_abRecvDataGuiding
                                                              + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                              + endJointMotor_sizeRecvData * index
                                                              + armNum * m_abRecvDataPerArm
                                                              + variable.offset],
                                                 m_abRecvData[m_abRecvDataGuiding
                                                              + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                              + endJointMotor_sizeRecvData * index
                                                              + armNum * m_abRecvDataPerArm
                                                              + variable.offset + 1],
                                                 m_abRecvData[m_abRecvDataGuiding
                                                              + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                              + endJointMotor_sizeRecvData * index
                                                              + armNum * m_abRecvDataPerArm
                                                              + variable.offset + 2],
                                                 m_abRecvData[m_abRecvDataGuiding
                                                              + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                              + endJointMotor_sizeRecvData * index
                                                              + armNum * m_abRecvDataPerArm
                                                              + variable.offset + 3]);
                return actualVel;
            }
        }
        case MotorType::MAXON:{
            const auto actualVel = hex2Int32(m_abRecvData[m_abRecvDataGuiding
                                                          + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                          + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                          + endInstrumentMotor_sizeRecvData * index
                                                          + armNum * m_abRecvDataPerArm
                                                          + variable.offset],
                                             m_abRecvData[m_abRecvDataGuiding
                                                          + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                          + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                          + endInstrumentMotor_sizeRecvData * index
                                                          + armNum * m_abRecvDataPerArm
                                                          + variable.offset + 1],
                                             m_abRecvData[m_abRecvDataGuiding
                                                          + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                          + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                          + endInstrumentMotor_sizeRecvData * index
                                                          + armNum * m_abRecvDataPerArm
                                                          + variable.offset + 2],
                                             m_abRecvData[m_abRecvDataGuiding
                                                          + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                          + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                          + endInstrumentMotor_sizeRecvData * index
                                                          + armNum * m_abRecvDataPerArm
                                                          + variable.offset + 3]);

            return actualVel;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
        }
    }
    else{
        LOG(ERROR) << "Error: there is no ActualVelocity in TxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}


//int16_t MotorDriver::getActualTrq(const MotorType& type, const int& index){
//    if(m_config[static_cast<int>(type)].TxPDO.variables.count(ACTTRQ) > 0){
//        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[ACTTRQ];
//        const auto actualTrq = hex2Int16(m_abRecvData[moons_sizeRecvData * m_endGimbalMotorNum + variable.offset + zeroErr_sizeRecvData * index],
//                                         m_abRecvData[moons_sizeRecvData * m_endGimbalMotorNum + variable.offset + zeroErr_sizeRecvData * index + 1]);
//        return actualTrq;
//    }
//    else{
//        LOG(ERROR) << "Error: there is no ActualTorque in TxPDO mapping under this motor type, please check PDO setting!";
//        return T_ERROR;
//    }
//}

int16_t MotorDriver::getActualTrq(const MotorType& type, const int& index, const int& armNum){//获取现在的扭矩
    if(m_config[static_cast<int>(type)].TxPDO.variables.count(ACTTRQ) > 0){
        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[ACTTRQ];
        switch(type){
        case MotorType::MOONS:{
            const auto actualTrq = hex2Int16(m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                          + variable.offset
                                                          + armNum * m_abRecvDataPerArm
                                                          + m_abRecvDataGuiding],
                                             m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                          + variable.offset
                                                          + armNum * m_abRecvDataPerArm
                                                          + m_abRecvDataGuiding + 1]);
            return actualTrq;
        }

        case MotorType::ZERO_ERR:{
            if(armNum == -1){
                const auto actualTrq = hex2Int16(m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                              + variable.offset],
                                                 m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                              + variable.offset + 1]);
                return actualTrq;
            }else{
                const auto actualTrq = hex2Int16(m_abRecvData[m_abRecvDataGuiding
                                                              + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                              + endJointMotor_sizeRecvData * index
                                                              + armNum * m_abRecvDataPerArm
                                                              + variable.offset],
                                                 m_abRecvData[m_abRecvDataGuiding
                                                              + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                              + endJointMotor_sizeRecvData * index
                                                              + armNum * m_abRecvDataPerArm
                                                              + variable.offset + 1]);
                return actualTrq;
            }
        }

        case MotorType::MAXON:{
            const auto actualTrq = hex2Int16(m_abRecvData[m_abRecvDataGuiding
                                                          + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                          + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                          + endInstrumentMotor_sizeRecvData * index
                                                          + armNum * m_abRecvDataPerArm
                                                          + variable.offset],
                                             m_abRecvData[m_abRecvDataGuiding
                                                          + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                          + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                          + endInstrumentMotor_sizeRecvData * index
                                                          + armNum * m_abRecvDataPerArm
                                                          + variable.offset + 1]);
            return actualTrq;
        }

        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
        }

    }
    else{
        LOG(ERROR) << "Error: there is no ActualTrq in TxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}


//int16_t MotorDriver::getActualCur(const MotorType& type, const int& index){
//    if(m_config[static_cast<int>(type)].TxPDO.variables.count(ACTCUR) > 0){
//        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[ACTCUR];
//        const auto actualCur = hex2Int16(m_abRecvData[moons_sizeRecvData * m_endGimbalMotorNum + variable.offset + zeroErr_sizeRecvData * index],
//                                         m_abRecvData[moons_sizeRecvData * m_endGimbalMotorNum + variable.offset + zeroErr_sizeRecvData * index + 1]);
//        return actualCur;
//    }
//    else{
//        LOG(ERROR) << "Error: there is no ActualCurrent in TxPDO mapping under this motor type, please check PDO setting!";
//        return T_ERROR;
//    }
//}

int16_t MotorDriver::getActualCur(const MotorType& type, const int& index, const int& armNum){//获取现在的电流
    if(m_config[static_cast<int>(type)].TxPDO.variables.count(ACTCUR) > 0){
        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[ACTCUR];
        switch(type){
        case MotorType::MOONS:{
            const auto actualCur = hex2Int16(m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                          + variable.offset
                                                          + armNum * m_abRecvDataPerArm
                                                          + m_abRecvDataGuiding],
                                             m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                          + variable.offset
                                                          + armNum * m_abRecvDataPerArm
                                                          + m_abRecvDataGuiding + 1]);
            return actualCur;
        }
        case MotorType::MAXON:{
            const auto actualCur = hex2Int16(m_abRecvData[m_abRecvDataGuiding
                                                          + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                          + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                          + endInstrumentMotor_sizeRecvData * index
                                                          + armNum * m_abRecvDataPerArm
                                                          + variable.offset],
                                             m_abRecvData[m_abRecvDataGuiding
                                                          + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                          + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                          + endInstrumentMotor_sizeRecvData * index
                                                          + armNum * m_abRecvDataPerArm
                                                          + variable.offset + 1]);
            return actualCur;
        }
        case MotorType::ZERO_ERR:{
            if(armNum == -1){
                const auto actualCur = hex2Int16(m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                              + variable.offset],
                                                 m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                              + variable.offset + 1]);
                return actualCur;
            }else{
                const auto actualCur = hex2Int16(m_abRecvData[m_abRecvDataGuiding
                                                              + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                              + endJointMotor_sizeRecvData * index
                                                              + armNum * m_abRecvDataPerArm
                                                              + variable.offset],
                                                 m_abRecvData[m_abRecvDataGuiding
                                                              + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                              + endJointMotor_sizeRecvData * index
                                                              + armNum * m_abRecvDataPerArm
                                                              + variable.offset + 1]);
                return actualCur;
            }
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
        }

    }
    else{
        LOG(ERROR) << "Error: there is no ActualCurrent in TxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}

std::array<int, 8> MotorDriver::getDigitalInputs(const MotorType& type, const int& index, const int& armNum){
    DigitalInput tmpVal{0};
    if(m_config[static_cast<int>(type)].TxPDO.variables.count(DIGITALINPUT) > 0){
        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[DIGITALINPUT];
        switch(type){
        case MotorType::MOONS:{
            const auto digitalInputs = hex2Uint32(m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                               + variable.offset
                                                               + armNum * m_abRecvDataPerArm
                                                               + m_abRecvDataGuiding],
                                                  m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                               + variable.offset
                                                               + armNum * m_abRecvDataPerArm
                                                               + m_abRecvDataGuiding + 1],
                                                  m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                               + variable.offset
                                                               + armNum * m_abRecvDataPerArm
                                                               + m_abRecvDataGuiding + 2],
                                                  m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                               + variable.offset
                                                               + armNum * m_abRecvDataPerArm
                                                               + m_abRecvDataGuiding + 3]);

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
        case MotorType::ZERO_ERR:{
            const auto digitalInputs = hex2Uint32(m_abRecvData[moons_sizeRecvData * m_motorNum + zeroErr_sizeRecvData * index + variable.offset],
                                                  m_abRecvData[moons_sizeRecvData * m_motorNum + zeroErr_sizeRecvData * index + variable.offset + 1],
                                                  m_abRecvData[moons_sizeRecvData * m_motorNum + zeroErr_sizeRecvData * index + variable.offset + 2],
                                                  m_abRecvData[moons_sizeRecvData * m_motorNum + zeroErr_sizeRecvData * index + variable.offset + 3]);

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
        case MotorType::MAXON:{
            const auto digitalInputs = hex2Uint32(m_abRecvData[m_abRecvDataGuiding
                                                               + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                               + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                               + endInstrumentMotor_sizeRecvData * index
                                                               + armNum * m_abRecvDataPerArm
                                                               + variable.offset],
                                                  m_abRecvData[m_abRecvDataGuiding
                                                               + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                               + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                               + endInstrumentMotor_sizeRecvData * index
                                                               + armNum * m_abRecvDataPerArm
                                                               + variable.offset + 1],
                                                  m_abRecvData[m_abRecvDataGuiding
                                                               + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                               + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                               + endInstrumentMotor_sizeRecvData * index
                                                               + armNum * m_abRecvDataPerArm
                                                               + variable.offset + 2],
                                                  m_abRecvData[m_abRecvDataGuiding
                                                               + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                               + endJointMotor_sizeRecvData * m_endJointMotorNumPerArm
                                                               + endInstrumentMotor_sizeRecvData * index
                                                               + armNum * m_abRecvDataPerArm
                                                               + variable.offset + 3]);

            tmpVal[0] = (digitalInputs >> 0) & 0x1;
            tmpVal[1] = (digitalInputs >> 1) & 0x1;
            tmpVal[2] = (digitalInputs >> 2) & 0x1;
            tmpVal[3] = (digitalInputs >> 3) & 0x1;
            tmpVal[4] = (digitalInputs >> 4) & 0x1;
            tmpVal[5] = (digitalInputs >> 5) & 0x1;
            tmpVal[6] = (digitalInputs >> 6) & 0x1;
            tmpVal[7] = (digitalInputs >> 7) & 0x1;

            return tmpVal;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return tmpVal;
        }
        }
    }
    else{
        LOG(ERROR) << "Error: There is no DigitalInput in TxPDO mapping under this motor type, please check PDO setting!";
        return tmpVal;
    }
}

int32_t MotorDriver::getFollowingPosErr(const MotorType& type, const int& index, const int& armNum){
    if(m_config[static_cast<int>(type)].TxPDO.variables.count(FOLLOWINGPOSERR) > 0){
        const auto& variable = m_config[static_cast<int>(type)].TxPDO.variables[FOLLOWINGPOSERR];
        if(type == MotorType::ZERO_ERR){
            if(armNum == -1){
                const auto followingPosErr = hex2Int32(m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                                    + variable.offset],
                                                       m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                                    + variable.offset + 1],
                                                       m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                                    + variable.offset + 2],
                                                       m_abRecvData[guidingJointMotor_sizeRecvData * index
                                                                    + variable.offset + 3]);
                return followingPosErr;
            }else{
                const auto followingPosErr = hex2Int32(m_abRecvData[m_abRecvDataGuiding
                                                                    + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                                    + endJointMotor_sizeRecvData * index
                                                                    + armNum * m_abRecvDataPerArm
                                                                    + variable.offset],
                                                       m_abRecvData[m_abRecvDataGuiding
                                                                    + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                                    + endJointMotor_sizeRecvData * index
                                                                    + armNum * m_abRecvDataPerArm
                                                                    + variable.offset + 1],
                                                       m_abRecvData[m_abRecvDataGuiding
                                                                    + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                                    + endJointMotor_sizeRecvData * index
                                                                    + armNum * m_abRecvDataPerArm
                                                                    + variable.offset + 2],
                                                       m_abRecvData[m_abRecvDataGuiding
                                                                    + endGimbalMotor_sizeRecvData * m_endGimbalMotorNumPerArm
                                                                    + endJointMotor_sizeRecvData * index
                                                                    + armNum * m_abRecvDataPerArm
                                                                    + variable.offset + 3]);
                return followingPosErr;
            }
        }
        else if(type == MotorType::MOONS){
            const auto followingPosErr = hex2Int32(m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                                + variable.offset
                                                                + armNum * m_abRecvDataPerArm
                                                                + m_abRecvDataGuiding],
                                                   m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                                + variable.offset
                                                                + armNum * m_abRecvDataPerArm
                                                                + m_abRecvDataGuiding + 1],
                                                   m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                                + variable.offset
                                                                + armNum * m_abRecvDataPerArm
                                                                + m_abRecvDataGuiding + 2],
                                                   m_abRecvData[endGimbalMotor_sizeRecvData * index
                                                                + variable.offset
                                                                + armNum * m_abRecvDataPerArm
                                                                + m_abRecvDataGuiding + 3]);
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

int MotorDriver::setControlWord(const MotorType& type, const int& index, const ControlCommand& cmd, const int& armNum){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(CONTROLWORD) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[CONTROLWORD];
        if(type == MotorType::ZERO_ERR){
            if(armNum == -1){
                m_abSendData[guidingJointMotor_sizeSendData * index
                             + variable.offset] = static_cast<unsigned char>(cmd);
            }else{
                m_abSendData[m_abSendDataGuiding
                             + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                             + endJointMotor_sizeSendData * index
                             + armNum * m_abSendDataPerArm
                             + variable.offset] = static_cast<unsigned char>(cmd);
            }
            return T_NOERROR;
        }
        else if(type == MotorType::MOONS){
            m_abSendData[m_abSendDataGuiding
                         + endGimbalMotor_sizeSendData * index
                         + armNum * m_abSendDataPerArm
                         + variable.offset] = static_cast<unsigned char>(cmd);
            return T_NOERROR;
        }
        else if(type == MotorType::MAXON){
            m_abRecvData[m_abSendDataGuiding
                         + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                         + endJointMotor_sizeSendData * m_endJointMotorNumPerArm
                         + endInstrumentMotor_sizeSendData * index
                         + armNum * m_abSendDataPerArm
                         + variable.offset] = static_cast<unsigned char>(cmd);
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

int MotorDriver::setOperationMode(const MotorType& type, const int& index, const OperationMode& mode, const int& armNum){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(OPMODE) > 0){

        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[OPMODE];
        if(type == MotorType::ZERO_ERR){
            if(armNum == -1){
                m_abSendData[guidingJointMotor_sizeSendData * index
                             + variable.offset] = static_cast<unsigned char>(mode);
            }else{
                m_abSendData[m_abSendDataGuiding
                             + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                             + endJointMotor_sizeSendData * index
                             + armNum * m_abSendDataPerArm
                             + variable.offset] = static_cast<unsigned char>(mode);
            }
            return T_NOERROR;
        }
        else if(type == MotorType::MOONS){
            m_abSendData[m_abSendDataGuiding
                         + endGimbalMotor_sizeSendData * index
                         + armNum * m_abSendDataPerArm
                         + variable.offset] = static_cast<unsigned char>(mode);
            return T_NOERROR;
        }
        else if(type == MotorType::MAXON){
            m_abRecvData[m_abSendDataGuiding
                         + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                         + endJointMotor_sizeSendData * m_endJointMotorNumPerArm
                         + endInstrumentMotor_sizeSendData * index
                         + armNum * m_abSendDataPerArm
                         + variable.offset] = static_cast<unsigned char>(mode);
            return T_NOERROR;
        }
        else{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
    }
    else{
        LOG(ERROR) << "Error: There is no OperationMode in RxPDO mapping under this motor type, please check PDO setting!";
        return T_ERROR;
    }
}


int MotorDriver::setTargetPos(const MotorType& type, const int& index, const int32_t& targetPos, const int& armNum){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(TARGETPOS) > 0){

        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[TARGETPOS];
        if(type == MotorType::ZERO_ERR){
            if(armNum == -1){
                int32ToBytes(targetPos,
                             m_abSendData
                                 + guidingJointMotor_sizeSendData * index
                                 + variable.offset);
            }else{
                int32ToBytes(targetPos,
                             m_abSendData
                                 + m_abSendDataGuiding
                                 + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                                 + endJointMotor_sizeSendData * index
                                 + armNum * m_abSendDataPerArm
                                 + variable.offset);
            }
            return T_NOERROR;
        }
        else if(type == MotorType::MOONS){
            int32ToBytes(targetPos,
                         m_abSendData
                             + m_abSendDataGuiding
                             + endGimbalMotor_sizeSendData * index
                             + armNum * m_abSendDataPerArm
                             + variable.offset);
            return T_NOERROR;
        }
        else if(type == MotorType::MAXON)
        {
            int32ToBytes(targetPos,
                         m_abSendData
                             + m_abSendDataGuiding
                             + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                             + endJointMotor_sizeSendData * m_endJointMotorNumPerArm
                             + endInstrumentMotor_sizeSendData * index
                             + armNum * m_abSendDataPerArm
                             + variable.offset);
            return T_NOERROR;
        }
        else{
            LOG(ERROR) << "Error: The input motor type is undefined,  please check motor type!";
            return T_ERROR;
        }
    }
    else{
        LOG(ERROR) << "Error: there is no TargetPosition in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

int MotorDriver::setTargetVel(const MotorType& type, const int& index, const int32_t& targetVel, const int& armNum){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(TARGETVEL) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[TARGETVEL];
        if(type == MotorType::ZERO_ERR){
            if(armNum == -1){
                int32ToBytes(targetVel,
                             m_abSendData
                                 + guidingJointMotor_sizeSendData * index
                                 + variable.offset);
            }else{
                int32ToBytes(targetVel,
                             m_abSendData
                                 + m_abSendDataGuiding
                                 + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                                 + endJointMotor_sizeSendData * index
                                 + armNum * m_abSendDataPerArm
                                 + variable.offset);
            }
            return T_NOERROR;
        }
        else if(type == MotorType::MOONS){
            int32ToBytes(targetVel,
                         m_abSendData
                             + m_abSendDataGuiding
                             + endGimbalMotor_sizeSendData * index
                             + armNum * m_abSendDataPerArm
                             + variable.offset);
            return T_NOERROR;
        }
        else if(type == MotorType::MAXON)
        {
            int32ToBytes(targetVel,
                         m_abSendData
                             + m_abSendDataGuiding
                             + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                             + endJointMotor_sizeSendData * m_endJointMotorNumPerArm
                             + endInstrumentMotor_sizeSendData * index
                             + armNum * m_abSendDataPerArm
                             + variable.offset);
            return T_NOERROR;
        }
        else{
            LOG(ERROR) << "Error: The input motor type is undefined,  please check motor type!";
            return T_ERROR;
        }
    }
    else{
        LOG(ERROR) << "Error: there is no TargetVelocity in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}


int MotorDriver::setTargetTrq(const MotorType& type, const int& index, const int16_t& targetTrq, const int& armNum){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(TARGETTRQ) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[TARGETTRQ];
        if(type == MotorType::ZERO_ERR){
            if(armNum == -1){
                int16ToBytes(targetTrq,
                             m_abSendData
                                 + guidingJointMotor_sizeSendData * index
                                 + variable.offset);
            }else{
                int16ToBytes(targetTrq,
                             m_abSendData
                                 + m_abSendDataGuiding
                                 + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                                 + endJointMotor_sizeSendData * index
                                 + armNum * m_abSendDataPerArm
                                 + variable.offset);
            }
            return T_NOERROR;
        }
        else if(type == MotorType::MOONS){
            int16ToBytes(targetTrq,
                         m_abSendData
                             + m_abSendDataGuiding
                             + endGimbalMotor_sizeSendData * index
                             + armNum * m_abSendDataPerArm
                             + variable.offset);
            return T_NOERROR;
        }
        else if(type == MotorType::MAXON)
        {
            int16ToBytes(targetTrq,
                         m_abSendData
                             + m_abSendDataGuiding
                             + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                             + endJointMotor_sizeSendData * m_endJointMotorNumPerArm
                             + endInstrumentMotor_sizeSendData * index
                             + armNum * m_abSendDataPerArm
                             + variable.offset);
            return T_NOERROR;
        }
        else{
            LOG(ERROR) << "Error: The input motor type is undefined,  please check motor type!";
            return T_ERROR;
        }
    }
    else{
        LOG(ERROR) << "Error: there is no TargetTorque in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

// int MotorDriver::setVelOffset(const MotorType& type, const int& index, const int32_t& velOffset){
//     if(m_config[static_cast<int>(type)].RxPDO.variables.count(VELOFFSET) > 0){
//         const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[VELOFFSET];
//         int32ToBytes(velOffset, m_abSendData + endGimbal_sizeSendData * m_endGimbalMotorNum + variable.offset + endJoint_sizeSendData * index);
//         return T_NOERROR;
//     }
//     else{
//         LOG(ERROR) << "Error: there is no VelocityOffset in RxPDO mapping under this motor type, please check PDO setting!" ;
//         return T_ERROR;
//     }
// }


// int MotorDriver::setTrqOffset(const MotorType& type, const int& index, const int16_t& trqOffset){
//     if(m_config[static_cast<int>(type)].RxPDO.variables.count(TRQOFFSET) > 0){
//         const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[TRQOFFSET];
//         int16ToBytes(trqOffset, m_abSendData + endGimbal_sizeSendData * m_endGimbalMotorNum + variable.offset + endJoint_sizeSendData * index);
//         return T_NOERROR;
//     }
//     else{
//         LOG(ERROR) << "Error: there is no TorqueOffset in RxPDO mapping under this motor type, please check PDO setting!" ;
//         return T_ERROR;
//     }
// }

int MotorDriver::setDigitalOutputs(const MotorType& type, const int& index, const uint32_t& digitalOutputs, const int& armNum){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(DIGITALOUT) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[DIGITALOUT];
        switch(type){
        case MotorType::MOONS:{
            uint32ToBytes(digitalOutputs,
                          m_abSendData
                              + m_abSendDataGuiding
                              + endGimbalMotor_sizeSendData * index
                              + armNum * m_abSendDataPerArm
                              + variable.offset);
            return T_NOERROR;
        }
        case MotorType::ZERO_ERR:{
            if(armNum == -1){
                uint32ToBytes(digitalOutputs,
                              m_abSendData
                                  + guidingJointMotor_sizeSendData * index
                                  + variable.offset);
            }else{
                uint32ToBytes(digitalOutputs,
                              m_abSendData
                                  + m_abSendDataGuiding
                                  + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                                  + endJointMotor_sizeSendData * index
                                  + armNum * m_abSendDataPerArm
                                  + variable.offset);
            }
            return T_NOERROR;
        }
        case MotorType::MAXON:{
            uint32ToBytes(digitalOutputs,
                          m_abSendData
                              + m_abSendDataGuiding
                              + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                              + endJointMotor_sizeSendData * m_endJointMotorNumPerArm
                              + endInstrumentMotor_sizeSendData * index
                              + armNum * m_abSendDataPerArm
                              + variable.offset);
            return T_NOERROR;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
        }
    }
    else{
        LOG(ERROR) << "Error: There is no DigitialOutputs in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

int MotorDriver::setProfileVel(const MotorType& type, const int& index, const uint32_t& profileVel, const int& armNum){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(PROFILEVEL) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[PROFILEVEL];
        switch(type){
        case MotorType::MOONS:{
            uint32ToBytes(profileVel,
                          m_abSendData
                              + m_abSendDataGuiding
                              + endGimbalMotor_sizeSendData * index
                              + armNum * m_abSendDataPerArm
                              + variable.offset);
            return T_NOERROR;
        }
        case MotorType::ZERO_ERR:{
            if(armNum == -1){
                uint32ToBytes(profileVel,
                              m_abSendData
                                  + guidingJointMotor_sizeSendData * index
                                  + variable.offset);
            }else{
                uint32ToBytes(profileVel,
                              m_abSendData
                                  + m_abSendDataGuiding
                                  + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                                  + endJointMotor_sizeSendData * index
                                  + armNum * m_abSendDataPerArm
                                  + variable.offset);
            }
            return T_NOERROR;
        }
        case MotorType::MAXON:{
            LOG(WARNING) << "WARNING: Setting profile velocity is not supported in maxon motor!";
            return T_ERROR;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
        }
    }
    else{
        LOG(ERROR) << "Error: There is no ProfileVelocity in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}


int MotorDriver::setProfileAcc(const MotorType& type, const int& index, const uint32_t& profileAcc, const int& armNum){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(PROFILEACC) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[PROFILEACC];
        switch(type){
        case MotorType::MOONS:{
            uint32ToBytes(profileAcc,
                          m_abSendData
                              + m_abSendDataGuiding
                              + endGimbalMotor_sizeSendData * index
                              + armNum * m_abSendDataPerArm
                              + variable.offset);
            return T_NOERROR;
        }
        case MotorType::ZERO_ERR:{
            if(armNum == -1){
                uint32ToBytes(profileAcc,
                              m_abSendData
                                  + guidingJointMotor_sizeSendData * index
                                  + variable.offset);
            }else{
                uint32ToBytes(profileAcc,
                              m_abSendData
                                  + m_abSendDataGuiding
                                  + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                                  + endJointMotor_sizeSendData * index
                                  + armNum * m_abSendDataPerArm
                                  + variable.offset);
            }
            return T_NOERROR;
        }
        case MotorType::MAXON:{
            uint32ToBytes(profileAcc,
                          m_abSendData
                              + m_abSendDataGuiding
                              + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                              + endJointMotor_sizeSendData * m_endJointMotorNumPerArm
                              + endInstrumentMotor_sizeSendData * index
                              + armNum * m_abSendDataPerArm
                              + variable.offset);
            return T_NOERROR;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
        }
    }
    else{
        LOG(ERROR) << "Error: there is no ProfileAcceleration in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

int MotorDriver::setProfileDec(const MotorType& type, const int& index, const uint32_t& profileDec, const int& armNum){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(PROFILEDEC) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[PROFILEDEC];
        switch(type){
        case MotorType::MOONS:{
            uint32ToBytes(profileDec, &m_abSendData[
                                          m_abSendDataGuiding
                                          + endGimbalMotor_sizeSendData * index
                                          + armNum * m_abSendDataPerArm
                                          + variable.offset]);
            return T_NOERROR;
        }
        case MotorType::ZERO_ERR:{
            if(armNum == -1){
                uint32ToBytes(profileDec, &m_abSendData[
                                              guidingJointMotor_sizeSendData * index
                                              + variable.offset]);
            }else{
                uint32ToBytes(profileDec,
                              m_abSendData
                                  + m_abSendDataGuiding
                                  + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                                  + endJointMotor_sizeSendData * index
                                  + armNum * m_abSendDataPerArm
                                  + variable.offset);
            }
            return T_NOERROR;
        }
        case MotorType::MAXON:{
            uint32ToBytes(profileDec,
                          m_abSendData
                              + m_abSendDataGuiding
                              + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                              + endJointMotor_sizeSendData * m_endJointMotorNumPerArm
                              + endInstrumentMotor_sizeSendData * index
                              + armNum * m_abSendDataPerArm
                              + variable.offset);
            return T_NOERROR;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
        }
    }
    else{
        LOG(ERROR) << "Error: there is no ProfileDeceleration in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

int MotorDriver::setMaxProfileVel(const MotorType& type, const int& index, const uint32_t& maxProfileVel, const int& armNum){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(MAXPROFILEVEL) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[MAXPROFILEVEL];
        switch(type){
        case MotorType::MOONS:{
            LOG(WARNING) << "WARNING: Setting max profile velocity is not supported in moons motor!";
            return T_ERROR;
        }
        case MotorType::ZERO_ERR:{
            if(armNum == -1){
                uint32ToBytes(maxProfileVel,
                              m_abSendData
                                  + guidingJointMotor_sizeSendData * index
                                  + variable.offset);
            }else{
                uint32ToBytes(maxProfileVel,
                              m_abSendData
                                  + m_abSendDataGuiding
                                  + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                                  + endJointMotor_sizeSendData * index
                                  + armNum * m_abSendDataPerArm
                                  + variable.offset);
            }
            return T_NOERROR;
        }
        case MotorType::MAXON:{
            LOG(WARNING) << "WARNING: Setting max profile velocity is not supported in maxon motor!";
            return T_ERROR;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
        }
    }
    else{
        LOG(ERROR) << "Error: there is no max ProfileVelocity in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

int MotorDriver::setTrqPosLimit(const MotorType& type, const int& index, const uint16_t& trqPosLimit, const int& armNum){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(POSTRQLIMIT) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[POSTRQLIMIT];
        switch(type){
        case MotorType::MOONS:{
            LOG(WARNING) << "WARNING: Setting positive torque limit is not supported in moons motor!";
            return T_ERROR;
        }
        case MotorType::ZERO_ERR:{
            if(armNum == -1){
                uint16ToBytes(trqPosLimit,
                              m_abSendData
                                  + guidingJointMotor_sizeSendData * index
                                  + variable.offset);
            }else{
                uint16ToBytes(trqPosLimit,
                              m_abSendData
                                  + m_abSendDataGuiding
                                  + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                                  + endJointMotor_sizeSendData * index
                                  + armNum * m_abSendDataPerArm
                                  + variable.offset);
            }
            return T_NOERROR;
        }
        case MotorType::MAXON:{
            LOG(WARNING) << "WARNING: Setting positive torque limit is not supported in maxon motor!";
            return T_ERROR;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
        }
    }
    else{
        LOG(ERROR) << "Error: There is no positive TorqueLimit in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

int MotorDriver::setTrqNegLimit(const MotorType& type, const int& index, const uint16_t& trqNegLimit, const int& armNum){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(NEGTRQLIMIT) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[NEGTRQLIMIT];
        switch(type){
        case MotorType::MOONS:{
            LOG(WARNING) << "WARNING: Setting negative torque limit is not supported in moons motor!";
            return T_ERROR;
        }
        case MotorType::ZERO_ERR:{
            if(armNum == -1){
                uint16ToBytes(trqNegLimit,
                              m_abSendData
                                  + guidingJointMotor_sizeSendData * index
                                  + variable.offset);
            }else{
                uint16ToBytes(trqNegLimit,
                              m_abSendData
                                  + m_abSendDataGuiding
                                  + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                                  + endJointMotor_sizeSendData * index
                                  + armNum * m_abSendDataPerArm
                                  + variable.offset);
            }
            return T_NOERROR;
        }
        case MotorType::MAXON:{
            LOG(WARNING) << "WARNING: Setting positive torque limit is not supported in maxon motor!";
            return T_ERROR;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
        }
    }
    else{
        LOG(ERROR) << "Error: there is no negative TorqueLimit in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

int MotorDriver::setHomeMethod(const MotorType& type, const int& index, const int& homeMethod, const int& armNum){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(HOMEMETHOD) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[HOMEMETHOD];
        switch(type){
        case MotorType::MAXON:{
            m_abRecvData[m_abSendDataGuiding
                         + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                         + endJointMotor_sizeSendData * m_endJointMotorNumPerArm
                         + endInstrumentMotor_sizeSendData * index
                         + armNum * m_abSendDataPerArm
                         + variable.offset] = static_cast<unsigned char>(homeMethod);
            return T_NOERROR;
            LOG(INFO) << "successfully set HOMEMETHODE of Maxon " << index;
        }
        case MotorType::MOONS:{
            m_abSendData[m_abSendDataGuiding
                         + endGimbalMotor_sizeSendData * index
                         + armNum * m_abSendDataPerArm
                         + variable.offset] = static_cast<unsigned char>(homeMethod);
            return T_NOERROR;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_NOERROR;
        }
        }
    }
    else{
        LOG(ERROR) << "Error: there is no HomeMethod in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

/* MOONS */
int MotorDriver::setHomeOffset(const MotorType& type, const int& index, const int32_t& homeOffset, const int& armNum){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(HOMEOFFSET) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[HOMEOFFSET];
        int32ToBytes(homeOffset, &m_abSendData[m_abSendDataGuiding
                                               + endGimbalMotor_sizeSendData * index
                                               + armNum * m_abSendDataPerArm
                                               + variable.offset]);
        return T_NOERROR;
    }
    else{
        LOG(ERROR) << "Error: there is no HomingOffset in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

int MotorDriver::setHomeVel(const MotorType& type, const int& index, const int32_t& homeVel, const int& armNum){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(HOMEVEL) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[HOMEVEL];
        switch (type) {
        case MotorType::MAXON: {
            int32ToBytes(homeVel, &m_abSendData[m_abSendDataGuiding
                                                + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                                                + endJointMotor_sizeSendData * m_endJointMotorNumPerArm
                                                + endInstrumentMotor_sizeSendData * index
                                                + armNum * m_abSendDataPerArm
                                                + variable.offset]);
            return T_NOERROR;
        }
        case MotorType::MOONS: {
            int32ToBytes(homeVel, &m_abSendData[m_abSendDataGuiding
                                                + endGimbalMotor_sizeSendData * index
                                                + armNum * m_abSendDataPerArm
                                                + variable.offset]);
            return T_NOERROR;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
        }
    }
    else
    {
        LOG(ERROR) << "Error: there is no HomingSpeed in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

// int MotorDriver::setHomeAcc(const MotorType& type, const int& index, const int32_t& homeAcc){
//     if(m_config[static_cast<int>(type)].RxPDO.variables.count(HOMEACC) > 0){
//         const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[HOMEACC];
//         int32ToBytes(homeAcc, &m_abSendData[variable.offset + endGimbal_sizeSendData * index]);
//         return T_NOERROR;
//     }
//     else{
//         LOG(ERROR) << "Error: there is no HomingAcc in RxPDO mapping under this motor type, please check PDO setting!" ;
//         return T_ERROR;
//     }
// }

int MotorDriver::setInterpolationTime(const MotorType& type, const int& index, const int& interpolationTime, const int& armNum){
    if(m_config[static_cast<int>(type)].RxPDO.variables.count(INTERPOLATIONTIME) > 0){
        const auto& variable = m_config[static_cast<int>(type)].RxPDO.variables[INTERPOLATIONTIME];
        switch(type){
        case MotorType::MOONS:{
            LOG(WARNING) << "WARNING: Setting interpolation time is not supported in moons motor!";
            return T_ERROR;
        }
        case MotorType::ZERO_ERR:{
            LOG(WARNING) << "WARNING: Setting interpolation time is not supported in zeroerr motor!";
            return T_ERROR;
        }
        case MotorType::MAXON:{
            m_abSendData[m_abSendDataGuiding
                         + endGimbalMotor_sizeSendData * m_endGimbalMotorNumPerArm
                         + endJointMotor_sizeSendData * m_endJointMotorNumPerArm
                         + endInstrumentMotor_sizeSendData * index
                         + armNum * m_abSendDataPerArm
                         + variable.offset] = static_cast<unsigned char>(interpolationTime);
            return T_NOERROR;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            return T_ERROR;
        }
        }
    }
    else{
        LOG(ERROR) << "Error: there is no Interpolation time in RxPDO mapping under this motor type, please check PDO setting!" ;
        return T_ERROR;
    }
}

// int MotorDriver::setBias(const int& controlWord){
//     const auto& variable = m_config[3].RxPDO.variables[CONTROLWORD_1];
//     int32ToBytes(controlWord, m_abSendData + endGimbal_sizeSendData * m_motorNum + endJoint_sizeSendData * m_endJointMotorNum + maxon_sizeSendData * m_endMotorNum + variable.offset);
//     return T_NOERROR;
// }


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


int MotorDriver::setMaxPosErr(const MotorType& type, const int& jointIndex, const SDO_COMMAND& sdoCmd){

    if(type == MotorType::MOONS || type == MotorType::MAXON){
        LOG(WARNING) << "WARNING: Setting max position error is not supported for moons or maxon motor!";
        return T_ERROR;
    }

    int preIdx = 1;
    //set tHeader information.
    m_tSendPkt.tHeader.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
    m_tSendPkt.tHeader.ulCmd = static_cast<uint32_t>(UlCmd::ECM_IF_CMD_COE_SDO_DOWNLOAD_REQ);
    m_tSendPkt.tHeader.ulLen = lengthSDODataHeader + sizeof(uint32_t);  // which is 18+4
    m_tSendPkt.tHeader.ulId = PROCESSID;

    // Motor Address
    m_tSendPkt.abData[0] = preIdx + jointIndex;
    m_tSendPkt.abData[1] = 0x01;
    // Transport Type, 0: CoE, 1: AoE
    m_tSendPkt.abData[2] = 0x00;
    m_tSendPkt.abData[3] = 0x00;
    // usAOEPort, only used if AOE transport
    m_tSendPkt.abData[4] = 0x00;
    m_tSendPkt.abData[5] = 0x00;
    // object Idx, SDO value
    m_tSendPkt.abData[6] = static_cast<short int>(MotorServoObjectIndex::POS_FOLLOWING_ERR_WINDOW) & 0xFF;
    m_tSendPkt.abData[7] = static_cast<short int>(MotorServoObjectIndex::POS_FOLLOWING_ERR_WINDOW) >> 8 & 0xFF;
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

    // data needs to be written
    m_tSendPkt.abData[18] = static_cast<uint32_t>(sdoCmd) & 0xFF;
    m_tSendPkt.abData[19] = static_cast<uint32_t>(sdoCmd) >> 8 & 0xFF;
    m_tSendPkt.abData[20] = static_cast<uint32_t>(sdoCmd) >> 16 & 0xFF;
    m_tSendPkt.abData[21] = static_cast<uint32_t>(sdoCmd) >> 24 & 0xFF;

    std::lock_guard<std::mutex> lock(m_myMutex);
    m_flagSDO = true;
    return T_NOERROR;
}

int MotorDriver::setMaxVelErr(const MotorType& type, const int& jointIndex, const SDO_COMMAND& sdoCmd){
    if(type == MotorType::MOONS || type == MotorType::MAXON){
        LOG(WARNING) << "WARNING: Setting max velocity error is not supported for moons or maxon motor!";
        return T_ERROR;
    }

    int preIdx = 1;
    //set tHeader information.
    m_tSendPkt.tHeader.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
    m_tSendPkt.tHeader.ulCmd = static_cast<uint32_t>(UlCmd::ECM_IF_CMD_COE_SDO_DOWNLOAD_REQ);
    m_tSendPkt.tHeader.ulLen = lengthSDODataHeader + sizeof(uint32_t);  // which is 18+4
    m_tSendPkt.tHeader.ulId = PROCESSID;

    //set SDO data info.
    // Motor Address
    m_tSendPkt.abData[0] = jointIndex + preIdx;
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
    if(targetState != MasterState::ECM_IF_STATE_INIT && targetState != MasterState::ECM_IF_STATE_PREOP && targetState != MasterState::ECM_IF_STATE_SAFEOP
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
    if(slaveIdx > m_slaveNum - 1){
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
    if(slaveIdx > m_slaveNum - 1){
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
    tSendPacket.abData[0] = static_cast<unsigned char>(slaveIdx);// + 1
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

    if(lRet != CIFX_NO_ERROR)
    {
        LOG(ERROR) << "Error initializing driver! Error code: 0x" << std::hex << lRet ;
        return lRet;
    }
    if (CIFX_NO_ERROR != (lRet = xDriverRestartDevice(&m_hDriver, CIFX_DEV, NULL)) )
    {
       LOG(ERROR) << "Error restart cifX driver! Error code: 0x" << std::hex << lRet ;
       return lRet;
    }

    // open connection/handle to cifX driver
    if (CIFX_NO_ERROR != (lRet = xDriverOpen(&m_hDriver)) ){
        LOG(ERROR) << "Error opening cifX driver! Error code: 0x" << std::hex << lRet ;
        return lRet;
    }

    // open connection to communication/user channel on the given board
    if(CIFX_NO_ERROR != (lRet = xChannelOpen(m_hDriver, CIFX_DEV, 0, &m_hChannel))){
        LOG(ERROR) << "Error opening channel! Error code: 0x" << std::hex << lRet ;
        return lRet;
    }

    //query basic channel information
    CHANNEL_INFORMATION tChannelInfo = {{0}};
    if(CIFX_NO_ERROR != (lRet = xChannelInfo(m_hChannel, sizeof(CHANNEL_INFORMATION), &tChannelInfo))){
        LOG(ERROR) << "Error querying system information block! Error code: " << lRet ;
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
//        dumpPacket(&m_tSendPkt);

        if(CIFX_NO_ERROR != (lRet = xChannelGetPacket(m_hChannel, sizeof(m_tRecvPkt), &m_tRecvPkt, timeWaitMessage))){
            LOG(ERROR) << "Error getting packet from device! Error code: 0x" << std::hex << lRet ;
            return T_ERROR;
        }
        else{
            LOG(INFO) << "Received Packet" ;
//            dumpPacket(&m_tRecvPkt);
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

void MotorDriver::enableMotor(const MotorType& type, const int& index){

    switch(type){
        case MotorType::MOONS:{
            // TODO
            if(m_jointEnabled[m_endGimbalMotorNum + index]){
                LOG(INFO) << "Moons Motor " << index + 1 << " is already enabled." ;
                break;
            }
            LOG(INFO) << "Starting initialize Moons Motor:" << " motor " << index + 1;
            if(setControlWord(type, index, ControlCommand::CLEAR_ERROR) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to clear error for Moons motor!" ;
                break;
            }
            usleep(50*1000);
            LOG(INFO) << "1: the control word is: " << static_cast<int>(ControlCommand::CLEAR_ERROR) <<  " " << "status word is: 0x" << std::hex << getStatusWord(type, index);

            if(setControlWord(type, index, ControlCommand::SHUT_DOWN) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to shut down Moons motor!" ;
                break;
            }
            usleep(50*1000);
            LOG(INFO) << "2: the control word is: " << static_cast<int>(ControlCommand::SHUT_DOWN) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            if(setControlWord(type, index, ControlCommand::SWITCH_ON) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to switch on Moons motor!";
                break;
            }
            usleep(50*1000);
            LOG(INFO) << "3: the control word is: " << static_cast<int>(ControlCommand::SWITCH_ON) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            if(setControlWord(type, index, ControlCommand::ENABLE) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to enable Moons motor!";
                break;
            }
            usleep(50*1000);
            LOG(INFO) << "4: the control word is: " << static_cast<int>(ControlCommand::ENABLE) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            m_jointEnabled[index] = true;
            LOG(INFO) << "Successfully enable Moons motor " << index + 1 ;
            break;
        }
        case MotorType::ZERO_ERR:{

            auto statusword = getStatusWord(type, index);
            LOG(INFO) << "current ZeroErr " << index + 1 << " status word is: " << std::hex <<statusword;

            auto errCode = getErrorCode(type,index);
            LOG(INFO) << "current ZeroErr " << index + 1 << " error code is: " << std::hex <<errCode;

            //TODO
            if (m_jointEnabled[m_endGimbalMotorNum + m_endGimbalMotorNum + index]){// && (statusword == 1237 || statusword == 1637 || statusword == 5687)
                LOG(INFO) << "ZeroErr Motor " << index + 1 << " is already enabled." ;
                break;
            }
            LOG(INFO) << "Starting initialize ZeroErr Motor: " <<  " motor" << index + 1;

            if(setControlWord(type, index, ControlCommand::CLEAR_ERROR) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to clear error for ZeroErr motor!" ;
                break;
            }
            usleep(50*1000);
            LOG(INFO) << "1: the control word is: " << static_cast<int>(ControlCommand::CLEAR_ERROR) <<  " " << "status word is: 0x" << std::hex << getStatusWord(type, index);

            if(setControlWord(type, index, ControlCommand::SHUT_DOWN) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to shut down ZeroErr motor!" ;
                break;
            }
            usleep(50*1000);
            LOG(INFO) << "2: the control word is: " << static_cast<int>(ControlCommand::SHUT_DOWN) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            if(setControlWord(type, index, ControlCommand::SWITCH_ON) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to switch on ZeroErr motor!";
                break;
            }
            usleep(50*1000);
            LOG(INFO) << "3: the control word is: " << static_cast<int>(ControlCommand::SWITCH_ON) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            if(setControlWord(type, index, ControlCommand::ENABLE) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to enable ZeroErr motor!";
                break;
            }
            usleep(50*1000);
            LOG(INFO) << "4: the control word is: " << static_cast<int>(ControlCommand::ENABLE) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            //TODO
            m_jointEnabled[m_endGimbalMotorNum + m_endJointMotorNum + index] = true;
            LOG(INFO) << "Successfully enable ZeroErr motor " << index + 1 ;
            break;
        }
        case MotorType::MAXON:{
            //TODO
            if(m_jointEnabled[m_endJointMotorNum + m_endGimbalMotorNum + m_endJointMotorNum + index]){
                LOG(INFO) << "Maxon motor " << index + 1 << " is already enabled." ;
                break;
            }
            LOG(INFO) << "Starting initialize Maxon Motor:" << " motor " << index + 1;
            usleep(50 * 1000);

            if(setControlWord(type, index, ControlCommand::CLEAR_ERROR) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to clear error for Maxon motor!";
                break;
            }
            usleep(50 * 1000);
            LOG(INFO) << "1: the control word is: " << static_cast<int>(ControlCommand::CLEAR_ERROR) <<  " " << "status word is: 0x" << std::hex << getStatusWord(type, index);

            if (setControlWord(type, index, ControlCommand::SHUT_DOWN) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to shut down Maxon motor!";
                break;
            }
            usleep(50 * 1000);
            LOG(INFO) << "2: the control word is: " << static_cast<int>(ControlCommand::SHUT_DOWN) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            if(setControlWord(type, index, ControlCommand::SWITCH_ON) != T_NOERROR){
                LOG(ERROR) << "Error switching on motor!";
                return;
            }
            usleep(50*1000);
            LOG(INFO) << "3: the control word is: " << static_cast<int>(ControlCommand::SWITCH_ON) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            if (setControlWord(type, index, ControlCommand::ENABLE) != T_NOERROR) {
                LOG(ERROR) << "Error: Failed to enable max motor!";
                break;
            }
            usleep(50*1000);
            LOG(INFO) << "4: the control word is: " << static_cast<int>(ControlCommand::ENABLE) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);
            //TODO
            m_jointEnabled[m_endJointMotorNum + m_endGimbalMotorNum + m_endJointMotorNum + index] = true;
            LOG(INFO) << "Successfully enable Maxon motor " << index + 1 ;
            break;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            break;
        }
    }
}

void MotorDriver::enableMotor_PP(const MotorType& type, const int& index){

    switch(type){

    case MotorType::MOONS:{
               LOG(INFO) << "Starting initialize Moons Motor:" << " motor " << index + 1;
               if(setControlWord(type, index, ControlCommand::CLEAR_ERROR) != T_NOERROR){
                   LOG(ERROR) << "Error: Failed to clear error for Moons motor!" ;
                   break;
               }
               usleep(50*1000);
               LOG(INFO) << "1: the control word is: " << static_cast<int>(ControlCommand::CLEAR_ERROR) <<  " " << "status word is: 0x" << std::hex << getStatusWord(type, index);

               if(setControlWord(type, index, ControlCommand::SHUT_DOWN) != T_NOERROR){
                   LOG(ERROR) << "Error: Failed to shut down Moons motor!" ;
                   break;
               }
               usleep(50*1000);
               LOG(INFO) << "2: the control word is: " << static_cast<int>(ControlCommand::SHUT_DOWN) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

               if(setControlWord(type, index, ControlCommand::SWITCH_ON) != T_NOERROR){
                   LOG(ERROR) << "Error: Failed to switch on Moons motor!";
                   break;
               }
               usleep(50*1000);
               LOG(INFO) << "3: the control word is: " << static_cast<int>(ControlCommand::SWITCH_ON) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

               if(setControlWord(type, index, ControlCommand::ENABLE) != T_NOERROR){
                   LOG(ERROR) << "Error: Failed to enable Moons motor!";
                   break;
               }
               usleep(50*1000);
               LOG(INFO) << "4: the control word is: " << static_cast<int>(ControlCommand::ENABLE) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);


               if(setControlWord(type, index, ControlCommand::NEW_SET_POINT_MOONS) != T_NOERROR){
                   LOG(ERROR) << "Error: Failed to Set Point relavtive of Moons motor!";
                   break;
               }
               usleep(50*1000);
               LOG(INFO) << "5: the control word is: " << static_cast<int>(ControlCommand::NEW_SET_POINT_MOONS) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);


               m_jointEnabled[index] = true;
               LOG(INFO) << "Successfully enable Moons motor in PP Mode" << index + 1 ;
               break;
           }
        case MotorType::ZERO_ERR:{

            LOG(INFO) << "Starting initialize ZeroErr Motor in PP Mode: " <<  " motor" << index + 1;

            if(setControlWord(type, index, ControlCommand::CLEAR_ERROR) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to clear error for ZeroErr motor!" ;
                break;
            }
            usleep(50*1000);
            LOG(INFO) << "1: the control word is: " << static_cast<int>(ControlCommand::CLEAR_ERROR) <<  " " << "status word is: 0x" << std::hex << getStatusWord(type, index);

            if(setControlWord(type, index, ControlCommand::SHUT_DOWN) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to shut down ZeroErr motor!" ;
                break;
            }
            usleep(50*1000);
            LOG(INFO) << "2: the control word is: " << static_cast<int>(ControlCommand::SHUT_DOWN) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            if(setControlWord(type, index, ControlCommand::SWITCH_ON) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to switch on ZeroErr motor!";
                break;
            }
            usleep(50*1000);
            LOG(INFO) << "3: the control word is: " << static_cast<int>(ControlCommand::SWITCH_ON) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            if(setControlWord(type, index, ControlCommand::ENABLE_PP) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to enable ZeroErr motor!";
                break;
            }
            usleep(50*1000);
            LOG(INFO) << "4: the control word is: " << static_cast<int>(ControlCommand::ENABLE_PP) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            //TODO
            m_jointEnabled[m_endGimbalMotorNum + m_endJointMotorNum + index] = true;
            LOG(INFO) << "Successfully enable ZeroErr motor " << index + 1 ;
            break;
        }
        case MotorType::MAXON:{
            //TODO
            if(m_jointEnabled[m_endJointMotorNum + m_endGimbalMotorNum + m_endJointMotorNum + index]){
                LOG(INFO) << "Maxon Motor " << index + 1 << " is already enabled." ;
                break;
            }
            LOG(INFO) << "Starting initialize Maxon Motor:" << " motor " << index + 1;

            if(setControlWord(type, index, ControlCommand::CLEAR_ERROR) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to clear error for maxon motor!";
                break;
            }
            usleep(50 * 1000);
            LOG(INFO) << "1: the control word is: " << static_cast<int>(ControlCommand::CLEAR_ERROR) <<  " " << "status word is: 0x" << std::hex << getStatusWord(type, index);

            if (setControlWord(type, index, ControlCommand::SHUT_DOWN) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to shut down maxon motor!";
                break;
            }
            usleep(50 * 1000);
            LOG(INFO) << "2: the control word is: " << static_cast<int>(ControlCommand::SHUT_DOWN) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            if(setControlWord(type, index, ControlCommand::SWITCH_ON) != T_NOERROR){
                LOG(ERROR) << "Error switching on motor!";
                return;
            }
            usleep(50*1000);

            if (setControlWord(type, index, ControlCommand::ENABLE) != T_NOERROR) {
                LOG(ERROR) << "Error: Failed to enable max motor!";
                break;
            }
            usleep(50*1000);
            LOG(INFO) << "3: the control word is: " << static_cast<int>(ControlCommand::ENABLE) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);
            //TODO
            m_jointEnabled[m_endJointMotorNum + m_endGimbalMotorNum + m_endJointMotorNum + index] = true;
            LOG(INFO) << "Successfully enable Maxon motor " << index + 1 ;
            break;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            break;
        }
    }
}

void MotorDriver::enableMotor_Homing(const MotorType &type, const int &index)
{
    switch (type) {
        case MotorType::MAXON: {
            if(setControlWord(type, index, ControlCommand::CLEAR_ERROR) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to clear error for MAXON motor!" ;
                break;
            }
            usleep(50 * 1000);
            LOG(INFO) << "1: the control word is: " << static_cast<int>(ControlCommand::CLEAR_ERROR) <<  " " << "status word is: 0x" << std::hex << getStatusWord(type, index);

            if(setControlWord(type, index, ControlCommand::SHUT_DOWN) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to shut down Moons motor!" ;
                break;
            }
            usleep(50 * 1000);
            LOG(INFO) << "2: the control word is: " << static_cast<int>(ControlCommand::SHUT_DOWN) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            if(setControlWord(type, index, ControlCommand::ENABLE) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to enable Moons motor!";
                break;
            }
            usleep(50 * 1000);
            LOG(INFO) << "3: the control word is: " << static_cast<int>(ControlCommand::ENABLE) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            if(setControlWord(type, index, ControlCommand::MOTION_START_HOMING) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to enable Moons motor!";
                break;
            }
            usleep(50 * 1000);
            LOG(INFO) << "4: the control word is: " << static_cast<int>(ControlCommand::MOTION_START_HOMING) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);
            break;
            }

            default:{
                LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
                break;
            }
        }
}


void MotorDriver::operationCSP(const MotorType& type, const int& index){
    switch(type){
        case MotorType::MOONS:{

            LOG(INFO) << "Start setting operation mode to CSP for Moons." ;

            usleep(20 * 1000);

            if(setProfileVel(type, index, PROFILE_VEL_MOONS_CSP) != T_NOERROR){
                LOG(ERROR) << "Failed to set max profile vel for Moons motor!" ;
                break;
            }
            usleep(20 * 1000);

            if(setProfileAcc(type, index, PROFILE_ACC_MOONS_CSP) != T_NOERROR){
                LOG(ERROR) << "Failed to set profile acceleration for Moons motor!" ;
                break;
            }
            usleep(20 * 1000);

            if(setProfileDec(type, index, PROFILE_DEC_MOONS_CSP) != T_NOERROR){
                LOG(ERROR) << "Failed to set profile deceleration for Moons motor!" ;
                break;
            }
            usleep(20 * 1000);

            if (setMaxPosErr(type, index, SDO_COMMAND::MAX_POS_ERR) != T_NOERROR){
                LOG(ERROR) << " Failed to set SDO 0x6065: Max Position Error for Moons motor!" ;
                break;
            }
            usleep(20 * 1000);

            if(setOperationMode(type, index, OperationMode::CSP) != T_NOERROR){
                LOG(ERROR) << "Failed to set operation mode to CSP for Moons motor!" ;
                break;
            }

            usleep(50*1000);
            enableMotor(type, index);
            break;
        }
        case MotorType::ZERO_ERR:{

            LOG(INFO) << "Start setting operation mode to CSP for ZeroErr." ;

            const auto actualPos = getActualPos(type, index);
            LOG(DEBUG) << "current pos encoder is: " << std::dec << actualPos;
            if(setTargetPos(type, index, actualPos) != T_NOERROR){
                LOG(ERROR) << "Failed to set actual pos to target pos for ZeroErr!" ;
                break;
            }
            usleep(20 * 1000);

            if(setMaxProfileVel(type, index, MAX_PROFILE_VEL) != T_NOERROR){
                LOG(ERROR) << "Failed to set max profile vel for ZeroErr!" ;
                break;
            }
            usleep(20 * 1000);

            if(setProfileAcc(type, index, PROFILE_ACC) != T_NOERROR){
                LOG(ERROR) << "Failed to set profile acceleration for ZeroErr!" ;
                break;
            }
            usleep(20 * 1000);

            if(setProfileDec(type, index, PROFILE_DEC) != T_NOERROR){
                LOG(ERROR) << "Failed to set profile deceleration for ZeroErr!" ;
                break;
            }
            usleep(20 * 1000);

            if (setMaxPosErr(type, index, SDO_COMMAND::MAX_POS_ERR) != T_NOERROR){
                LOG(ERROR) << " Failed to set SDO 0x6065: Max Position Error for ZeroErr!" ;
                break;
            }
            usleep(20 * 1000);

            if(setOperationMode(type, index, OperationMode::CSP) != T_NOERROR){
                LOG(ERROR) << "Failed to set operation mode to CSP for ZeroErr!" ;
                break;
            }

            usleep(50*1000);
            enableMotor(type, index);
            break;
        }
        case MotorType::MAXON:{
            LOG(INFO) << "Start setting operation mode to CSP for Maxon." ;

            const auto actualPos = getActualPos(type, index);
            LOG(DEBUG) << "current pos encoder is: " << std::dec << actualPos;
            if(setTargetPos(type, index, actualPos) != T_NOERROR){
                LOG(ERROR) << "Failed to set actual pos to target pos for MAXON!" ;
                break;
            }
            usleep(20 * 1000);

            if(setInterpolationTime(type, index, interpolationTime_maxonMotor) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to set interpolation time!";
                break;
            }
            usleep(20 * 1000);

//            if(setProfileAcc(type, index, PROFILE_ACC) != T_NOERROR){
//                LOG(ERROR) << "Failed to set profile acceleration for MAXON!" ;
//                break;
//            }
//            usleep(20 * 1000);

//            if(setProfileDec(type, index, PROFILE_DEC) != T_NOERROR){
//                LOG(ERROR) << "Failed to set profile deceleration for MAXON!" ;
//                break;
//            }
//            usleep(20 * 1000);

            if(setOperationMode(type, index, OperationMode::CSP) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to set operation mode to CSP for maxon motor!";
                break;
            }

            enableMotor(type, index);
            break;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            break;
        }
    }

}

void MotorDriver::operationCSV(const MotorType& type, const int& index){
    switch(type){
        case MotorType::MOONS:{
            LOG(INFO) << "Start setting operation mode to CSV for MOONS." ;
            if(setOperationMode(type, index, OperationMode::CSV) != T_NOERROR){
                LOG(ERROR) << "Failed to set operation mode to CSV for MOONS!";
                break;
            }
            usleep(50 * 1000);
            if(setTargetVel(type, index, zeroVel) != T_NOERROR){
                LOG(ERROR) << "Failed to set current vel to 0 for MOONS!";
                break;
            }
            enableMotor(type, index);
            usleep(50*1000);
            break;
        }
        case MotorType::ZERO_ERR:{
            LOG(INFO) << "Start setting operation mode to CSV for ZeroErr." ;
            if(setOperationMode(type, index, OperationMode::CSV) != T_NOERROR){
                LOG(ERROR) << "Failed to set operation mode to CSV for ZeroErr!" << std::endl;
                break;
            }
            if(setTargetVel(type, index, zeroVel) != T_NOERROR){
                LOG(ERROR) << "Failed to set current vel to 0 for ZeroErr!" << std::endl;
                break;
            }
            if(setMaxProfileVel(type, index, MAX_PROFILE_VEL) != T_NOERROR){
                LOG(ERROR)<< "Failed to set max profile vel for ZeroErr!" << std::endl;
                return;
            }
            if(setProfileAcc(type, index, PROFILE_ACC) != T_NOERROR){
                LOG(ERROR) << "Failed to set profile acceleration for ZeroErr!" << std::endl;
                break;
            }
            if(setProfileDec(type, index, PROFILE_DEC) != T_NOERROR){
                LOG(ERROR) << "Failed to set profile deceleration for ZeroErr!" << std::endl;
                break;
            }
            if (setMaxVelErr(type, index, SDO_COMMAND::MAX_VEL_ERR) != T_NOERROR){
                LOG(ERROR) << " Failed to set SDO 0x3B60: Max Velocity Error for ZeroErr!" << std::endl;
                break;
            }
            usleep(50*1000);
            enableMotor(type, index);
            break;
        }
        case MotorType::MAXON:{
            LOG(INFO) << "Start setting operation mode to CSV for Maxon." ;

            if(setOperationMode(type, index, OperationMode::CSV) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to set operation mode to CSV for maxon motor!";
                break;
            }
            if(setInterpolationTime(type, index, interpolationTime_maxonMotor) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to set interpolation time!";
                break;
            }
            usleep(50 * 1000);
            enableMotor(type, index);
            break;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!" << static_cast<int>(type);
            break;
        }
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
    enableMotor(type, index);
}

void MotorDriver::operationPP(const MotorType& type, const int& index){

    switch(type){
        case MotorType::MOONS:{
        if(setOperationMode(type, index, OperationMode::PP) != T_NOERROR){
            LOG(ERROR) << "Failed to set operation mode to PP for Moons!" ;
            break;
        }
        usleep(150 * 1000);

        if(setProfileVel(type, index, PROFILE_VEL_MOONS) != T_NOERROR)
        {
            LOG(ERROR) << "Failed to set max profile vel for Moons!";
            std::cout << "最大速度： " << PROFILE_VEL_MOONS << std::endl;
            break;
        }
        usleep(50 * 1000);

        if(setProfileAcc(type, index, PROFILE_ACC_MOONS) != T_NOERROR){
            LOG(ERROR) << "Failed to set profile acceleration for ZeroErr!";
            break;
        }
        usleep(50 * 1000);

        if(setProfileDec(type, index, PROFILE_DEC_MOONS) != T_NOERROR){
            LOG(ERROR) << "Failed to set profile deceleration for ZeroErr!";
            break;
        }
        usleep(50 * 1000);

        enableMotor_PP(type, index);
        break;

        }
        case MotorType::ZERO_ERR:{
            LOG(INFO) << "Starting set operation mode to PP for ZeroErr." ;
//            const auto actualPos = getActualPos(type, index);
//            if(setTargetPos(type, index, actualPos) != T_NOERROR){
//                LOG(ERROR) << "Failed to set actual pos to target pos for ZeroErr!" ;
//                break;
//            }
//            usleep(5 * 1000);

            if(setOperationMode(type, index, OperationMode::PP) != T_NOERROR){
                LOG(ERROR) << "Failed to set operation mode to PP for ZeroErr!" ;
                break;
            }
            usleep(50*1000);
            if(setMaxProfileVel(type, index, MAX_PROFILE_VEL) != T_NOERROR){
                LOG(ERROR) << "Failed to set max profile vel for ZeroErr!";
                break;
            }

            if(setProfileAcc(type, index, PROFILE_ACC) != T_NOERROR){
                LOG(ERROR) << "Failed to set profile acceleration for ZeroErr!";
                break;
            }

            if(setProfileDec(type, index, PROFILE_DEC) != T_NOERROR){
                LOG(ERROR) << "Failed to set profile deceleration for ZeroErr!";
                break;
            }


            enableMotor_PP(type, index);
            break;
        }
        case MotorType::MAXON:{
            LOG(WARNING) << "PP mode is not implemented for Maxon Motor.";
            break;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            break;
        }
    }
}


void MotorDriver::gotoTargetPos_PPMode(const MotorType& type, const int& index, const int32_t& targetVel, const int32_t& targetPos)
{
    switch(type){
        case MotorType::MOONS:{
            LOG(WARNING) << "PP mode is not implemented for MOONS Motor.";
            if(setOperationMode(type, index, OperationMode::PP) != T_NOERROR){
                LOG(ERROR) << "Failed to set operation mode to PP for Moons!" ;
                break;
            }
            break;
        }
        case MotorType::ZERO_ERR:{
            LOG(INFO) << "Starting go to target Position in PP mode for ZeroErr " << index + 1;

            auto errCode = getErrorCode(type,index);
            if(setOperationMode(type, index, OperationMode::PP) != T_NOERROR){
                LOG(ERROR) << "Failed to set operation mode to PP for ZeroErr!" << index + 1 ;
                break;
            }
            usleep(100 * 1000);
            LOG(INFO) << "1: the target operation mode is: " << static_cast<int>(OperationMode::PP) <<  " "
                      << "operation mode display: 0x" << std::hex << getOperationMode(type, index)<<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            if(setProfileVel(type, index, targetVel) != T_NOERROR){
                LOG(ERROR) << "Failed to set actual pos to target vel for ZeroErr!"<<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index); ;
                break;
            }
            LOG(INFO) << "2: set Profile velocity: " << std::dec << targetVel;

            if(setTargetPos(type, index, targetPos) != T_NOERROR){
                LOG(ERROR) << "Failed to set actual pos to target pos for ZeroErr!" ;
                break;
            }
            LOG(INFO) << "3: set target position: " << std::dec<< targetPos<<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);;
            usleep(100 * 1000);

            if(setControlWord(type, index, ControlCommand::SHUT_DOWN) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to shut down ZeroErr motor!" ;
                break;
            }
            usleep(1000 * 1000);
            LOG(INFO) << "4: the control word is: " << static_cast<int>(ControlCommand::SHUT_DOWN) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            if(setControlWord(type, index, ControlCommand::SWITCH_ON) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to switch on ZeroErr motor!";
                break;
            }
            usleep(1000 * 1000);
            LOG(INFO) << "5: the control word is: " << static_cast<int>(ControlCommand::SWITCH_ON) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            if(setControlWord(type, index, ControlCommand::ENABLE_PP) != T_NOERROR){
                LOG(ERROR) << "Error: Failed to enable Zero-Error motor!";
                break;
            }
            usleep(1000 * 1000);
            LOG(INFO) << "6: the control word is: " << std::hex << static_cast<int>(ControlCommand::ENABLE_PP) <<  " " << "status word is: 0x " << std::hex << getStatusWord(type, index);

            break;
        }

        case MotorType::MAXON:{
            LOG(WARNING) << "PP mode is not implemented for Maxon Motor.";
            break;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            break;
        }
    }
}

void MotorDriver::operationHOME(const MotorType& type, const int& index){

    switch (type) {
        case MotorType::MAXON:{
            LOG(INFO) << "Starting set operation mode to HOME for MAXONS";

            if(setOperationMode(type, index, OperationMode::HOME) != T_NOERROR){
                LOG(ERROR) << "Failed to set operation mode to HomeMode for MAXONS";
                return;
            }
            usleep(50 * 1000);
            LOG(INFO) << "OP Mode is: 0x" << std::hex << getOperationMode(type, index);

            int homeMode = 23;
            if (setHomeMethod(type, index, homeMode) != T_NOERROR){
                LOG(ERROR) << "Failed to set profile HomeMethod for MAXONS!";
                return;
            }
            usleep(50 * 1000);

//            if (setHomeVel(type, index, 100) != T_NOERROR){
            if (setHomeVel(type, index, 300) != T_NOERROR){
                LOG(ERROR) << "Failed to set profile SearchZeroVel for MAXONS!";
                return;
            }
            usleep(50 * 1000);

            enableMotor_Homing(type, index);
            break;
        }
        case MotorType::ZERO_ERR:{
            LOG(WARNING) << "Homing mode is not implemented for Zero Error Motor.";
            break;
        }
        case MotorType::MOONS:{
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
            //TODO
            // if (setHomeAcc(type, index, PROFILE_ACC) != T_NOERROR){
            //     LOG(ERROR) << "Failed to set profile HomingAcc for MOONS!";
            //     return;
            // }

            const int32_t velOffset = 0;
            if (setHomeOffset(type, index, velOffset)){
                LOG(ERROR) << "Failed to set profile HomingOffset!";
                return;
            }
            break;
        }
        default:{
            LOG(ERROR) << "Error: The input motor type is undefined, please check motor type!";
            break;
        }
    }
}

int MotorDriver::checkECatStationState(){
    int lRet;
    if((lRet = m_selfPointer->getECatMasterState()) != static_cast<int>(MasterState::ECM_IF_STATE_OP)){
        LOG(ERROR) << "Connection lost, EtherCAT master is not in OP state, current state is: 0x" << std::hex << lRet;
        return T_ERROR;
    }

    if((lRet = m_selfPointer->getECatSlaveState(0)) != static_cast<int>(SlaveState::ECM_IF_STATE_OP))
    {
        LOG(ERROR) << "Connection lost, EtherCAT slave 1 is not in OP state, current state is: 0x" << std::hex << lRet;
    }

    if((lRet = m_selfPointer->getECatSlaveState(1)) != static_cast<int>(SlaveState::ECM_IF_STATE_OP)){
        LOG(ERROR) << "Connection lost, EtherCAT slave 2 is not in OP state, current state is: 0x" << std::hex << lRet;
        return T_ERROR;
    }

    if((lRet = m_selfPointer->getECatSlaveState(2)) != static_cast<int>(SlaveState::ECM_IF_STATE_OP)){
        LOG(ERROR) << "Connection lost, EtherCAT slave 3 is not in OP state, current state is: 0x" << std::hex << lRet;
        return T_ERROR;
    }

    if((lRet = m_selfPointer->getECatSlaveState(3)) != static_cast<int>(SlaveState::ECM_IF_STATE_OP)){
        LOG(ERROR) << "Connection lost, EtherCAT slave 4 is not in OP state, current state is: 0x" << std::hex << lRet;
        return T_ERROR;
    }

    if((lRet = m_selfPointer->getECatSlaveState(4)) != static_cast<int>(SlaveState::ECM_IF_STATE_OP)){
        LOG(ERROR) << "Connection lost, EtherCAT slave 5 is not in OP state, current state is: 0x" << std::hex << lRet;
        return T_ERROR;
    }

    if((lRet = m_selfPointer->getECatSlaveState(5)) != static_cast<int>(SlaveState::ECM_IF_STATE_OP)){
        LOG(ERROR) << "Connection lost, EtherCAT slave 6 is not in OP state, current state is: 0x" << std::hex << lRet;
        return T_ERROR;
    }

    if((lRet = m_selfPointer->getECatSlaveState(6)) != static_cast<int>(SlaveState::ECM_IF_STATE_OP)){
        LOG(ERROR) << "Connection lost, EtherCAT slave 7 is not in OP state, current state is: 0x" << std::hex << lRet;
        return T_ERROR;
    }
    return T_NOERROR;
    if((lRet = m_selfPointer->getECatSlaveState(7)) != static_cast<int>(SlaveState::ECM_IF_STATE_OP)){
        LOG(ERROR) << "Connection lost, EtherCAT slave 5 is not in OP state, current state is: 0x" << std::hex << lRet;
        return T_ERROR;
    }

    if((lRet = m_selfPointer->getECatSlaveState(6)) != static_cast<int>(SlaveState::ECM_IF_STATE_OP)){
        LOG(ERROR) << "Connection lost, EtherCAT slave 6 is not in OP state, current state is: 0x" << std::hex << lRet;
        return T_ERROR;
    }

    if((lRet = m_selfPointer->getECatSlaveState(9)) != static_cast<int>(SlaveState::ECM_IF_STATE_OP)){
        LOG(ERROR) << "Connection lost, EtherCAT slave 7 is not in OP state, current state is: 0x" << std::hex << lRet;
        return T_ERROR;
    }
}

int MotorDriver::checkMotorState(){
    int lRet;

    if((lRet = getErrorCode(MotorType::MOONS, 0)) != 0x0){
        LOG(ERROR) << "Error: Moons motor x-direction, error code is: 0x" << std::hex << lRet;
         return T_ERROR;
    }
    if((lRet = getErrorCode(MotorType::ZERO_ERR, 0)) != 0x0){
//        LOG(ERROR) << "Error: Zero Error motor joint 1, error code is: 0x" << std::hex << lRet;
         return T_ERROR;
    }
    if((lRet = getErrorCode(MotorType::ZERO_ERR, 1)) != 0x0){
        LOG(ERROR) << "Error: Zero Error motor joint 2, error code is: 0x" << std::hex << lRet;
         return T_ERROR;
    }
    if((lRet = getErrorCode(MotorType::ZERO_ERR, 2)) != 0x0){
        LOG(ERROR) << "Error: Zero Error motor joint 3, error code is 0x: " << std::hex << lRet;
         return T_ERROR;
    }
    if((lRet = getErrorCode(MotorType::MAXON, 0)) != 0x0){
        LOG(ERROR) << "Error: Maxon motor joint 1, error code is: 0x" << std::hex << lRet;
         return T_ERROR;
    }
    if((lRet = getErrorCode(MotorType::MAXON, 1)) != 0x0){
        LOG(ERROR) << "Error: Maxon motor joint 2, error code is 0x: " << std::hex << lRet;
         return T_ERROR;
    }
    if((lRet = getErrorCode(MotorType::MAXON, 2)) != 0x0){
        LOG(ERROR) << "Error: Maxon motor joint 3, error code is: 0x" << std::hex << lRet;
         return T_ERROR;
    }
    if((lRet = getErrorCode(MotorType::MAXON, 3)) != 0x0){
        LOG(ERROR) << "Error: Maxon motor joint 4, error code is 0x: " << std::hex << lRet;
         return T_ERROR;
    }
    return T_NOERROR;
    if((lRet = getErrorCode(MotorType::MAXON, 4)) != 0x0){
        LOG(ERROR) << "Error: Maxon motor joint 3, error code is: 0x" << std::hex << lRet;
         return T_ERROR;
    }
    if((lRet = getErrorCode(MotorType::MAXON, 5)) != 0x0){
        LOG(ERROR) << "Error: Maxon motor joint 4, error code is 0x: " << std::hex << lRet;
         return T_ERROR;
    }
}

void MotorDriver::displayMotorErrCode(){
    LOG(INFO) << "Moons motor x-direction, error code is: 0x" << std::hex << getErrorCode(MotorType::MOONS, 0);
    LOG(INFO) << "Zero Error motor joint 1, error code is: 0x" << std::hex << getErrorCode(MotorType::ZERO_ERR, 0);
    LOG(INFO) << "Zero Error motor joint 2, error code is: 0x" << std::hex << getErrorCode(MotorType::ZERO_ERR, 1);
    LOG(INFO) << "Zero Error motor joint 3, error code is: 0x" << std::hex << getErrorCode(MotorType::ZERO_ERR, 2);
    LOG(INFO) << "Maxon motor joint 1, error code is: 0x" << std::hex << getErrorCode(MotorType::MAXON, 0);
    LOG(INFO) << "Maxon motor joint 2, error code is: 0x" << std::hex << getErrorCode(MotorType::MAXON, 1);
    LOG(INFO) << "Maxon motor joint 3, error code is: 0x" << std::hex << getErrorCode(MotorType::MAXON, 2);
    LOG(INFO) << "Maxon motor joint 4, error code is: 0x" << std::hex << getErrorCode(MotorType::MAXON, 3);
    LOG(INFO) << "Maxon motor joint 5, error code is: 0x" << std::hex << getErrorCode(MotorType::MAXON, 4);
    LOG(INFO) << "Maxon motor joint 6, error code is: 0x" << std::hex << getErrorCode(MotorType::MAXON, 5);

}

void MotorDriver::motorDriverThread(std::promise<bool> &promiseCommunication){
    auto lRet = m_selfPointer->motorDriverInit();
    if(lRet != CIFX_NO_ERROR){
        LOG(ERROR) << "cifX driver cannot be initialized, hence, motor driver thread cannot be started.";
        m_selfPointer->m_threadTerminated.store(true, std::memory_order_release);
        return;
    }

    while(!m_selfPointer->m_threadTerminated){

        if(m_selfPointer->checkECatStationState() != T_NOERROR){
            break;
        }

        m_selfPointer->checkMotorState();

        if(m_selfPointer->m_flagSDO){
            std::lock_guard<std::mutex> lock(m_selfPointer->m_myMutex);
            if (T_NOERROR != (lRet = m_selfPointer->mailboxPacketTransfer())){
                LOG(ERROR) << "SDO service is offline, connection may be lost." ;
                // m_isMotorDriverOk.store(false);
                break;
            }
            m_selfPointer->m_flagSDO = false;
            LOG(INFO) << "already set SDO ! " ;
        }

        if (T_NOERROR != (lRet = m_selfPointer->cyclicDataTransfer())){
            LOG(ERROR) << "PDO service is offline, connection may be lost." ;
            // m_isMotorDriverOk.store(false);
            break;
        }
    }

    m_selfPointer->displayMotorErrCode();
    m_selfPointer->motorDriverExit();
    m_selfPointer->m_threadTerminated.store(true, std::memory_order_release);
    LOG(INFO) << "MotorDriver thread ends!";
}

void MotorDriver::startThread(std::promise<bool> &promiseCommunication){
    m_etherCatThread = std::thread(motorDriverThread, std::ref(promiseCommunication));
    LOG(INFO) << "EtherCat thread ID: " << m_etherCatThread.get_id();
    m_etherCatThread.detach();
}

bool MotorDriver::isMotorDriverThreadTerminated(){
    const auto isMotorDriverThreadTerminated = m_threadTerminated.load(std::memory_order_acquire);
    return isMotorDriverThreadTerminated;
}

void MotorDriver::motorDriverExit(){
    closeBusConnection();
    xChannelClose(m_hChannel);
    xDriverClose(m_hDriver);
    cifXDriverDeinit();
}

void MotorDriver::SendInnerMsg(Module_Inner_E recever,int Action, QString arg)
{
    Message_Inner_T msgTemp;
    msgTemp.Sender = Module_Inner_E::MotorDriver;
    msgTemp.Recver = recever;
    msgTemp.Request.insert(Action, arg);
    m_messagePool.SendMessage(msgTemp);
}

void MotorDriver::SendInnerMsg(Module_Inner_E recever, int Action, QList<QString> arglist)
{
    Message_Inner_T msgTemp;
    msgTemp.Sender = Module_Inner_E::MotorDriver;
    msgTemp.Recver = recever;
    for(int i = 0; i < arglist.length(); i++)
    {
        msgTemp.Request.insert(Action, arglist[i]);
    }
    m_messagePool.SendMessage(msgTemp);
}


void MotorDriver::GetAmMsg(Message_Inner_T msg)
{
    m_MsgGottenRWLock.lockForWrite();
    m_MsgGottenQueue.enqueue(msg);
    m_MsgGottenRWLock.unlock();
    emit DealMsgSignal();
}

void MotorDriver::dealWithMsg()
{
  SteadyDelay(10);
    Message_Inner_T msg;
    while(1){

        m_MsgGottenRWLock.lockForRead();

        if(m_MsgGottenQueue.isEmpty() == false)
        {
            msg = m_MsgGottenQueue.dequeue();
        }else
        {
            m_MsgGottenRWLock.unlock();
            break;
        }

        m_MsgGottenRWLock.unlock();

        if(msg.Recver != Module_Inner_E::MotorDriver && msg.Recver != Module_Inner_E::MultipleModules){return;}

        QHash<int,QString>::ConstIterator i = msg.Request.constBegin();

        while(i != msg.Request.constEnd())
        {
            switch (i.key())
            {
                case static_cast<int>(MotorDriverAction_E::BootSelfCheck):
                {
                    break;
                }
                case static_cast<int>(MotorDriverAction_E::EtherCAT_Off):
                {
                    motorDriverExit();
                    LOG(INFO) << "Motor Driver Exit ";
//                    SendInnerMsg(Module_Inner_E::MasterConsole, static_cast<int>(MasterConsoleAction_E::MasterShutDown), "");
                    SendInnerMsg(Module_Inner_E::Security, static_cast<int>(SecurityAction_E::RecvMotorDriverShutDown), "Ok");
                    break;
                }

                default:break;
            }
            i++;
        }
    }
}

 MotorDriver* MotorDriver::getInstance(const MotorDriverParameter motorDriverParameter, MessageQueue&  messagePool){
     if(m_selfPointer == nullptr){
         m_selfPointer = new MotorDriver(motorDriverParameter, messagePool);
     }
     return m_selfPointer;
 }
