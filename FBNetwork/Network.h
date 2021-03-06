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
#include "FBNetwork/NetworkExceptions.h"
namespace fb {
	class FB_DLL_NETWORK Network
	{
	public:
		static int ErrorCode;
		enum
		{
			NoError,
			CannotConnect,
			InvalidURL,
		};
		// curl_global_init
		// should call these initialize function in application side.
		static void Initialize();
		static void Uninitialize();

		static std::string MakeMimeTypeForSuffix(const char* suffix);
		/// . is included
		static std::string MakeSuffixForMimeType(const char* mimeType);
		static ByteArray readURLContentToBuffer(const std::string& url);
		static ByteArray readURLContentToBuffer(const std::string& url, std::string& contentType);
		static ByteArray readURLContentToBuffer(const std::string& url, std::string& contentType, bool allocateDirect);
		static bool isHostAvailable(const char* url);
		static void logUnavailableHost(const char* url);
		static void logAvailableHost(const char* url);

		static std::string GetURIPath(const std::string& url);
		static std::string GetURIString(const std::string& url);
		static std::string GetURIHost(const std::string& url);
		static std::string GetURIQuery(const std::string& url);
		static std::string GetURIProtocol(const std::string& url);
		static void GetURIHostPathQuery(const std::string& url,
			std::string& host, std::string& path, std::string& query);
		static void GetURIHostPath(const std::string& url,
			std::string& host, std::string& path);
		static void GetURIAuthorityPath(const std::string& url,
			std::string& authority, std::string& path);		
	};
}