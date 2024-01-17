/**************************************************************************************

Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved.

***************************************************************************************

  $Id: CifXConsole.h 12620 2018-10-30 11:55:55Z LuisContreras $:

  Description:
    Headerfile for CifXTest_Console implementation

  Changes:
    Date        Description
    -----------------------------------------------------------------------------------
    2018-10-19  Ported from cifXTest_Console V1.0.6.0
    2010-07-20  initial version

**************************************************************************************/

/* prevent multiple inclusion */
#ifndef __CIFx_CONSOLE_H
#define __CIFx_CONSOLE_H

#include "cifXUser.h"
#include "cifXErrors.h"

/***************************************************************************
* Global Functions
***************************************************************************/
void ShowError          (int32_t lError);
void RunCifXConsoleTest (char* szBoard, char* pszFirmwareFileName, char* pszConfigFileName,
                         unsigned long ulTimerResolution, unsigned long ulIOTimeout);

#endif  /* __CIFx_CONSOLE_H */
