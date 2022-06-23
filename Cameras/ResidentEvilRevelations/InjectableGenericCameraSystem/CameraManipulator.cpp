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
#include "CameraManipulator.h"
#include "GameConstants.h"
#include "Console.h"

using namespace DirectX;
using namespace std;

extern "C" {
	// global variables which are accessed in Interceptor.asm and therefore need to be defined as 'extern "C"'
	LPBYTE g_cameraStructAddress = nullptr;
    LPBYTE g_fovStructAddress = nullptr;
	LPBYTE g_runFramesStructAddress = nullptr;
}

namespace IGCS::GameSpecific::CameraManipulator
{
	static float _originalRotationMatrixData[9];
	static float _originalCoordsData[3];
	static float _originalFoV;
	static bool _timeHasBeenStopped = false;
	static LPBYTE _showHudAddress = nullptr;
	static LPBYTE _stopTimeAddress = nullptr;
	

	void setShowHudAddress(LPBYTE address)
	{
		_showHudAddress = address;
	}


	void setStopTimeAddress(LPBYTE address)
	{
		_stopTimeAddress = address;
	}
	

	// newValue is the amount of frames to skip ahead when g_stopTime (via setStopTimeValue) is set to a value > 0
	void setRunFramesValue(BYTE newValue)
	{
		if (nullptr == g_runFramesStructAddress)
		{
			return;
		}
		*(g_runFramesStructAddress + RUNFRAMES_CVAR_IN_STRUCT_OFFSET) = newValue;
	}


	// newValue: 1 == time should be frozen, 0 == normal gameplay. There are 3 values possible for this variable (g_stopTime). 2 stops everything, also the player, 
	// I opted for 1, as it leaves a bit freedom for what to do with the player after pausing the game. 
	void setStopTimeValue(BYTE newValue)
	{
		if (nullptr == _stopTimeAddress)
		{
			return;
		}
		*_stopTimeAddress = newValue;
	}


	// 1== show hud, 0 is hide hud.
	void setShowHudValue(BYTE newValue)
	{
		if (nullptr == _showHudAddress)
		{
			return;
		}
		*_showHudAddress = newValue;
	}


	XMFLOAT3 getCurrentCameraCoords()
	{
        float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_STRUCT_POS_X_OFFSET);
        XMFLOAT3 currentCoords = XMFLOAT3(coordsInMemory);
        return currentCoords;
	}


	// newLookQuaternion: newly calculated quaternion of camera view space. Used here to construct a game-specific camera structure (posVec, upVec, tagetLocation)
	// newCoords are the new coordinates for the camera in worldspace.
	void writeNewCameraValuesToGameData(XMVECTOR newLookQuaternion, XMFLOAT3 newCoords)
	{
        XMFLOAT4 qAsFloat4;
        XMStoreFloat4(&qAsFloat4, newLookQuaternion);

        XMMATRIX rotationMatrixPacked = XMMatrixRotationQuaternion(newLookQuaternion);
        XMFLOAT4X4 rotationMatrix;
        XMStoreFloat4x4(&rotationMatrix, rotationMatrixPacked);

        // The columns of the rotation (view) matrix are direction vectors in space; in relation to the camera, these are:
        // column1: right       -- local x-axis
        // column2: up          -- local y-axis 
        // column3: back        -- local z-axis  (out of the screen)

        XMFLOAT3 up(
            rotationMatrix._12,
            rotationMatrix._22,
            rotationMatrix._32
        );

        XMFLOAT3 forward(
            -rotationMatrix._13, 
            -rotationMatrix._23, 
            -rotationMatrix._33
        );

        if (CONTROLLER_Y_INVERT) {

            up.x = rotationMatrix._21;
            up.y = rotationMatrix._22;
            up.z = rotationMatrix._23;

            forward.x = -rotationMatrix._31;
            forward.y = -rotationMatrix._32;
            forward.z = -rotationMatrix._33;
        }

        // Update the values in memory
        float* cameraLocation = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_STRUCT_POS_X_OFFSET);
        cameraLocation[0] = newCoords.x;
        cameraLocation[1] = newCoords.y;
        cameraLocation[2] = newCoords.z;        

        // to actually translate the camera, the look-at target object needs to be translated as well
        float* cameraTargetLocation = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_STRUCT_TARGET_X_OFFSET);
        cameraTargetLocation[0] = newCoords.x + 100 * forward.x;
        cameraTargetLocation[1] = newCoords.y + 100 * forward.y;
        cameraTargetLocation[2] = newCoords.z + 100 * forward.z;

        float* cameraUpVecLocation = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_STRUCT_UP_X_OFFSET);
        cameraUpVecLocation[0] = up.x;
        cameraUpVecLocation[1] = up.y;
        cameraUpVecLocation[2] = up.z;        
	}


	// Waits for the interceptor to pick up the camera struct address. Should only return if address is found 
	void waitForCameraStructAddresses()
	{
		Console::WriteLine("Waiting for camera struct interception...");
		while (nullptr == g_cameraStructAddress)
		{
			Sleep(100);
		}
		Console::WriteLine("Camera found.");

#ifdef _DEBUG
		cout << "Camera address: " << hex << (void*)g_cameraStructAddress << endl;
#endif
	}


	void resetFoV()
	{
		if (nullptr == g_fovStructAddress)
		{
			return;
		}
		float* fovInMemory = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_FOV_STRUCT_OFFSET);
		*fovInMemory = _originalFoV;
	}


	void changeFoV(float amount)
	{
		if (nullptr == g_fovStructAddress)
		{
			return;
		}

		float* fovInMemory = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_FOV_STRUCT_OFFSET);
		*fovInMemory += amount;
	}


	void restoreOriginalCameraValues()
	{
        // TODO: restore camera
        /*
		if (nullptr == g_cameraStructAddress)
		{
			return;
		}
		float* matrixInMemory = reinterpret_cast<float*>(g_cameraStructAddress + ROTATION_MATRIX_IN_CAMERA_STRUCT_OFFSET);
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
		memcpy(matrixInMemory, _originalRotationMatrixData, 9 * sizeof(float));
		memcpy(coordsInMemory, _originalCoordsData, 3 * sizeof(float));
        */
		if (nullptr != g_fovStructAddress)
		{
			float* floatInMemory = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_FOV_STRUCT_OFFSET);
			*floatInMemory = _originalFoV;
		}
	}


	void cacheOriginalCameraValues()
	{
        // TODO: cache vals to restore camera
        /*
		if (nullptr == g_cameraStructAddress)
		{
			return;
		}
		float* matrixInMemory = reinterpret_cast<float*>(g_cameraStructAddress + ROTATION_MATRIX_IN_CAMERA_STRUCT_OFFSET);
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
		memcpy(_originalRotationMatrixData, matrixInMemory, 9 * sizeof(float));
		memcpy(_originalCoordsData, coordsInMemory, 3 * sizeof(float));
        */
		if (nullptr != g_fovStructAddress)
		{
			float* floatInMemory = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_FOV_STRUCT_OFFSET);
			_originalFoV = *floatInMemory;
		}
	}
}