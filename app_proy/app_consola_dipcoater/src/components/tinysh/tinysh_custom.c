#include <stdio.h>
//#if _EPITKL_CFG_ARCH__x86 == _EPITKL_CFG_ARCH
//#include <errno.h>
//#include <termios.h>
//#include <unistd.h>
//#include <fcntl.h>
//#endif
#include "include/tinysh.h"
#include  "../api/include/command.h"

//#if _EPITKL_CFG_ARCH__x86 == _EPITKL_CFG_ARCH
/*--- stuff for single character input entry --------------------------------*/
//static struct termios saved_term_state;

/* remove standard input buffering to simulate minimal input capability
 * like read from the uart in an embedded environment
 */
//int set_cbreak (int fd)
//{
//	struct termios temp_term_state;
//	/* get current terminal state */
//	if (tcgetattr (fd, &saved_term_state) < 0)
//	{
//		perror ("could not get terminal state : ");
//		return (errno);
//	}
//	temp_term_state = saved_term_state;
//	temp_term_state.c_lflag &= ~(ECHO | ICANON);
//	temp_term_state.c_cc [VMIN] = 1;    /* return after 1 byte min */
//	temp_term_state.c_cc [VTIME] = 0;   /* wait forever for data */
//	/* set terminal state to cbreak */
//	if (tcsetattr (fd, TCSAFLUSH, &temp_term_state) < 0)
//	{
//		perror ("could not set terminal state : ");
//		return (errno);
//	}
//	return (0);
//}
//
//static int unset_cbreak(int fd)
//{
//	if (tcsetattr (fd, TCSAFLUSH, &saved_term_state) < 0)
//	{
//		perror ("could not set terminal state : ");
//		return (errno);
//	}
//	return 0;
//}
//#endif

putchar_handler_t putcharHandler = NULL;
/*MOVEMENT COMMANDS*/
//PROCESS STANDARD:
static tinysh_cmd_t commandSETSTANDARDPROGRAM = 		{NULL,"SETSTANDARDPROGRAM", NULL, NULL, CommandSETSTANDARDPROGRAMHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandLOADPROGRAMSTANDARD = 		{NULL,"LOADPROGRAMSTANDARD", NULL, NULL, CommandLOADPROGRAMSTANDARDHandler, NULL, NULL, NULL};
//PROCESS CUSTOM:
static tinysh_cmd_t commandSETCOMMANDCUSTOMPROGRAM = 	{NULL,"SETCUSTOMPROGRAM", NULL, NULL, CommandSETCOMMANDCUSTOMPROGRAMHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSETALLCUSTOMPROGRAM = 		{NULL,"SETALLCUSTOMPROGRAM", NULL, NULL, CommandSETALLCUSTOMPROGRAMHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandLOADPROGRAMCUSTOM = 			{NULL,"LOADPROGRAMCUSTOM", NULL, NULL, CommandLOADPROGRAMCUSTOMHandler, NULL, NULL, NULL};
//PROCESS DINAMIC:
static tinysh_cmd_t commandCLEANPROGRAMDINAMIC = 		{NULL,"CLEANPROGRAMDINAMIC", NULL, NULL, CommandCLEANPROGRAMDINAMICHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandADDSETALLCOMANDDINAMIC = 	{NULL,"ADDSETALLCOMANDDINAMIC", NULL, NULL, CommandADDSETALLCOMANDDINAMICHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandLOADPROGRAMDINAMIC = 		{NULL,"LOADPROGRAMDINAMIC", NULL, NULL, CommandLOADPROGRAMDINAMICHandler, NULL, NULL, NULL};
//RUN LOADED PROCESS:
static tinysh_cmd_t commandRUN = 						{NULL,"RUN", NULL, NULL, CommandRUNHandler, NULL, NULL, NULL};
//SINGLE MOVEMENTS:
static tinysh_cmd_t commandUPFAST = 					{NULL,"UPFAST", NULL, NULL, CommandUPFASTHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandUP = 						{NULL,"UP", NULL, NULL, CommandUPHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandUPSLOW = 					{NULL,"UPSLOW", NULL, NULL, CommandUPSLOWHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDOWNFAST = 					{NULL,"DOWNFAST", NULL, NULL, CommandDOWNFASTHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDOWN = 						{NULL,"DOWN", NULL, NULL, CommandDOWNHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDOWNSLOW = 					{NULL,"DOWNSLOW", NULL, NULL, CommandDOWNSLOWHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSTOP = 						{NULL,"STOP", NULL, NULL, CommandSTOPHandler, NULL, NULL, NULL};
/*CONFIGURATION COMMANDS*/
static tinysh_cmd_t commandSAMPLE = 					{NULL,"LOADSAMPLE", NULL, NULL, commandSAMPLEHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandRECIPIENT = 					{NULL,"LOADRECIPIENT", NULL, NULL, commandRECIPIENTMHandler, NULL, NULL, NULL};

//ENVIROMENTAL CHAMBER:
static tinysh_cmd_t commandSETRH = 						{NULL,"SETRH", NULL, NULL, CommandSETRHHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSETTEMP = 					{NULL,"SETTEMP", NULL, NULL, CommandSETTEMPHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandACTIVATEENVIROMENTALCHAMBER ={NULL,"ACTIVATEENVIROMENTALCHAMBER", NULL, NULL, CommandACTIVATEENVIROMENTALCHAMBERHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDEACTIVATEENVIROMENTALCHAMBER ={NULL,"DEACTIVATEENVIROMENTALCHAMBER", NULL, NULL, CommandDEACTIVATEENVIROMENTALCHAMBERHandler, NULL, NULL, NULL};

void tinysh_set_putchar(putchar_handler_t handler){
	putcharHandler = handler;
}

void tinysh_char_out(unsigned char c)
{
	if(NULL != putcharHandler){
		putcharHandler(c);
	}
}

/*AGREGADO ---> Seteo de comandos , inicializacion y HandlerConsolePutchar*/

void HandlerConsolePutchar(unsigned char c){
	putchar(c);
}

void tinysh_init() {

	//command initialization
	tinysh_add_command(&commandLOADPROGRAMSTANDARD);
	tinysh_add_command(&commandLOADPROGRAMCUSTOM);
	tinysh_add_command(&commandLOADPROGRAMDINAMIC);
	tinysh_add_command(&commandSETCOMMANDCUSTOMPROGRAM);
	tinysh_add_command(&commandSETSTANDARDPROGRAM);
	tinysh_add_command(&commandSETALLCUSTOMPROGRAM);
	tinysh_add_command(&commandCLEANPROGRAMDINAMIC);
	tinysh_add_command(&commandADDSETALLCOMANDDINAMIC);
	tinysh_add_command(&commandRUN);
	tinysh_add_command(&commandUPFAST);
	tinysh_add_command(&commandUP);
	tinysh_add_command(&commandUPSLOW);
	tinysh_add_command(&commandDOWNFAST);
	tinysh_add_command(&commandDOWN);
	tinysh_add_command(&commandDOWNSLOW);
	tinysh_add_command(&commandSTOP);
	tinysh_add_command(&commandSAMPLE);
	tinysh_add_command(&commandRECIPIENT);
	tinysh_add_command(&commandSETRH);
	tinysh_add_command(&commandSETTEMP);
	tinysh_add_command(&commandACTIVATEENVIROMENTALCHAMBER);
	tinysh_add_command(&commandDEACTIVATEENVIROMENTALCHAMBER);


	tinysh_set_putchar(HandlerConsolePutchar);

	//	set_cbreak(0);
}

void tinysh_finish() {
//	unset_cbreak(0);
}

