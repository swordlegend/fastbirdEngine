/*
 -----------------------------------------------------------------------------
 This source file is part of fastbird engine
 For the latest info, see http://www.jungwan.net/
 
 Copyright (c) 2013-2015 Jungwan Byun
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 -----------------------------------------------------------------------------
*/

#pragma once
#include "FBCommonHeaders/Types.h"
#include <mutex>
#include <atomic>
#include <condition_variable>
namespace fb
{
	//---------------------------------------------------------------------------
	struct FB_DLL_THREAD FB_CRITICAL_SECTION
	{
	private:
		std::mutex mMutex;
	public:
		FB_CRITICAL_SECTION();
		~FB_CRITICAL_SECTION();
		void Lock();
		void Unlock();	
	};

	//---------------------------------------------------------------------------
	struct FB_DLL_THREAD FB_CRITICAL_SECTION_R
	{
	private:
		std::recursive_mutex mMutex;
	public:
		FB_CRITICAL_SECTION_R();
		~FB_CRITICAL_SECTION_R();
		void Lock();
		void Unlock();
	};

	//---------------------------------------------------------------------------
	struct FB_DLL_THREAD ENTER_CRITICAL_SECTION
	{
		FB_CRITICAL_SECTION* mCS;

		ENTER_CRITICAL_SECTION(FB_CRITICAL_SECTION* cs);
		ENTER_CRITICAL_SECTION(FB_CRITICAL_SECTION& cs);
		~ENTER_CRITICAL_SECTION();
	};

	struct FB_DLL_THREAD ENTER_CRITICAL_SECTION_R
	{
		FB_CRITICAL_SECTION_R* mCS;

		ENTER_CRITICAL_SECTION_R(FB_CRITICAL_SECTION_R* cs);
		ENTER_CRITICAL_SECTION_R(FB_CRITICAL_SECTION_R& cs);
		~ENTER_CRITICAL_SECTION_R();
	};

	//---------------------------------------------------------------------------
	struct FB_DLL_THREAD FB_READ_WRITE_CS
	{
	private:
		std::mutex mMutex;		
		std::condition_variable mC;				
		std::atomic<int> mNumReaders;
		std::atomic<bool> mWriting;

	public:
		FB_READ_WRITE_CS();
		~FB_READ_WRITE_CS();
		void EnterReader();
		void LeaveReader();
		void EnterWriter();
		void LeaveWriter();
	};

	//-----------------------------------------------------------------------
	struct FB_DLL_THREAD READ_LOCK
	{
		FB_READ_WRITE_CS* mReadWrite;

		READ_LOCK(FB_READ_WRITE_CS& lock);
		~READ_LOCK();
	};

	struct FB_DLL_THREAD WRITE_LOCK
	{
		FB_READ_WRITE_CS* mReadWrite;

		WRITE_LOCK(FB_READ_WRITE_CS& lock);
		~WRITE_LOCK();
	};

	FB_DECLARE_SMART_PTR(SyncEvent);
	class FB_DLL_THREAD SyncEvent
	{
	public:
		virtual ~SyncEvent() {}				
		virtual void Trigger() = 0;
		virtual void Reset() = 0;
		virtual void Lock() = 0;
		virtual bool Wait(DWORD WaitTime = 0xffffffff) = 0;
	};

	FB_DLL_THREAD SyncEventPtr CreateSyncEvent(bool ManualReset = false, char* Name = 0);
}

