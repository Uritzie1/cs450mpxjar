/**
*/
#define FILE_LENGTH = 15;










void load_prog(char[FILE_LENGTH] fname, int pri){

	int offset_p;
	int progLength;
	struct PCB *newNode;
	unsigned char *load_address;
	unsigned char *excu_address;

	if(pri < -127 || pri > 127)
		return 1;
	//if NAME is invalid
	//	return NAME_NOT_UNIQUE

	err = sys_check_program("/0",fname,&progLength,&offset_p)
	if(err < OK)
		return err;
	if(NULL == (newNode = allocate_PCB)
		return err;

	create_PCB(newNode,fname,APPLICATION,pri);
	newNode->suspended = 1;

	newNode->load_address = (unsigned char*)sys_alloc_mem(progLength);
	newNode->execution_address = load_address + offset_p;

	//contect cp* = (context *)pcb_>stack_top;
	//IP = FP_OFF(execution_address);
	//CS = FP_SEG(execution_address);

	//DS = _DS;
	//ES = _ES;

	//flags = 0x200;
	int Terminate()
	{
		delete_PCB();
	}








}