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
#include "Timer.h"

namespace IGCS::Timekeeping
{
    // Wraps an ITimeProvider for use in scenarios where the provider will be called 
    // (perhaps as a dependency of other objects) many times in a tight loop (like a game loop).
    // Since ITimeProvider may call an API such as QueryPerformanceCounter, this can have some performance
    // impact in such a scenario if thousands of calls per cycle are made. 
    // This class allows the timestamp to be obtained once per update cycle, or at some other frequency, via a call to update(). 
    // The getTimestamp() method returns the cached the value obtained at the last update() call.
    class CycleLimitedTimeProvider : public ITimeProvider
    {
    public:
        CycleLimitedTimeProvider(ITimeProvider& timeProvider);
        void update();
        long long getTimestamp() override;
        double getMillisecondsPerTick() const override;
    
    private:
        ITimeProvider& _internalProvider;
        long long _lastTimestamp = 0;
    };
}
