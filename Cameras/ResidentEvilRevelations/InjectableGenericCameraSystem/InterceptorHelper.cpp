////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2018, Frans Bouma
// All rights reserved.
// https://github.com/FransBouma/InjectableGenericCameraSystem
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
//
//  * Redistributions of source code must retain the above copyright notice, this
//	  list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and / or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <map>
#include "InterceptorHelper.h"
#include "GameConstants.h"
#include "GameImageHooker.h"
#include "Utils.h"
#include "AOBBlock.h"
#include "Console.h"
#include "CameraManipulator.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------
// external asm functions, defined in Interceptor.asm
extern "C" {
	void cameraAddressInterceptor();
    void cameraTargetInterceptor();
    void cameraUpVectorInterceptor();
	void fovAddressInterceptor();
	//void runFramesAddressInterceptor();   // TODO
}

// external addresses used in functions in Interceptor.asm
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
    LPBYTE _cameraTargetInterceptionContinue = nullptr;
    LPBYTE _cameraUpVectorInterceptionContinue = nullptr;
	LPBYTE _fovAddressInterceptionContinue = nullptr;
	LPBYTE _runFramesAddressInterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "F3 0F 11 43 08 0F 57 C0 F3 0F 11 43 0C F3 0F 58 D4 F3 0F 11 53 04 F3 0F 58 CB F3 0F 11 0B", 1);
		aobBlocks[FOV_ADDRESS_INTERCEPT_KEY] = new AOBBlock(FOV_ADDRESS_INTERCEPT_KEY, "F3 0F 11 46 10 F3 0F 11 86 C4 00 00 00 D9 03 D9 18 D9 43 04 D9 58 04 D9 43 08 F3 0F 11 48 0C D9 58 08 8B 44 24 14", 1);
        aobBlocks[CAMERA_TARGET_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_TARGET_ADDRESS_INTERCEPT_KEY, "F3 0F 11 07 0F 57 C0 F3 0F 11 47 0C F3 0F 59 CE F3 0F 58 CC F3 0F 11 4F 04 F3 0F 59 D6 F3 0F 58 D5 F3 0F 11 57 08", 1);
        aobBlocks[CAMERA_UPVEC_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_UPVEC_ADDRESS_INTERCEPT_KEY, "84 C0 74 2E F3 0F 10 84 24 90 00 00 00 F3 0F 11 01 F3 0F 10 84 24 94 00 00 00 F3 0F 11 41 04 F3 0F 10 84 24 98 00 00 00 F3 0F 11 41 08 F3 0F 11 49 0C F3 0F 10 96 10 01 00 00 F3 0F 10 5E 10", 1);
        // TODO:
		//aobBlocks[SHOWHUD_CVAR_ADDRESS_INTERCEPT_KEY] = new AOBBlock(SHOWHUD_CVAR_ADDRESS_INTERCEPT_KEY, "48 8D 54 24 ?? 48 89 44 24 ?? 48 8B CB E8 ?? ?? ?? ?? 83 3D | ?? ?? ?? ?? 00", 1);
		//aobBlocks[STOPTIME_CVAR_ADDRESS_INTERCEPT_KEY] = new AOBBlock(STOPTIME_CVAR_ADDRESS_INTERCEPT_KEY, "48 89 6C 24 ?? 48 89 74 24 ?? 48 89 7C 24 ?? 48 8B D9 83 3D | ?? ?? ?? ?? 00 74", 1);
		//aobBlocks[RUNFRAMES_CVAR_ADDRESS_INTERCEPT_KEY] = new AOBBlock(RUNFRAMES_CVAR_ADDRESS_INTERCEPT_KEY, "49 3B C0 75 ?? 48 8D 81 ?? ?? ?? ?? EB ?? FF D2 48 8B 48 ?? 8B 51", 1);

		map<string, AOBBlock*>::iterator it;
		bool result = true;
		for(it = aobBlocks.begin(); it!=aobBlocks.end();it++)
		{
			result &= it->second->scan(hostImageAddress, hostImageSize);
		}
		if (result)
		{
			Console::WriteLine("All interception offsets found.");
		}
		else
		{
			Console::WriteError("One or more interception offsets weren't found: tools aren't compatible with this game's version.");
		}
	}


	void setCameraStructInterceptorHook(map<string, AOBBlock*> &aobBlocks)
	{
        const DWORD camInterceptContinueOffset = 0x1E;
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], camInterceptContinueOffset, &_cameraStructInterceptionContinue, &cameraAddressInterceptor);
	}
	

	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks)
	{
        const DWORD camTargetInterceptContinueOffset = 0x26;
        GameImageHooker::setHook(aobBlocks[CAMERA_TARGET_ADDRESS_INTERCEPT_KEY], camTargetInterceptContinueOffset, &_cameraTargetInterceptionContinue, &cameraTargetInterceptor);

        const DWORD camUpVectorInterceptContinueOffset = 0x3F;
        GameImageHooker::setHook(aobBlocks[CAMERA_UPVEC_ADDRESS_INTERCEPT_KEY], camUpVectorInterceptContinueOffset, &_cameraUpVectorInterceptionContinue, &cameraUpVectorInterceptor);

        const DWORD fovInterceptContinueOffset = 0x17;
		GameImageHooker::setHook(aobBlocks[FOV_ADDRESS_INTERCEPT_KEY], fovInterceptContinueOffset, &_fovAddressInterceptionContinue, &fovAddressInterceptor);
        // TODO:
		// GameImageHooker::setHook(aobBlocks[RUNFRAMES_CVAR_ADDRESS_INTERCEPT_KEY], 0x17, &_runFramesAddressInterceptionContinue, &runFramesAddressInterceptor);
		// CameraManipulator::setShowHudAddress(Utils::calculateAbsoluteAddress(aobBlocks[SHOWHUD_CVAR_ADDRESS_INTERCEPT_KEY], 5));		//NewColossus_x64vk.exe+F9C173 - 83 3D 669FFF01 00  - cmp dword ptr [NewColossus_x64vk.exe+2F960E0],00 << g_showhud read here.
		// CameraManipulator::setStopTimeAddress(Utils::calculateAbsoluteAddress(aobBlocks[STOPTIME_CVAR_ADDRESS_INTERCEPT_KEY], 5));		//NewColossus_x64vk.exe+A8F886 - 83 3D 839E4A02 00  - cmp dword ptr [NewColossus_x64vk.exe+2F39710],00 << g_stopTime read here.
	}
}
