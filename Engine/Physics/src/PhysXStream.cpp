//------------------------------------------------------------------------------
// Created by Daniel Sefton
//------------------------------------------------------------------------------

// This code contains NVIDIA Confidential Information and is disclosed to you 
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and 
// any modifications thereto. Any use, reproduction, disclosure, or 
// distribution of this software and related documentation without an express 
// license agreement from NVIDIA Corporation is strictly prohibited.
// 
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2012 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#include "Platform/StableHeaders.h"

#include "Physics/PhysXStream.h"

#include <PhysX/PxPhysicsAPI.h>

using namespace physx;

//------------------------------------------------------------------------------
namespace Dangine {
//------------------------------------------------------------------------------
PhysXMemoryOutputStream::PhysXMemoryOutputStream()
	: mData(NULL)
	, mSize(0)
	, mCapacity(0)
{
}
//------------------------------------------------------------------------------
PhysXMemoryOutputStream::~PhysXMemoryOutputStream()
{
	if (mData)
		delete[] mData;
}
//------------------------------------------------------------------------------
PxU32 PhysXMemoryOutputStream::write(const void* src, PxU32 size)
{
	PxU32 expectedSize = mSize + size;
	if (expectedSize > mCapacity)
	{
		mCapacity = expectedSize + 4096;

		PxU8* newData = new PxU8[mCapacity];
		PX_ASSERT(newData != NULL);

		if (newData)
		{
			memcpy(newData, mData, mSize);
			delete[] mData;
		}
		mData = newData;
	}
	memcpy(mData + mSize, src, size);
	mSize += size;
	return size;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PhysXMemoryInputData::PhysXMemoryInputData(PxU8* data, PxU32 length)
	: mSize(length)
	, mData(data)
	, mPos(0)
{
}
//------------------------------------------------------------------------------
PxU32 PhysXMemoryInputData::read(void* dest, PxU32 count)
{
	PxU32 length = PxMin<PxU32>(count, mSize - mPos);
	memcpy(dest, mData + mPos, length);
	mPos += length;
	return length;
}
//------------------------------------------------------------------------------
PxU32 PhysXMemoryInputData::getLength() const
{
	return mSize;
}
//------------------------------------------------------------------------------
void PhysXMemoryInputData::seek(PxU32 offset)
{
	mPos = PxMin<PxU32>(mSize, offset);
}
//------------------------------------------------------------------------------
PxU32 PhysXMemoryInputData::tell() const
{
	return mPos;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PhysXFileOutputStream::PhysXFileOutputStream(const char* filename)
{
	mFile = NULL;
	fopen_s(&mFile, filename, "wb");
}
//------------------------------------------------------------------------------
PhysXFileOutputStream::~PhysXFileOutputStream()
{
	if (mFile)
		fclose(mFile);
}
//------------------------------------------------------------------------------
PxU32 PhysXFileOutputStream::write(const void* src, PxU32 count)
{
	return mFile ? (PxU32)fwrite(src, 1, count, mFile) : 0;
}
//------------------------------------------------------------------------------
bool PhysXFileOutputStream::isValid()
{
	return mFile != NULL;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PhysXFileInputData::PhysXFileInputData(const char* filename)
{
	mFile = NULL;
	fopen_s(&mFile, filename, "rb");

	if (mFile)
	{
		fseek(mFile, 0, SEEK_END);
		mLength = ftell(mFile);
		fseek(mFile, 0, SEEK_SET);
	}
	else
	{
		mLength = 0;
	}
}
//------------------------------------------------------------------------------
PhysXFileInputData::~PhysXFileInputData()
{
	if (mFile)
		fclose(mFile);
}
//------------------------------------------------------------------------------
PxU32 PhysXFileInputData::read(void* dest, PxU32 count)
{
	PX_ASSERT(mFile);
	const size_t size = fread(dest, 1, count, mFile);
	PX_ASSERT(PxU32(size) == count);
	return PxU32(size);
}
//------------------------------------------------------------------------------
PxU32 PhysXFileInputData::getLength() const
{
	return mLength;
}
//------------------------------------------------------------------------------
void PhysXFileInputData::seek(PxU32 pos)
{
	fseek(mFile, pos, SEEK_SET);
}
//------------------------------------------------------------------------------
PxU32 PhysXFileInputData::tell() const
{
	return ftell(mFile);
}
//------------------------------------------------------------------------------
bool PhysXFileInputData::isValid() const
{
	return mFile != NULL;
}
//------------------------------------------------------------------------------
} // namespace Dangine
//------------------------------------------------------------------------------