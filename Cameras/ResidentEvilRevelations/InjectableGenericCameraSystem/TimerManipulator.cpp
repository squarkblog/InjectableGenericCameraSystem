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
#include "TimerManipulator.h"
#include "GameConstants.h"
#include "Console.h"

extern "C" {
    // global variables which are accessed in Interceptor.asm and therefore need to be defined as 'extern "C"'
    LPBYTE g_timerStructAddress = nullptr;
}

namespace IGCS::GameSpecific::TimerManipulator
{
    static float _originalGameplayFps;
    static float _currentGameplayFps;
    static const float c_fpsIncrementPercentage = 1 / 150.0f;
    static bool _slowAdvanceToggled = false;
    static float _prevGameplayFps;
    static float* _pFps = nullptr;

    void _Init() 
    {
        if (nullptr == g_timerStructAddress)
        {
            return;
        }

        _pFps = (float*)(g_timerStructAddress + GAME_TIMER_GAMEPLAY_FPS_OFFSET);
        _originalGameplayFps = *_pFps;
        _currentGameplayFps = _originalGameplayFps;
    }

    bool TimerHooked()
    {
        if (nullptr == g_timerStructAddress)
        {
            return false;
        }

        if (nullptr == _pFps)
        {
            _Init();
        }

        return true;
    }

    void FreezeTime() 
    {
        if (nullptr == _pFps)
        {
            return;
        }

        _slowAdvanceToggled = false;
        _currentGameplayFps = 0.001f;
        *_pFps = _currentGameplayFps;
    }

    void SlowAdvance()
    {
        if (nullptr == _pFps)
        {
            return;
        }

        _slowAdvanceToggled = true;
        _currentGameplayFps = _originalGameplayFps * c_fpsIncrementPercentage;
        *_pFps = _currentGameplayFps;
    }

    void SlowAdvanceToggle()
    {
        if (nullptr == _pFps)
        {
            return;
        }

        if (_slowAdvanceToggled)
        {
            _slowAdvanceToggled = false;
            _currentGameplayFps = _prevGameplayFps;
            *_pFps = _currentGameplayFps;
        }
        else 
        {
            _prevGameplayFps = _currentGameplayFps;
            SlowAdvance();
        }
    }

    void RestoreNormalTimeFlow() 
    {
        if (nullptr == _pFps) 
        {
            return;
        }

        _slowAdvanceToggled = false;
        _currentGameplayFps = _originalGameplayFps;
        *_pFps = _currentGameplayFps;
    }



    void PrintTimerStructAddress()
    {
        if (nullptr != g_timerStructAddress) 
        {
#ifdef _DEBUG
            cout << "Timer struct address: " << hex << (void*)(g_timerStructAddress + GAME_TIMER_STRUCT_OFFSET) << endl;
            cout << "FPS address: " << hex << (void*)(g_timerStructAddress + GAME_TIMER_GAMEPLAY_FPS_OFFSET) << endl;
#endif
        }
    }
}

