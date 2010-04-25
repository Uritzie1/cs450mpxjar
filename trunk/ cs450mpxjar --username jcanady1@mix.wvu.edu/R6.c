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

#define COMHAN_STACK_SIZE 4096
#define ERR_UNKN_REQUEST (-212)

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
struct IOCB *com_queue;
struct IOCB *term_queue;


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
  if (np == NULL) err = ERR_UCPCB;
  else {
    err = setup_PCB(np, "comhan",SYSTEM,127);
    if (err < OK) return err;
   	sys_free_mem(np->stack_base);
	np->stack_base = (unsigned char*)sys_alloc_mem(COMHAN_STACK_SIZE*sizeof(unsigned char));
	np->stack_top = np->stack_base + COMHAN_STACK_SIZE - sizeof(struct context);
    npc = (struct context*) np->stack_top;
    npc->IP = FP_OFF(&comhan); 
    npc->CS = FP_SEG(&comhan);
    npc->FLAGS = 0x200;
    npc->DS = _DS;
    npc->ES = _ES;
    err = insert(np,RUNNING);
  }
  
  load_prog("IDLE", -128, SYSTEM);
  temppcb = findPCB(buff, temppcb);
  temppcb->suspended = NOTSUSP;
  
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
					com_read((com_queue->head)->tran_buff, (com_queue->head)->buff_count);
					break;}
			case WRITE: {
					com_write((com_queue->head)->tran_buff, (com_queue->head)->buff_count);
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
					trm_read((com_queue->head)->tran_buff, (com_queue->head)->buff_count);
					break;}
			case WRITE: {
					trm_write((com_queue->head)->tran_buff, (com_queue->head)->buff_count);
					break;}
			case CLEAR: {
				    trm_clear();
					break;}
			case GOTOXY: {
				    trm_gotoxy();
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
		queue->count++;
		retv = 1;
	}
	else {
		queue->tail->next = nIOD;
		queue->tail = nIOD;
		queue->count++;
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
