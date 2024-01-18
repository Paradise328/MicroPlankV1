/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   Filename:
    $Workfile: main.cpp $
   Last Modification:
    $Author: sebastiand $
    $Modtime: 9.02.10 15:45 $
    $Revision: 3096 $

   Targets:
     Linux : yes

   Description:
     Linux demo application (console)

   Changes:

     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
      1        23.11.2011  SD       - Initial Version

**************************************************************************************/

#include <limits.h>
#include "OS_Includes.h"
#include "CifXConsole.h"
#include "cifxlinux.h"

//-------------------------------------------------
// Default configurations
#define DEFAULT_BOARD    "cifX0"
//TODO: set to path where firmware and config is stored
#define DEFAULT_CONFIG   "/opt/cifx/../config.nxd"
#define DEFAULT_FIRMWARE "/opt/cifx/../fw.nxf"

//-------------------------------------------------
// Command line parameters and functions
// Program command line parameters
#define COMMAND_CARD       "/N:"
#define COMMAND_CARD_NO    0
#define COMMAND_WAIT       "/W:"
#define COMMAND_WAIT_NO    1
#define COMMAND_FW         "/F:"
#define COMMAND_FW_NO      2
#define COMMAND_CONFIG     "/C:"
#define COMMAND_CONFIG_NO  3
#define COMMAND_HELP       "/?"
#define COMMAND_HELP_NO    4

//-------------------------------------------------
// Helper functions for evaluating command line
// arguments
void SetString(char* szArgument, void* pvDest)
{
  char* szDest = reinterpret_cast<char*>(pvDest);
  strcpy(szDest, szArgument);
}

void SetTestDevice(char* szArgument, void* pvDest)
{
  int iDest;
  
  sscanf(szArgument, "%x", &iDest);
  sprintf( (char*)pvDest, "cifX%d", iDest);
}

void SetDWORD(char* szArgument, void* pvDest)
{
  uint32_t* pulDest = reinterpret_cast<uint32_t*>(pvDest);
  
  sscanf(szArgument, "%ul", pulDest);
}

//=============================================================================
// Show program help
//
//
//=============================================================================
void ShowProgramHelp( char* szArgument, void* pvUser)
{
  //(void*) 
  
  printf("CifX Test Program - Help\r\n");
  printf("  Options: \r\n");
  printf("  /N:<Number of board to test [0..n]>\r\n");
  printf("  /W:<IO wait timeout in milliseconds [1..n]>\r\n");
  printf("  /F:<Firmware to download>\r\n");
  printf("  /C:<Configuration to download>\r\n");
  printf("  /? - Show this help\r\n\n");
}


typedef void(*PFN_COMMAND)(char*, void*);

typedef struct tagCMDLINE_PARAMS
{
  char*       szCommandPrefix;
  PFN_COMMAND pfnCommand;
  void*       pvData;
  int         CmdNo;
} CMDLINE_PARAM;


/* global variable used to initialize the demo application */
char          g_szBoard[]    = DEFAULT_BOARD;
char*         g_pszBoardName = g_szBoard;
char          g_szFirmware[PATH_MAX];
char          g_szConfig[PATH_MAX];

unsigned long g_ulIOTimeout;

static CMDLINE_PARAM s_atCommands[] =
{                      
  {(char*)COMMAND_FW,     SetString,       g_szFirmware,  0},
  {(char*)COMMAND_CONFIG, SetString,       g_szConfig,      },
  {(char*)COMMAND_CARD,   SetTestDevice,   g_pszBoardName,  },
  {(char*)COMMAND_WAIT,   SetDWORD,        &g_ulIOTimeout,  },
  {(char*)COMMAND_HELP,   ShowProgramHelp, NULL,            }
};
//-------------------------------------------------

//=============================================================================
// Get program parameters
//
//
//=============================================================================
bool GetProgramParameters( int argc, char* argv[])
{
  for(int iArg = 1; iArg < argc; iArg++)
  {
    bool fFound = false;
    
    if(OS_Strnicmp(argv[iArg], 
                  s_atCommands[COMMAND_HELP_NO].szCommandPrefix,
                  strlen(s_atCommands[COMMAND_HELP_NO].szCommandPrefix)) == 0)
    {
      s_atCommands[COMMAND_HELP_NO].pfnCommand(argv[iArg] + strlen(s_atCommands[COMMAND_HELP_NO].szCommandPrefix),
                                          s_atCommands[COMMAND_HELP_NO].pvData);
      return false;
    }
    
    for(unsigned int iCommand = 0; iCommand < sizeof(s_atCommands) / sizeof(s_atCommands[0]); iCommand++)
    {
      if(OS_Strnicmp(argv[iArg], 
                  s_atCommands[iCommand].szCommandPrefix,
                  strlen(s_atCommands[iCommand].szCommandPrefix)) == 0)
      {
        s_atCommands[iCommand].pfnCommand(argv[iArg] + strlen(s_atCommands[iCommand].szCommandPrefix),
                                          s_atCommands[iCommand].pvData);
        fFound = true;
        break;
      }
    }

    if(!fFound)
    {
      printf("Invalid option '%s'\r\n", argv[iArg]);
      return false;
    }
  }
  
  return true;
}
 
//=============================================================================
// MAIN program
//
//
//=============================================================================
int main(int argc, char* argv[])
{
  CIFX_LINUX_INIT    tInit       = {0};
  
  printf("********************************************************************************\r\n");
  printf("*** cifX Driver Test Program \r\n");
  printf("********************************************************************************\r\n\n");
 
  sprintf( g_szFirmware, "%s", DEFAULT_FIRMWARE);
  sprintf( g_szConfig,   "%s", DEFAULT_CONFIG);
  
  //==============================================================
	// Get program parameter
  //==============================================================	
  if ( GetProgramParameters( argc, argv) == false)
  {
    ShowProgramHelp( NULL, NULL);
    return 0;  
  }
  /* set to default values */
  tInit.init_options        = CIFX_DRIVER_INIT_AUTOSCAN;
  tInit.iCardNumber         = 0;
  tInit.fEnableCardLocking  = 0;
  tInit.base_dir            = NULL;
  tInit.poll_interval       = 0;
  tInit.poll_StackSize      = 0;
  tInit.trace_level         = 255;
  tInit.user_card_cnt       = 0; 
 
  printf("cifXDriverInit...\n");
  /* First of all initialize toolkit */
  int32_t lRet = cifXDriverInit(&tInit);
 
  if (lRet == CIFX_NO_ERROR)
  {
    /* run demo application */
    RunCifXConsoleTest( g_szBoard,
                        g_szFirmware,
                        g_szConfig,
                        1,
                        10);
  } else
  {
	printf( "Error initializing toolkit! Error: 0x%08X", lRet);

  }
  cifXDriverDeinit();
 
}
  
