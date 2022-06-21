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
#pragma once
#include "stdafx.h"

using namespace DirectX;

namespace IGCS
{
	class Camera
	{
	public:
		Camera();
		~Camera(void);

		XMVECTOR calculateLookQuaternion();
		XMFLOAT3 calculateNewCoords(const XMFLOAT3 currentCoords, const XMVECTOR lookQ);
		void resetMovement();
		void resetAngles();
		void moveForward(float amount);
		void moveRight(float amount);
		void moveUp(float amount);
		void yaw(float amount);
		void pitch(float amount);
		void roll(float amount);
		void setPitch(float angle);
		void setYaw(float angle);
		void setRoll(float angle);
		float lookDirectionInverter() { return _lookDirectionInverter; }
		void toggleLookDirectionInverter() { _lookDirectionInverter = -_lookDirectionInverter; }

	private:
		float clampAngle(float angle) const;

		XMFLOAT3 _direction;
		float _yaw;
		float _pitch;
		float _roll;
		bool _movementOccurred;
		float _movementSpeed;
		float _rotationSpeed;
		float _lookDirectionInverter;
	};
}