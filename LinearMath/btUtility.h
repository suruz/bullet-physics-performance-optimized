
//! @brief Utility class.

#ifndef __BT_UTILITY_H__
#define __BT_UTILITY_H__
#ifdef __cplusplus
extern "C"
{
#endif


#include <stdlib.h>
#include <string.h>

#include <nnsys.h>

#define BT_SYSTEM_HEAP_SIZE				64 * 1024
#define BT_ROUND_UP(value, alignment)		(((u32) (value) + (alignment - 1)) &~(alignment - 1))
#define BT_ROUND_DOWN(value, alignment)	((u32) (value) &~(alignment - 1))

extern NNSFndHeapHandle BT_SystemHeap; // System heap (Use extended heap)
extern NNSFndHeapHandle BT_AppHeap;	// Application heap (expanded heap is used)
extern NNSFndAllocator	BT_allocator;	// Memory allocator
extern NNSFndAllocator	BT_gMicAllocator; //Microphone memory allocator
 
NNSFndAllocator* BT_getAppAllocator();
NNSFndAllocator* BT_getMicAllocator();
NNSFndHeapHandle* BT_getSystemHeap();
NNSFndHeapHandle* BT_getAppHeap();



#define AFFINE_IDX                  3                       //!< Affine Index for cell
#define CELL_ROTATE_UNIT            0xFF                    //!< Units of cell rotations
#define CELL_SCALE_UNIT             ((FX32_ONE >> 7) * 3)   //!< Units of cell scaling
                                                            //!< The scaling value is ensured to be not near 0
#define CELL_INIT_POS_X             (120 << FX32_SHIFT)     //!< Initial X position of the cell
#define CELL_INIT_POS_Y             (120 << FX32_SHIFT)     //!< Initial Y position of the cell

#define INIT_OUTPUT_TYPE            NNS_G2D_OAMTYPE_MAIN    //!< Initial value for output method

#define NUM_OF_OAM                  128                     //!< Number of OAMs allocated to OAM manager
#define NUM_OF_AFFINE               (NUM_OF_OAM / 4)        //!< Number of affine parameters allocated to OAM Manager

#define TEX_BASE                    0x0                     //!< Texture base address
#define TEX_PLTT_BASE               0x0                     //!< Texture palette base address

#define SCREEN_WIDTH                256						//!< screen width
#define SCREEN_HEIGHT               192						//!< screen height
#define PALETTE_SIZE_IN_BYTES		32

void *operator new(std::size_t blocksize);
void *operator new[] (std::size_t blocksize);
void operator delete(void *block) throw();
void operator delete[] (void *block)throw();



void BT_initMemoryTracker(void);
void BT_stopMemoryTracker(void);
void BT_showMemory(void);
u32 BT_getFreeMemory(void);
bool BT_compareFreeMemory(void);
void BT_checkHeap(void);
void BT_trace(const char *pchFormat, ...);


//! @brief Checks if a specific point is inside a rectangle.
//! @param fxPoint - X coordinate of the point to be checked.
//! @param fyPoint - Y coordinate of the point to be checked.
//! @param fxRect - X coordinate of the rectangle.
//! @param fyRect - Y coordinate of the rectangle.
//! @param fwRect - Width of the rectangle.
//! @param fhRect - Height of the rectangle.
//! @return Returns the result of the check.
//! @retval true - Point is inside the rectangle.
//! @retval false - Point is outside the rectangle.
bool BT_pointIsInRect(float fxPoint, float fyPoint, float fxRect, float fyRect, float fwRect, float fhRect);

/**
*	Returns a file name where the specified extension has been concatenated with the specified file name.
*	@param fname File name
*	@param fext File extension.
*	@return Pointer to the buffer used to store the string given by concatenating fname and fext.
*			Since this buffer is statically allocated within the function, the contents are destroyed on the next call.
*/
const char *BT_catFileExt(const char *pchFname, const char *pchFext);
void BT_mtxRot22( MtxFx22* m, fx32 sinVal, fx32 cosVal );




#define BT_RAND_MAX 32767
static MATHRandContext32 BT_randContext;

//! @brief Generates a random number from 0 - RAND_MAX
//! @return Returns the generated random number.
u32 BT_u32Rand();

/**
*	Allocate memory.
*	@param blocksize size of memory to be allocated.
*	@return pointer to the allocated memory.
*/
void *BT_malloc(std::size_t blocksize);
/**
*	Allocate memory.
*	@param count amount of array to be allocated.
*	@param blocksize size of memory to be allocated.
*	@return pointer to the allocated memory.
*/
void *BT_calloc(u32 count, std::size_t blocksize);
/**
*	Free allcated memory.
*	@param block pointer to the allocated memory to be released.
*/
void BT_free(void *block);
/**
*	Return the first occurence of ch from pchString.
*	Author: Octacore
*	Reference: java.lang.String.indexOf
*	@param pchString pointer to the string.
*	@param ch the character to find.
*	@return -1 if ch is not found, otherwise the first index where the ch was found.
*/
s16 BT_indexOf(const char* pchString, const char ch);
/**
*	Return a substring of the given string starting from offset.
*	Author: Octacore
*	Reference: java.lang.String.substring
*	@param pchString pointer to the string.
*	@param u8Offset offset of the new string.
*	@return substring
*/
const char* BT_substring(const char* pchString, u8 u8Offset);
/**
*	Return a substring of the given string starting from offset.
*	Author: Octacore
*	Reference: java.lang.String.substring
*	@param pchString pointer to the string.
*	@param u8Offset offset of the new string.
*	@param u8Length length of the new string.
*	@return substring
*/
const char* BT_substringEx(const char* pchString, u8 u8Offset, u8 u8Length);

/**
*	Checks if the given string contains the given prefix
*	Author: Octacore
*	Reference: java.lang.String.startsWith
*	@param pchString pointer to the string.
*	@param pchPrefix pointer to the string prefix.
*	@return true if the given prefix is found.
*/
bool BT_startsWith(const char* pchString, const char* pchPrefix);

/**
*	Checks if the given string contains the given suffix
*	Author: Octacore
*	Reference: java.lang.String.startsWith
*	@param pchString pointer to the string.
*	@param pchSuffix pointer to the string suffix.
*	@return true if the given suffix is found.
*/
bool BT_endsWith(const char* pchString, const char* pchSuffix);


//--- methods used by box2d
bool BT_finite(float fValue);
/*void memset(void* pDst, u32 u32Value, u8 u8Size);
void memcpy(void* pSrc, void* pDst, u8 u8Size);
void memmove(void* pDst, void* source, u8 u8Size);*/


#ifdef __cplusplus
}
#endif
/* __BT_UTILITY_H__ */
#endif
