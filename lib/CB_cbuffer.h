#ifndef CIRCULAR_FIFO_BUFFER
#define CIRCULAR_FIFO_BUFFER

struct cb_resource{
	void *rsc;
};

typedef struct cb_resource CB_HDL;

CB_HDL CB_create(int bufferSize, int elementByteSize);

unsigned char CB_isFull(CB_HDL *handle);

unsigned char CB_isEmpty(CB_HDL *handle);

unsigned char CB_addElement(CB_HDL *handle,const void *element);

unsigned char CB_getElement(CB_HDL *handle, void *element);

int CB_curSize(CB_HDL *handle);

void CB_clear(CB_HDL *handleTobeCleared);

#endif