/* contains stuff to be added to other modules.*/


struct IOD
{
	char name[NAME_LENGTH];
	PCB *requestor;
	int request;
	char *tran_buff;
	int *buff_count;
	struct IOD *next;
}IOD;


struct IOCB
{	int event_flag;
	int count;
	struct IOD *head;
	struct IOD *tail;
}IOCB;

struct IOCB com_queue;
struct IOCB term_queue;

int IOschedule()
{
	int retq = 0;
	int device_id = param_p->device_id;
	struct *newIOD = createIOD();

	if(device_id = COM){
		retq = enqueue(newIOD,com_queue);
		if(retq == 1){
			switch(IOD->request){
				{case READ:
					//com_read
					break;}
			case WRITE:{
					//com_write
					break;}
			case CLEAR:{
				    //com_clear
					break;}
			case GOTOXY:{
				    //com_cotoxy
					break;}
			default:{
					return ERR_UNKN_REQUEST}
			}
		}
	}
	else if(device_id = TERM)
	{
				retq = enqueue(newIOD,com_queue);
		if(retq == 1){
			switch(IOD->request){
			case READ:{
					//trm_read
					break;}
			case WRITE:{
					//trm_write
					break;}
			case CLEAR:{
				    //trm_clear
					break;	  }
			case GOTOXY:{
				    //trm_cotoxy
					break;}
			default:{
					return ERR_UNKN_REQUEST}
			}
	}
	else
		return ERR_UNKN_DEVICE;

	  cop->state = BLOCKED;
	  insert(cop, BLOCKED);

	  return OK;

}
int enqueue(struct *IOD nIOD, struct *IOCB queue)
{
	int retv = 0;

	if(queue->count = 0)
	{
		queue->head = nIOD;
		queue->tail = nIOD;
		queue->count = queue->count++;
		retv = 1;
	}
	else
	{
		queue->tail->next = nIOD;
		queue->tail = nIOD;
		queue->count = queue->count++;
	}
	return retv;
}



struct *IOD createIOD()
{
	struct IOD *newIOD = NULL;
	newIOD = sys_alloc_mem((sizeof(struct PCB)));
	newIOD->name = cop->name;
	newIOD->requestor = cop;
	newIOD->tran_buff = param_p->buf_addr;
	newIOD->count = param_p->count_addr;
	newIOD->request = param_p->op_code;
	return newIOD;
}