#include <nitro.h>

#include "btUtility.h"
#ifdef __cpluspluc
extern "C"
{
#endif
NNSFndHeapHandle	BT_SystemHeap; // System heap (Use extended heap)
NNSFndHeapHandle	BT_AppHeap;	// Application heap (expanded heap is used)
NNSFndAllocator	BT_allocator;		// Memory allocator
NNSFndAllocator	BT_gMicAllocator;  //Microphone memory allocator
static u32			BT_memory;
void				*BT_sysHeapMemory;
void				*BT_appHeapMemory;



const u32 align = 4;


#define BT_HEAP_ID 	((OSHeapHandle)0)
#define BT_ARENA_ID	((OSArenaId)OS_ARENA_MAIN)
#define BT_ROUND(n, a)     (((u32) (n) + (a) - 1) & ~((a) - 1))

#define BT_MAX_MONITORED_ALLOCBLOCKS 512 

u32 BT_u32ctrAllocatedBlocks = 0;
void * BT_apAllocatedBlockList[BT_MAX_MONITORED_ALLOCBLOCKS];
bool BT_bMemoryTrackingEnabled = 0; 
u32 BT_u32previousHeap; 

#define BT_BREAK_AT_BLOCK -10

void* operator new ( std::size_t blocksize ) 
{	
	SDK_ASSERT(NNS_FndGetAllocatableSizeForExpHeap(AppHeap) >= blocksize);
	if (BT_bMemoryTrackingEnabled)
	{
		if (BT_u32ctrAllocatedBlocks == BT_BREAK_AT_BLOCK)
		{
			OS_Printf("DEBUG point: check stack trace to identify");	
		}    
		
		//OS_Printf("ALLOC_ID<new>: %d\n", BT_u32ctrAllocatedBlocks);
	    BT_apAllocatedBlockList[BT_u32ctrAllocatedBlocks] = NNS_FndAllocFromExpHeapEx(BT_AppHeap, blocksize, align);//OS_AllocFromHeap( ARENA_ID, HEAP_ID, blocksize );
	    
	    BT_showMemory();
	    
	    return BT_apAllocatedBlockList[BT_u32ctrAllocatedBlocks++];	
	}
	else 
	{
		
		
		return NNS_FndAllocFromExpHeapEx(BT_AppHeap, blocksize, align);//OS_AllocFromHeap( ARENA_ID, HEAP_ID, blocksize );
	}
}

//----------------------------------------------------------------
//
void* operator new[] ( std::size_t blocksize )
{
	if (BT_bMemoryTrackingEnabled)
	{
		if (BT_u32ctrAllocatedBlocks == BT_BREAK_AT_BLOCK)
		{
			OS_Panic("DEBUG point: check stack trace to identify");	
		}
		
		//OS_Printf("ALLOC_ID<new[]>: %d\n", BT_u32ctrAllocatedBlocks);
	    BT_apAllocatedBlockList[BT_u32ctrAllocatedBlocks] = NNS_FndAllocFromExpHeapEx(BT_AppHeap, blocksize, align);//OS_AllocFromHeap( ARENA_ID, HEAP_ID, blocksize );
	    
	    BT_showMemory();
	    
	    return BT_apAllocatedBlockList[BT_u32ctrAllocatedBlocks++];	
	}
	else
	{
		return NNS_FndAllocFromExpHeapEx(BT_AppHeap, blocksize, align);//OS_AllocFromHeap( ARENA_ID, HEAP_ID, blocksize );
	}	
}

//----------------------------------------------------------------
//
void operator delete ( void* block ) throw()
{
	if (BT_bMemoryTrackingEnabled)
	{
		u32 u32ctrTemp = 0;
		
		while (block != BT_apAllocatedBlockList[u32ctrTemp]) u32ctrTemp++;		
		BT_apAllocatedBlockList[u32ctrTemp] = 0;
	}
	
	
	//OS_Printf("<delete>: %d\n", BT_u32ctrAllocatedBlocks);
	
	
	NNS_FndFreeToExpHeap(BT_AppHeap, block);
    
    BT_showMemory();
    
    //OS_FreeToHeap( ARENA_ID, HEAP_ID, block );
}

//---------------------------------------------------------------- 
//
void operator delete[] ( void* block ) throw()
{
	if (BT_bMemoryTrackingEnabled)
	{
		u32 u32ctrTemp = 0;
		
		while (block != BT_apAllocatedBlockList[u32ctrTemp]) u32ctrTemp++;		
		BT_apAllocatedBlockList[u32ctrTemp] = 0;	
	}
	
	//OS_Printf("<delete[]>: %d\n", BT_u32ctrAllocatedBlocks);
	
	NNS_FndFreeToExpHeap(BT_AppHeap, block);
    
    BT_showMemory();
    
    //OS_FreeToHeap( ARENA_ID, HEAP_ID, block );
}



void BT_initMemoryTracker(void)
{
	BT_u32ctrAllocatedBlocks = 0;
	
    for (int ictrAllocatedBlocks = 0; ictrAllocatedBlocks < BT_MAX_MONITORED_ALLOCBLOCKS; ictrAllocatedBlocks++)
	{
		BT_apAllocatedBlockList[ictrAllocatedBlocks] = 0;
	}
	
	BT_bMemoryTrackingEnabled = 1;
}

void BT_stopMemoryTracker(void)
{
	BT_u32ctrAllocatedBlocks = 0;
	
    for (int ictrAllocatedBlocks = 0; ictrAllocatedBlocks < BT_MAX_MONITORED_ALLOCBLOCKS; ictrAllocatedBlocks++)
	{
		BT_apAllocatedBlockList[ictrAllocatedBlocks] = 0;
	}
	
	BT_bMemoryTrackingEnabled = 0;
}

void BT_showMemory(void)
{
	u32 u32FreeTotal = NNS_FndGetTotalFreeSizeForExpHeap(BT_AppHeap);//OS_GetTotalFreeSize(ARENA_ID, HEAP_ID);
	u32 u32AllocTotal = NNS_FndGetAllocatableSizeForExpHeap(BT_AppHeap);//OS_GetTotalAllocSize(ARENA_ID, HEAP_ID);
	//OS_Printf("alloc/free: %d/%d\n", u32AllocTotal, u32FreeTotal);
}

u32 BT_getFreeMemory(void)
{
	BT_u32previousHeap = NNS_FndGetTotalFreeSizeForExpHeap(BT_AppHeap);//OS_GetTotalFreeSize(ARENA_ID, HEAP_ID);
	return BT_u32previousHeap;
}

bool BT_compareFreeMemory(void)
{
	return (BT_u32previousHeap < OS_GetTotalFreeSize(BT_ARENA_ID, BT_HEAP_ID));
}

//!
//!
//!
void BT_checkHeap(void)
{
	if (!BT_bMemoryTrackingEnabled) return;

	u32 u32ctrBlocksNotUnloaded = 0;
	
	if (BT_u32ctrAllocatedBlocks > 0)
	{ 
		
		//OS_Printf("\nIdentifying blocks still loaded... \n");			
		for (int ictrAllocatedBlocks = 0; ictrAllocatedBlocks < BT_MAX_MONITORED_ALLOCBLOCKS; ictrAllocatedBlocks++)
		{
			if (BT_apAllocatedBlockList[ictrAllocatedBlocks] != 0) 
			{	
				//OS_Printf("block %d: %d\n", ictrAllocatedBlocks, BT_apAllocatedBlockList[ictrAllocatedBlocks]);
				u32ctrBlocksNotUnloaded++;
			}			
		}	
	}
	
	if (u32ctrBlocksNotUnloaded > 0)
	{
		//OS_Printf("%d blocks NOT unloaded!!!\n",u32ctrBlocksNotUnloaded);		
		//OS_Printf("HEAP is not completely cleared :(\n"); 
	}
	else 
	{
		//OS_Printf("\nHEAP is clean ^___^\n");
	}
}


/*

#define TRACE_ENABLED
void trace(const char *pchFormat, ...)
{
	#ifdef TRACE_ENABLED
		va_list vlist;
		char	achBuffer[256];
		SDK_NULL_ASSERT(pchFormat);

		va_start(vlist, pchFormat);
		(void) vsnprintf(achBuffer, sizeof(achBuffer), pchFormat, vlist);
		va_end(vlist);
		
		//OS_Printf("%s", achBuffer);
	#endif
}
*/


bool BT_pointIsInRect(float fxPoint, float fyPoint, float fxRect, float fyRect, float fwRect, float fhRect)
{
	if (fxPoint >= fxRect)						// Left check
		if (fxPoint <= fxRect + fwRect)			// Right check
			if (fyPoint >= fyRect)				// Top check
				if (fyPoint <= fyRect + fhRect)	// Bottom check
					return true;
	
	return false;
}





const char *BT_catFileExt(const char *pchFname, const char *pchFext)
{
    static char chBuf[FS_FILE_NAME_MAX + 1];
    SDK_NULL_ASSERT( pchFname );
    SDK_NULL_ASSERT( pchFext );
	SDK_ASSERT( strlen(pchFname) + strlen(pchFext) <= FS_FILE_NAME_MAX );

    (void)strcpy(chBuf, pchFname);
    (void)strcat(chBuf, pchFext);
    return chBuf;
}

void BT_mtxRot22( MtxFx22* m, fx32 sinVal, fx32 cosVal )
{
    SDK_NULL_ASSERT( m );
    m->_00 = cosVal;
    m->_01 = sinVal;
    m->_10 = -sinVal;
    m->_11 = cosVal;
}


/*---------------------------------------------------------------------------*
  Name:         NitroStartUp

  Description:  startup before NitroMain()
                 - Initialize memory control system for new()

                 FYI:
                 - Startup fuctions called in following order
                          1) NitroStartUp();
                          2) Global/Static Constructors
                          3) NitroMain();

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/


void NitroStartUp(void) 
{
    //void*    arenaLo;
    //void*    arenaHi;
 

    //arenaLo = OS_GetArenaLo( ARENA_ID );  
    //arenaHi = OS_GetArenaHi( ARENA_ID );

    // Create a heap
    //arenaLo = OS_InitAlloc( ARENA_ID, arenaLo, arenaHi, 1 );
    //OS_SetArenaLo( ARENA_ID, arenaLo );

    // Ensure boundaries are 32B aligned
    //arenaLo = (void*)ROUND( arenaLo, align );
    //arenaHi = (void*)ROUND( arenaHi, align );

    // The boundaries given to OSCreateHeap should be 32B aligned
    //(void)OS_SetCurrentHeap( ARENA_ID, OS_CreateHeap( ARENA_ID, arenaLo, arenaHi ) );

    // From here on out, OS_Alloc and OS_Free behave like malloc and free respectively
    //OS_SetArenaLo( ARENA_ID, arenaLo = arenaHi );
    

    OS_Init();  
	
	//OS_Printf("starting up...\n");
	
	
	
	BT_sysHeapMemory = OS_AllocFromMainArenaLo(BT_SYSTEM_HEAP_SIZE, align);
	u32 arenaLow = BT_ROUND_UP(OS_GetMainArenaLo(), align);
	u32 arenaHigh = BT_ROUND_DOWN(OS_GetMainArenaHi(), align);
	u32 appHeapSize = arenaHigh - arenaLow;
	
	//OS_Printf("appHeapSize...\n",appHeapSize);
	
	BT_appHeapMemory = OS_AllocFromMainArenaLo(appHeapSize, align);

	BT_SystemHeap = NNS_FndCreateExpHeap(BT_sysHeapMemory, BT_SYSTEM_HEAP_SIZE);
	BT_AppHeap = NNS_FndCreateExpHeap(BT_appHeapMemory, appHeapSize);

	// Initialize the allocator to allocate and free the memory from the expanded heap.
	NNS_FndInitAllocatorForExpHeap(
		&BT_allocator,						// NNSFndAllocator Structure Address.
		BT_AppHeap,						// Expanded heap handle.
		align								// Alignment value that is applied to each memory block allocated.
	);
	
	// Initialize the allocator to allocate and free the memory from the expanded heap.
	NNS_FndInitAllocatorForExpHeap(
		&BT_gMicAllocator,						// NNSFndAllocator Structure Address.
		BT_AppHeap,						// Expanded heap handle.
		align								// Alignment value that is applied to each memory block allocated.
	);

	// Set the memory allocation mode of the expanded heap.
	NNS_FndSetAllocModeForExpHeap(
		BT_AppHeap,						// Expanded heap handle.
		NNS_FND_EXPHEAP_ALLOC_MODE_NEAR // Memory allocation mode.
							  );

	BT_memory = appHeapSize;
    
    for (int ictrAllocatedBlocks = 0; ictrAllocatedBlocks < BT_MAX_MONITORED_ALLOCBLOCKS; ictrAllocatedBlocks++)
	{
		BT_apAllocatedBlockList[ictrAllocatedBlocks] = 0;
	}	
	
	//warning: enabling BT_initMemoryTracker will cause memory allocation related crash
	//BT_initMemoryTracker();
	
	
}



NNSFndAllocator* BT_getAppAllocator()
{
	
	return &BT_allocator;
	
}

NNSFndAllocator* BT_getMicAllocator()
{
	
	return &BT_gMicAllocator;
	
}



NNSFndHeapHandle* BT_getSystemHeap()
{
	return &BT_SystemHeap;
	
}

NNSFndHeapHandle* BT_getAppHeap()
{
	
	return &BT_AppHeap;
}




u32 BT_u32Rand()
{
	return MATH_Rand32( &BT_randContext, RAND_MAX );
}


void *BT_malloc(std::size_t blocksize)
{
	
	SDK_ASSERT(NNS_FndGetAllocatableSizeForExpHeap(AppHeap) >= blocksize);
	if (BT_bMemoryTrackingEnabled)
	{
		if (BT_u32ctrAllocatedBlocks == BT_BREAK_AT_BLOCK)
		{
			//OS_Printf("DEBUG point: check stack trace to identify");	
		}    
		
		//OS_Printf("ALLOC_ID: %d\n", BT_u32ctrAllocatedBlocks);
	    BT_apAllocatedBlockList[BT_u32ctrAllocatedBlocks] = NNS_FndAllocFromExpHeapEx(BT_AppHeap, blocksize, align);//OS_AllocFromHeap( ARENA_ID, HEAP_ID, blocksize );
	    
	    BT_showMemory();

	    
	    return BT_apAllocatedBlockList[BT_u32ctrAllocatedBlocks++];	
	}
	else 
	{
		return NNS_FndAllocFromExpHeapEx(BT_AppHeap, blocksize, align);//OS_AllocFromHeap( ARENA_ID, HEAP_ID, blocksize );
	}
   
	////OS_Printf("BT_malloc working...\n");
	//return NNS_FndAllocFromExpHeapEx(BT_AppHeap, blocksize, align);//OS_AllocFromHeap( ARENA_ID, HEAP_ID, blocksize );
}

void *BT_calloc(u32 count, std::size_t blocksize)
{
	return NNS_FndAllocFromExpHeapEx(BT_AppHeap, count * blocksize, align);//OS_AllocFromHeap( ARENA_ID, HEAP_ID, count * blocksize );
}

void BT_free(void *block)
{
	
	
	
	if (BT_bMemoryTrackingEnabled)
	{
		u32 u32ctrTemp = 0;
		
		while (block != BT_apAllocatedBlockList[u32ctrTemp]) u32ctrTemp++;		
		BT_apAllocatedBlockList[u32ctrTemp] = 0;
	}
	
	NNS_FndFreeToExpHeap(BT_AppHeap, block);
    //OS_FreeToHeap( ARENA_ID, HEAP_ID, block );

	
	
	
	
	
	//if(block == NULL)
		//return;
	
	//SDK_NULL_ASSERT(block);
	
	//NNS_FndFreeToExpHeap(BT_AppHeap, block);
	//OS_FreeToHeap( ARENA_ID, HEAP_ID, block );
}

s16 BT_indexOf(const char* pchString, const char ch)
{
	for(u8 u8ctr = 0; u8ctr < strlen(pchString); u8ctr++)
		if(pchString[u8ctr] == ch)
			return u8ctr;
	return -1;
}

const char* BT_substring(const char* pchString, u8 u8Offset)
{
	char achBuf[256];
	if(u8Offset < 0 || u8Offset >= strlen(pchString)) return pchString;
	for(u8 u8ctr = u8Offset; u8ctr < strlen(pchString); u8ctr++)
		achBuf[u8ctr - u8Offset] = pchString[u8ctr];
	return achBuf;
}

const char* BT_substringEx(const char* pchString, u8 u8Offset, u8 u8Length)
{
	char achBuf[256];
	if(u8Offset < 0 || u8Offset >= strlen(pchString)) return pchString;
	if(u8Length - u8Offset > strlen(pchString)) u8Length = strlen(pchString) - u8Offset;
	for(u8 u8ctr = u8Offset; u8ctr < u8Offset + u8Length; u8ctr++)
		achBuf[u8ctr - u8Offset] = pchString[u8ctr];
	return achBuf;
}

bool BT_startsWith(const char* pchString, const char* pchPrefix)
{
	u8 u8Len = strlen(pchPrefix);
	if(u8Len > strlen(pchString)) return false;
	for(u8 u8ctr = 0; u8ctr < u8Len; u8ctr++)
		if(pchString[u8ctr] != pchPrefix[u8ctr])
			return false;
	return true;
}

bool BT_endsWith(const char* pchString, const char* pchSuffix)
{
	u8 u8Len = strlen(pchSuffix);
	if(u8Len > strlen(pchString)) return false;
	for(u8 u8ctr = 0; u8ctr < u8Len; u8ctr++)
		if(pchString[(strlen(pchString) - u8Len) + u8ctr] != pchSuffix[u8ctr])
			return false;
	return true;
}


/*
void memset(void* pDst, u32 u32Value, u8 u8Size)
{
	MI_DmaFill32(1, pDst, u32Value, u8Size);
}


void memcpy(void* pSrc, void* pDst, u8 u8Size)
{
	MI_DmaCopy32(2, pSrc, pDst, u8Size);
}


void memmove(void* pDst, void* source, u8 u8Size)
{
	MI_DmaSend32(3, source, pDst, u8Size); 
}
*/

bool BT_finite(float fValue)
{
	if (fValue != fValue)
		return false;
	
	if (fValue == 0.0f)
		return false;
	
	return true;
}

#ifdef __cpluspluc
}
#endif
