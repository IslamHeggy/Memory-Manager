#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
//=================================================================================//
//============================ REQUIRED FUNCTION ==================================//
//=================================================================================//
void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT 2017 - [1] Kernel Heap] kmalloc()
	// Write your code here, remove the panic and write your code
	//panic("kmalloc() is not implemented yet...!!");
	//NOTE: Allocation is continuous increasing virtual address
	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details
	//TODO: [PROJECT 2017 - BONUS1] Implement a Kernel allocation strategy
	// Instead of the continuous allocation/deallocation, implement both
	// FIRST FIT and NEXT FIT strategies
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy
	if(isKHeapPlacementStrategyCONTALLOC())
	{
		return cont_allocation(size);
	}
	else if(isKHeapPlacementStrategyFIRSTFIT())
	{
		return first_fit_allocation(size);
	}
	else if(isKHeapPlacementStrategyNEXTFIT())
	{
		return next_fit_allocation(size);
	}

return NULL;
}
void kfree(void* virtual_address)
{
	//TODO: [PROJECT 2017 - [1] Kernel Heap] kfree()
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");

	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details
	struct Block_Info* bi=NULL;
	uint32 tmp=(uint32)virtual_address;
   LIST_FOREACH(bi,&Block_info_taken_list)
    {
    	if(bi->start_va==tmp)
    	{
           for(int j=0;j<bi->size/PAGE_SIZE;j++)
           {
             unmap_frame(ptr_page_directory,(void *)tmp);
             tmp+=PAGE_SIZE;
           }
        LIST_REMOVE(&Block_info_taken_list,bi);
	   Coalescing_heap(bi);
    	   break;
    	}
    }
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT 2017 - [1] Kernel Heap] kheap_virtual_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");
	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details
	//change this "return" according to your answers
	if((&frames_info[PPN(physical_address)])==0)return 0;
	return ((&frames_info[PPN(physical_address)])->va)+(physical_address&0x00000FFF);
}
unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT 2017 - [1] Kernel Heap] kheap_physical_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer
	uint32 *ptr=NULL;
	get_page_table(ptr_page_directory,(void *)virtual_address,&ptr);
	return ((ptr[PTX(virtual_address)]>>12)*PAGE_SIZE)+(virtual_address&0x00000FFF);
}


