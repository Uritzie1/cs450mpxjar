/**
 * File Name: R6.c
 * \title JAROS
 * \author Jonroy Canady
 * \author Adam Trainer
 * \author Rob Wayland   
 * \date 4/15/2010
 * \version: 6.0
 *
 * Components: 
 *
 *******************************************************************************
 * Change Log:
 *
 *        4/15/2010  JC,RW,AT   Preliminary Code
 */


//Structures
struct IOD {
	char name[NAME_LENGTH];
	PCB *requestor;
	int request;
	char *tran_buff;
	int *buff_count;
	struct IOD *next;
};

struct IOCB {	
    int event_flag;
	int count;
	struct IOD *head;
	struct IOD *tail;
};

//Globals
struct IOCB com_queue;
struct IOCB term_queue;


int main() {
  struct PCB *np;
  struct context *npc;
  
  sys_init(MODULE_R4);
  err = init_r1();
  err = init_r2();
  err = init_r3();
  err = init_f();
  com_open();
  
  np = allocate_PCB();
  if (np == NULL) err3 = ERR_UCPCB;
  else {
    err3 = setup_PCB(np, "test5",0,0);
    if (err3 < OK) return err3;
    npc = (struct context*) np->stack_top;
    npc->IP = FP_OFF(&COMHAN); 
    npc->CS = FP_SEG(&test5_R3);
    npc->FLAGS = 0x200;
    npc->DS = _DS;
    npc->ES = _ES;
    err3 = insert(np,RUNNING);
  }
  
  load_prog("IDLE.MPX", -127, SYS);
  dispatcher();
  
  com_close();
  err = cleanup_r1();
  err = cleanup_r2();
  err = cleanup_r3();
  err = cleanup_f();
  terminate_mpx();
  return 0;
}


int IOschedule() {
	int retq = 0;
	int device_id = param_p->device_id;
	struct *newIOD = createIOD();

	if(device_id = COM) {
		retq = enqueue(newIOD,com_queue);
		if(retq == 1) {
			switch(IOD->request) {
			case READ: {
					//com_read
					break;}
			case WRITE: {
					//com_write
					break;}
			case CLEAR: {
				    //com_clear
					break;}
			case GOTOXY: {
				    //com_cotoxy
					break;}
			default: {
					return ERR_UNKN_REQUEST;}
			}
		}
	}
	else if(device_id = TERM) {
	    retq = enqueue(newIOD,com_queue);
		if(retq == 1) {
			switch(IOD->request) {
			case READ: {
					//trm_read
					break;}
			case WRITE: {
					//trm_write
					break;}
			case CLEAR: {
				    //trm_clear
					break;	  }
			case GOTOXY: {
				    //trm_cotoxy
					break;}
			default: {
					return ERR_UNKN_REQUEST}
			}
	    }
    }
    else return ERR_UNKN_DEVICE;

    cop->state = BLOCKED;
    insert(cop, BLOCKED);
    return OK;
}


int enqueue(struct *IOD nIOD, struct *IOCB queue) {
	int retv = 0;

	if(queue->count = 0) {
		queue->head = nIOD;
		queue->tail = nIOD;
		queue->count = queue->count++;
		retv = 1;
	}
	else {
		queue->tail->next = nIOD;
		queue->tail = nIOD;
		queue->count = queue->count++;
	}
	return retv;
}


struct *IOD createIOD() {
	struct IOD *newIOD = NULL;
	newIOD = sys_alloc_mem((sizeof(struct PCB)));
	newIOD->name = cop->name;
	newIOD->requestor = cop;
	newIOD->tran_buff = param_p->buf_addr;
	newIOD->count = param_p->count_addr;
	newIOD->request = param_p->op_code;
	return newIOD;
}
