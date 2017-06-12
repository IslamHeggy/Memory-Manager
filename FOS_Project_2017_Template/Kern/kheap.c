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
   LIST_FOREACH(bi,&Block_info_taken_list)
    {
    	if(bi->start_va==(uint32)virtual_address)
    	{
           for(int j=0;j<bi->size/PAGE_SIZE;j++)
           {
             unmap_frame(ptr_page_directory,(void *)virtual_address);
             virtual_address+=PAGE_SIZE;
           }
    	   break;
    	}
    }
   if(bi!=NULL)
   {
	   LIST_REMOVE(&Block_info_taken_list,bi);
	   Coalescing_heap(bi);
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
	panic("krealloc() is not implemented yet...!!");
    void *ptr=kmalloc(new_size);
    if(ptr==0)
    {
    	return NULL;
    }
    kfree(virtual_address);
	return ptr;
}
void Coalescing_heap(struct Block_Info *take)
{
	struct Block_Info *curr;
	int found=0;
	LIST_FOREACH(curr,&Block_info_free_list)
	{
	if(take->start_va + take->size==curr->start_va)
	{
		curr->start_va=take->start_va;
	    curr->size+=take->size;
	    found=1;
	    break;
	}
	else if(curr->start_va+curr->size==take->start_va)
	{
		curr->size+=take->size;
		found=1;
	    break;
	}
	}
	if(found)
	{
		LIST_REMOVE(&Block_info_free_list,curr);
		Coalescing_heap(curr);
	}
	if(!found)
	{
		if(isKHeapPlacementStrategyNEXTFIT())
		{
			if(take->start_va<last_va_in_kheap)
			    {
			    last_va_in_kheap=take->start_va;
			    }
		}
		LIST_INSERT_HEAD(&Block_info_free_list,take);
	}
}
void *cont_allocation(unsigned int size)
{
size=ROUNDUP(size,PAGE_SIZE);
if(size<KERNEL_HEAP_MAX-last_va_in_kheap+1 && curr_number_of_blocks<Max_number_of_blocks)
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
          blocks_info[curr_number_of_blocks].size=size;
          blocks_info[curr_number_of_blocks].start_va=tmp;
          LIST_INSERT_HEAD(&Block_info_taken_list,&blocks_info[curr_number_of_blocks]);
          curr_number_of_blocks++;
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
				if(found==NULL || bi->start_va<found->start_va)
				{
					found=bi;
				}
			}
		}
		if(found==NULL || curr_number_of_blocks==Max_number_of_blocks)
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
	      last_va_in_kheap+=size;
	      blocks_info[curr_number_of_blocks].size=size;
	      blocks_info[curr_number_of_blocks].start_va=tmp;
	      LIST_INSERT_HEAD(&Block_info_taken_list,&blocks_info[curr_number_of_blocks]);
	      curr_number_of_blocks++;
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
                	  }
                	  else if(after->start_va>=bi->start_va)
                	  {
                		  after=bi;
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
			if((after==NULL&&before==NULL) || curr_number_of_blocks==Max_number_of_blocks)
			{
		         return 0;
			}
			uint32 tmp;
			if(after!=NULL)
			{
				tmp=after->start_va;
				after->start_va+=size;
				after->size-=size;
				last_va_in_kheap=tmp;
			}
			else
			{
				tmp=before->start_va;
				before->start_va+=size;
				before->size-=size;
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
		      blocks_info[curr_number_of_blocks].size=size;
		      blocks_info[curr_number_of_blocks].start_va=tmp;
		      LIST_INSERT_HEAD(&Block_info_taken_list,&blocks_info[curr_number_of_blocks]);
		      curr_number_of_blocks++;
		      return (void *)tmp;
}
