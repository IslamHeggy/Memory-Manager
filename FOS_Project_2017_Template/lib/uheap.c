#include <inc/lib.h>
#define Max_number_of_blocks_user 100000
struct Linked_List_block Block_info_free_list_user;
struct Linked_List_block Block_info_taken_list_user;
struct Block_Info blocks_info_user[Max_number_of_blocks_user];
struct Linked_List_block free_blocks_user;
uint8 first_time=1;

// malloc()
//	This function use FIRST FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

void* malloc(uint32 size)
{
	//TODO: [PROJECT 2017 - [5] User Heap] malloc() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");

	// Steps:
	//	1) Implement FIRST FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	//	2) if no suitable space found, return NULL
	//	 Else,
	//	3) Call sys_allocateMem to invoke the Kernel for allocation
	// 	4) Return pointer containing the virtual address of allocated space,
	//

	//This function should find the space of the required range
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyFIRSTFIT() to check the current strategy

	//change this "return" according to your answer
    size=ROUNDUP(size,PAGE_SIZE);
    void * space=first_fit_user(size);
    if(space==NULL)
    	return NULL;
    sys_allocateMem((uint32)space,size);
    return space;
}
void* first_fit_user(uint32 size)
{
	 if(first_time)
	    {
		    LIST_INIT(&Block_info_free_list_user);
	    	LIST_INIT(&Block_info_taken_list_user);
	    	LIST_INIT(&free_blocks_user);
		 for(int i=0;i<Max_number_of_blocks_user;i++)
		 {
             LIST_INSERT_HEAD(&free_blocks_user,&blocks_info_user[i]);
		 }
		    struct Block_Info *ptr=NULL;
		    allocate_block(&ptr);
	    	first_time=0;

	    	ptr->start_va=USER_HEAP_START;
	    	ptr->size=USER_HEAP_MAX-USER_HEAP_START;
	    	LIST_INSERT_HEAD(&Block_info_free_list_user,ptr);
	    }
	        size=ROUNDUP(size,PAGE_SIZE);
			struct Block_Info *bi=NULL;
			struct Block_Info* found=NULL;
			LIST_FOREACH(bi,&Block_info_free_list_user)
			{
				if(bi->size>=size)
				{
					found=bi;
					break;
				}
			}

			if(found==NULL)
			{
				return NULL;
			}
			  uint32 tmp=found->start_va;
		      found->start_va+=size;
		      found->size-=size;
		      if(found->size==0)
		     	      {
		     	    	  LIST_REMOVE(&Block_info_free_list_user,found);
		     	    	  found->size=0;
		     	    	  found->start_va=0;
		     	    	  found->prev_next_info.le_next=(void *)0;
		     	    	  found->prev_next_info.le_prev=(void *)0;
		     	          LIST_INSERT_HEAD(&free_blocks_user,found);
		     	      }
		      struct Block_Info *ptr=NULL;
		      allocate_block(&ptr);
		      ptr->size=size;
		      ptr->start_va=tmp;
		      LIST_INSERT_HEAD(&Block_info_taken_list_user,ptr);
		      return (void *)tmp;
}
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//TODO: [PROJECT 2017 - [6] Shared Variables: Creation] smalloc() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("smalloc() is not implemented yet...!!");

	// Steps:
	//	1) Implement FIRST FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	//	2) if no suitable space found, return NULL
	//	 Else,
	//	3) Call sys_createSharedObject(...) to invoke the Kernel for allocation of shared variable
	//		sys_createSharedObject(): if succeed, it returns the ID of the created variable. Else, it returns -ve
	//	4) If the Kernel successfully creates the shared variable, return its virtual address
	//	   Else, return NULL

	//This function should find the space of the required range
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyFIRSTFIT() to check the current strategy

	//change this "return" according to your answer

	if(sys_isUHeapPlacementStrategyFIRSTFIT())
	{
		size=ROUNDUP(size,PAGE_SIZE);
		void * Result=first_fit_user(size);
			if (Result!=NULL)
			{
				int Shared_ID=sys_createSharedObject(sharedVarName,size,isWritable,Result);
				if(Shared_ID >=0)
				{
					return Result;
				}
			}
		//change this "return" according to your answer
		return NULL;
	}
	return NULL;

}
void* sget(int32 ownerEnvID, char *sharedVarName)
{
	//TODO: [PROJECT 2017 - [6] Shared Variables: Get] sget() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("sget() is not implemented yet...!!");

	// Steps:
	//	1) Get the size of the shared variable (use sys_getSizeOfSharedObject())
	//	2) If not exists, return NULL
	//	3) Implement FIRST FIT strategy to search the heap for suitable space
	//		to share the variable (should be on 4 KB BOUNDARY)
	//	4) if no suitable space found, return NULL
	//	 Else,
	//	5) Call sys_getSharedObject(...) to invoke the Kernel for sharing this variable
	//		sys_getSharedObject(): if succeed, it returns the ID of the shared variable. Else, it returns -ve
	//	6) If the Kernel successfully share the variable, return its virtual address
	//	   Else, return NULL
	//

	//This function should find the space for sharing the variable
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyFIRSTFIT() to check the current strategy

	//change this "return" according to your answer
	if(sys_isUHeapPlacementStrategyFIRSTFIT())
			{
				int check=sys_getSizeOfSharedObject(ownerEnvID,sharedVarName);
				if(check==E_SHARED_MEM_NOT_EXISTS)
					return NULL;
				void * Result=first_fit_user((uint32)check);
				if(Result!=NULL)
				{
					int Shared_ID=sys_getSharedObject(ownerEnvID,sharedVarName,Result);

					if(Shared_ID>=0)
						return Result;
				}
			//change this "return" according to your answer
			}
		return NULL;

}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{
	//TODO: [PROJECT 2017 - [5] User Heap] free() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("free() is not implemented yet...!!");
	   struct Block_Info* bi=NULL;
	   LIST_FOREACH(bi,&Block_info_taken_list_user)
	   {
	   if(bi!=NULL && bi->start_va==(uint32)virtual_address)
		{
          sys_freeMem((uint32)virtual_address,bi->size);
         LIST_REMOVE(&Block_info_taken_list_user,bi);
         Coalescing_heap_user(bi);
             return;
		 }
	    }
	//you should get the size of the given allocation using its address
	//you need to call sys_freeMem()
	//refer to the project presentation and documentation for details
}
void Coalescing_heap_user(struct Block_Info *take)
{
	struct Block_Info *curr=NULL;
	struct Block_Info *nex=NULL;
	struct Block_Info *pre=NULL;
	struct Block_Info *str=LIST_FIRST(&Block_info_free_list_user);
	struct Block_Info *end=LIST_LAST(&Block_info_free_list_user);
    if(str->start_va >=take->start_va && take->start_va+take->size==str->start_va)
    {
    	str->start_va=take->start_va;
    	str->size+=take->size;
    	take->size=0;
    	take->start_va=0;
    	take->prev_next_info.le_next=(void *)0;
    	take->prev_next_info.le_prev=(void *)0;
    	LIST_INSERT_HEAD(&free_blocks_user,take);
    	return;
    }
    if(take->start_va >=end->start_va)
        {
    	   if(end->start_va+end->size==take->start_va)
    	   {
    	   end->size+=take->size;
        	take->size=0;
        	take->start_va=0;
        	take->prev_next_info.le_next=(void *)0;
        	take->prev_next_info.le_prev=(void *)0;
        	LIST_INSERT_HEAD(&free_blocks_user,take);
    	   }
    	   else
    	   {
    		   insert_sorted_user(&Block_info_free_list_user,take);
    	   }
        	return;
        }
	LIST_FOREACH(curr,&Block_info_free_list_user)
	{
     if(curr->start_va<take->start_va)
     {
    	 pre=curr;
     }
     else
     {
    	 nex=curr;
    	 break;
     }
	}
	if(nex!=NULL &&pre!=NULL &&  take->start_va + take->size==nex->start_va && pre->start_va+pre->size==take->start_va)
	{
	   pre->size+=(take->size+nex->size);
	   LIST_REMOVE(&Block_info_free_list_user,nex);
	   take->size=nex->size=0;
	   take->start_va=nex->start_va=0;
	   take->prev_next_info.le_next=nex->prev_next_info.le_next=(void *)0;
	    take->prev_next_info.le_prev=nex->prev_next_info.le_prev=(void *)0;
	    LIST_INSERT_HEAD(&free_blocks_user,take);
	    LIST_INSERT_HEAD(&free_blocks_user,nex);
	}
	else if(nex!=NULL && take->start_va + take->size==nex->start_va)
		{
			nex->start_va=take->start_va;
		    nex->size+=take->size;
		    take->size=0;
		    take->start_va=0;
		    take->prev_next_info.le_next=(void *)0;
		    take->prev_next_info.le_prev=(void *)0;
		    LIST_INSERT_HEAD(&free_blocks_user,take);
		}
		else if(pre!=NULL && pre->start_va+pre->size==take->start_va)
		{
			pre->size+=take->size;
			take->size=0;
			take->start_va=0;
			take->prev_next_info.le_next=(void *)0;
			take->prev_next_info.le_prev=(void *)0;
			LIST_INSERT_HEAD(&free_blocks_user,take);
		}
		else
		{
			insert_sorted_user(&Block_info_free_list_user,take);
		}
}
void insert_sorted_user(struct Linked_List_block *curr_list,struct Block_Info *elem)
{
	    struct Block_Info *pre=NULL;
		struct Block_Info *nex=NULL;
		struct Block_Info *cur=NULL;
		struct Block_Info * str=LIST_FIRST(curr_list);
		struct Block_Info * end=LIST_LAST(curr_list);
		if(elem->start_va <=str->start_va)
		{
			LIST_INSERT_BEFORE(curr_list,str,elem);
			return;
		}
		if(elem->start_va >=end->start_va)
		{
			LIST_INSERT_AFTER(curr_list,end,elem);
			return;
		}
	    LIST_FOREACH(cur,curr_list)
		{
	    	if(cur->start_va<=elem->start_va)
	    	{
	    		pre=cur;
	    	}
	    	else
	    	{
	    		nex=cur;
	    		break;
	    	}
		}
	    if(nex==NULL && pre==NULL)
	    {
	    	LIST_INSERT_HEAD(curr_list,elem);
	    }
	    else if(pre!=NULL)
	    {
	    	LIST_INSERT_AFTER(curr_list,pre,elem);
	    }
	    else if(nex!=NULL)
	    {
	    	LIST_INSERT_BEFORE(curr_list,nex,elem);
	    }
}
//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=============
// [1] sfree():
//=============
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	//TODO: [PROJECT 2017 - BONUS4] Free Shared Variable [User Side]
	// Write your code here, remove the panic and write your code
	//panic("sfree() is not implemented yet...!!");

	//	1) you should find the ID of the shared variable at the given address
	//	2) you need to call sys_freeSharedObject()
    sys_freeSharedObject(0,virtual_address);
}