//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().
uint32 last_va_in_kheap=KERNEL_HEAP_START;
void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2017 - BONUS2] Kernel Heap Realloc
	// Write your code here, remove the panic and write your code
	// comment it when test the krealloc
	return NULL;
	//panic("krealloc() is not implemented yet...!!");

	new_size=ROUNDUP(new_size,PAGE_SIZE);
	if(virtual_address==NULL)
   {
	   return kmalloc(new_size);
   }
   if(new_size==0)
   {
	   kfree(virtual_address);
	   return NULL;
   }
   unsigned int old_size;
   struct Block_Info *bi=NULL;
   struct Block_Info *old_node=NULL;
   	   LIST_FOREACH(bi,&Block_info_taken_list)
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
   LIST_FOREACH(curr,&Block_info_free_list)
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
	  nex->size-=(new_size-old_size);
	  nex->start_va+=(new_size-old_size);
	  if(nex->size==0)
	  {
	 LIST_REMOVE(&Block_info_free_list,nex);
	 nex->size=0;
	 nex->start_va=0;
	nex->prev_next_info.le_next=(void *)0;
	nex->prev_next_info.le_prev=(void *)0;
	 LIST_INSERT_HEAD(&free_blocks,nex);
	  }
	  uint32 tmp=(uint32)virtual_address;
	  for(int i=0;i<new_size/PAGE_SIZE;i++)
	  	{
		  uint32 *ptr;
		  if(get_frame_info(ptr_page_directory,(void *)tmp,&ptr) ==0)
		  {
			struct Frame_Info*fi=NULL;
	  	    allocate_frame(&fi);
	        map_frame(ptr_page_directory,fi,(void *)tmp,PERM_WRITEABLE);
		  }
	        tmp+=PAGE_SIZE;
	  	 }
	  return virtual_address;
  }
	void*  res=kmalloc(new_size);
   if(res==NULL)
   {
	   return NULL;
   }
   else
   {
	   uint32 num_of_pages=old_size/PAGE_SIZE;
	   unsigned char *src_va=(unsigned char *)virtual_address;
	   unsigned char *dst_va=(unsigned char *)res;
	  for(int i=0;i<num_of_pages;i++)
	  {
		  unmap_frame(ptr_page_directory,(void *)dst_va);
		  unmap_frame(ptr_page_directory,(void *)src_va);
		  struct Frame_Info*fi;
		  allocate_frame(&fi);
		  map_frame(ptr_page_directory,fi,(void * )dst_va,PERM_WRITEABLE);
		  src_va+=PAGE_SIZE;
		  dst_va+=PAGE_SIZE;
	  }
	  LIST_REMOVE(&Block_info_taken_list,old_node);
	  Coalescing_heap(old_node);
       return res;
   }
}
void  Coalescing_heap(struct Block_Info *take)
{
	struct Block_Info *curr=NULL;
	struct Block_Info *pre=NULL;
	struct Block_Info *nex=NULL;
	LIST_FOREACH(curr,&Block_info_free_list)
	{
	if(curr->start_va<=take->start_va)
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
		   LIST_REMOVE(&Block_info_free_list,nex);
		   take->size=nex->size=0;
		   take->start_va=nex->start_va=0;
		   take->prev_next_info.le_next=nex->prev_next_info.le_next=(void *)0;
		    take->prev_next_info.le_prev=nex->prev_next_info.le_prev=(void *)0;
		    LIST_INSERT_HEAD(&free_blocks,take);
		    LIST_INSERT_HEAD(&free_blocks,nex);
		}
		else if(nex!=NULL && take->start_va + take->size==nex->start_va)
			{
				nex->start_va=take->start_va;
			    nex->size+=take->size;
			    take->size=0;
			    take->start_va=0;
			    take->prev_next_info.le_next=(void *)0;
			    take->prev_next_info.le_prev=(void *)0;
			    LIST_INSERT_HEAD(&free_blocks,take);
			}
			else if(pre!=NULL && pre->start_va+pre->size==take->start_va)
			{
				pre->size+=take->size;
				take->size=0;
				take->start_va=0;
				take->prev_next_info.le_next=(void *)0;
				take->prev_next_info.le_prev=(void *)0;
				LIST_INSERT_HEAD(&free_blocks,take);
			}
			else
			{
				if(isKHeapPlacementStrategyNEXTFIT())
		           {
			if(take->start_va<last_va_in_kheap)
			    {
			    last_va_in_kheap=take->start_va;
			    }
		           }
				insert_sorted(&Block_info_free_list,take);
			}
}
void *cont_allocation(unsigned int size)
{
size=ROUNDUP(size,PAGE_SIZE);
if(size<=KERNEL_HEAP_MAX-last_va_in_kheap)
{
		uint32 tmp=last_va_in_kheap;
	      for(int i=0;i<size/PAGE_SIZE;i++)
	      {
	    	  struct Frame_Info*fi=NULL;
	    	  allocate_frame(&fi);
	    	  if(fi==NULL)
	    	  {
	    		  panic("No free frames is exist!!!");
	    	  }
	    	  map_frame(ptr_page_directory,fi,(void *)tmp,PERM_WRITEABLE);
	    	  tmp+=PAGE_SIZE;
	      }
	      tmp=last_va_in_kheap;
	      last_va_in_kheap+=size;
	      struct Block_Info *ptr;
	      k_allocate_block(&ptr);
          ptr->size=size;
          ptr->start_va=tmp;
          LIST_INSERT_HEAD(&Block_info_taken_list,ptr);
          return (void *)tmp;
}
else
{
	return NULL;
}
}
void *first_fit_allocation(unsigned int size)
{
	    size=ROUNDUP(size,PAGE_SIZE);
		struct Block_Info* bi;
		struct Block_Info* found=NULL;
		LIST_FOREACH(bi,&Block_info_free_list)
		{
			if(bi->size>=size)
			{
				found=bi;
				break;
			}
		}
		if(found==NULL )
		{
	         return 0;
		}
		uint32 tmp=found->start_va;
	      for(int i=0;i<size/PAGE_SIZE;i++)
	      {
	    	  struct Frame_Info*fi=NULL;
	    	  allocate_frame(&fi);
	    	  map_frame(ptr_page_directory,fi,(void *)tmp,PERM_WRITEABLE);
	    	  tmp+=PAGE_SIZE;
	      }
	      tmp=found->start_va;
	      found->start_va+=size;
	      found->size-=size;
	      if(found->size==0)
	      {
	    	  LIST_REMOVE(&Block_info_free_list,found);
	    	  found->size=0;
	    	  found->start_va=0;
	    	  found->prev_next_info.le_next=(void *)0;
	    	  found->prev_next_info.le_prev=(void *)0;
	          LIST_INSERT_HEAD(&free_blocks,found);
	      }
	      last_va_in_kheap+=size;
	      struct Block_Info*ptr=NULL;
	      k_allocate_block(&ptr);
	     ptr->size=size;
	      ptr->start_va=tmp;
	      insert_sorted(&Block_info_taken_list,ptr);
	      return (void *)tmp;
}
void *next_fit_allocation(unsigned int size)
{
	       size=ROUNDUP(size,PAGE_SIZE);
		   struct Block_Info* bi;
		   struct Block_Info* after=NULL;
		   struct Block_Info* before=NULL;
			LIST_FOREACH(bi,&Block_info_free_list)
			{
				if(bi->size>=size)
				{
                  if(bi->start_va>=last_va_in_kheap)
                  {
                	if(after==NULL)
                	  {
                		  after=bi;
                		  break;
                	  }
                	  else if(after->start_va>=bi->start_va)
                	  {
                		  after=bi;
                		  break;
                	  }
                  }
                  else
                 {
                    	  if(before==NULL)
                  {
                    		  before=bi;
                  }
                    	  else if(before->start_va>=bi->start_va)
                    	  {
                    		  before=bi;
                    	  }
                 }
				}
			}
			if((after==NULL&&before==NULL) )
			{
		         return 0;
			}
			uint32 tmp;
			if(after!=NULL)
			{
				tmp=after->start_va;
				after->start_va+=size;
				after->size-=size;
				 if(after->size==0)
					      {
					    	  LIST_REMOVE(&Block_info_free_list,after);
					    	  after->size=0;
					    	  after->start_va=0;
					    	  after->prev_next_info.le_next=(void *)0;
					    	  after->prev_next_info.le_prev=(void *)0;
					          LIST_INSERT_HEAD(&free_blocks,after);
					      }
				last_va_in_kheap=tmp;
			}
			else
			{
				tmp=before->start_va;
				before->start_va+=size;
				before->size-=size;
			if(before->size==0)
			{
			LIST_REMOVE(&Block_info_free_list,before);
			before->size=0;
			before->start_va=0;
			before->prev_next_info.le_next=(void *)0;
			before->prev_next_info.le_prev=(void *)0;
			LIST_INSERT_HEAD(&free_blocks,before);
			}
                last_va_in_kheap=tmp;
			}
			uint32 tmp1=tmp;
		      for(int i=0;i<size/PAGE_SIZE;i++)
		      {
		    	  struct Frame_Info*fi=NULL;
		    	  allocate_frame(&fi);
		    	  map_frame(ptr_page_directory,fi,(void *)tmp,PERM_WRITEABLE);
		    	  tmp+=PAGE_SIZE;
		      }
		      tmp=tmp1;
		      struct Block_Info*ptr=NULL;
		      k_allocate_block(&ptr);
		      ptr->size=size;
		      ptr->start_va=tmp;
		      LIST_INSERT_HEAD(&Block_info_taken_list,ptr);
		      return (void *)tmp;
}
void insert_sorted(struct Linked_List_block *curr_list,struct Block_Info *elem)
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
