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
#include <chrono>
#include <ratio>
#include <Windows.h>
#include "DefaultTimeProvider.h"

namespace IGCS::Timekeeping
{

    DefaultTimeProvider::DefaultTimeProvider()
        :_msPerTick(0), _hiResTimerSupported(false)
    {
        LARGE_INTEGER frequency;
        if (!QueryPerformanceFrequency(&frequency)) 
        {
            // will not occur on systems running Windows XP or later
            
            // fallback to std::chrono::steady_clock
            _hiResTimerSupported = false;
            
            std::ratio secondsPerTick = std::chrono::steady_clock::period();
            _msPerTick = 1000.0 * (double)secondsPerTick.num / secondsPerTick.den;;
        }
        else 
        {
            _hiResTimerSupported = true;

            // QPF returns the frequency in ticks per second, 
            // what we want is milliseconds per tick.
            LONGLONG ticksPerSecond = frequency.QuadPart;
            _msPerTick = 1000.0 / ticksPerSecond;
        }
    }

    long long DefaultTimeProvider::getTimestamp()
    {
        if (_hiResTimerSupported) 
        {
            LARGE_INTEGER ticks;
            QueryPerformanceCounter(&ticks);
            return ticks.QuadPart;
        }
        else
        {
            auto time = std::chrono::steady_clock::now();
            auto duration = time.time_since_epoch();
            return duration.count();
        }
    }

    double DefaultTimeProvider::getMillisecondsPerTick() const
    {
        return _msPerTick;
    }
}