//===============
// [2] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2017 - BONUS3] User Heap Realloc [User Side]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");
	new_size=ROUNDUP(new_size,PAGE_SIZE);
	if(virtual_address==NULL)
	   {
		   return malloc(new_size);
	   }
	   if(new_size==0)
	   {
		   free(virtual_address);
		   return NULL;
	   }
	   unsigned int old_size;
	      struct Block_Info *bi=NULL;
	      struct Block_Info *old_node=NULL;
	      	   LIST_FOREACH(bi,&Block_info_taken_list_user)
	      	   {
	      		   if(bi->start_va==(uint32)virtual_address)
	      		   {
	      			   old_size=bi->size;
	      			   old_node=bi;
	      			   break;
	      		   }
	      	   }
	   	   if(old_size>=new_size)
	   	   {
	   		   return virtual_address;
	   	   }
	     struct Block_Info *curr=NULL;
	     struct Block_Info *nex=NULL;
	     struct Block_Info *pre=NULL;
	      LIST_FOREACH(curr,&Block_info_free_list_user)
	     {
	   	   if(curr->start_va<=(uint32)virtual_address)
	   	   {
	   		   pre=curr;
	   	   }
	   	   else
	   	   {
	   		   nex=curr;
	   		   break;
	   	   }
	     }
	     if(nex!=NULL && old_node->start_va+old_size==nex->start_va && old_size+nex->size >=new_size)
	     {
	   	  old_node->size=new_size;
	   	  sys_allocateMem(nex->start_va,new_size-old_size);
	   	  nex->size-=(new_size-old_size);
	   	  nex->start_va+=(new_size-old_size);
	   	  if(nex->size==0)
	   	  {
	   	 LIST_REMOVE(&Block_info_free_list_user,nex);
	   	 nex->size=0;
	   	 nex->start_va=0;
	   	nex->prev_next_info.le_next=(void *)0;
	   	nex->prev_next_info.le_prev=(void *)0;
	   	 LIST_INSERT_HEAD(&free_blocks_user,nex);
	   	  }
	   	  return virtual_address;
	     }
	     void*  res=malloc(new_size);
	        if(res==NULL)
	        {
	     	   return NULL;
	        }
	        else
	        {
	          sys_moveMem((uint32)virtual_address,(uint32)res,old_size);
	          sys_allocateMem((uint32)res+old_size,new_size-old_size);
	            return res;
	        }
}
void allocate_block(struct Block_Info**ptr)
{
	*ptr=LIST_FIRST(&free_blocks_user);
	 if(*ptr==NULL)
	 panic("Error no free blocks !!!!");
	LIST_REMOVE(&free_blocks_user,*ptr);
}
