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
#include <memory>
#include "Timer.h"

namespace IGCS::Timekeeping
{

    Timer::Timer(std::shared_ptr<ITimeProvider> pTimeProvider, double clampDiffAt)
        :_isPaused(true),
        _lastStartTimestamp(0),
        _accumulatedTime(0),
        _pTimeProvider(pTimeProvider), 
        _clampDiffAt(clampDiffAt)
    {
    }

    void Timer::start()
    {
        _lastStartTimestamp = getInternalTimestamp();
        _isPaused = false;
    }

    void Timer::pause()
    {
        _accumulatedTime = getTimeInMilliseconds();
        _isPaused = true;
    }

    void Timer::restart(bool restartInPausedState)
    {
        _accumulatedTime = 0;
        _isPaused = restartInPausedState;
        if (!restartInPausedState)
        {
            start();
        }
    }

    double Timer::getTimeInMilliseconds() const
    {
        if (_isPaused)
        {
            return _accumulatedTime;
        }

        long long diff = getInternalTimestamp() - _lastStartTimestamp;
        double elapsed = _accumulatedTime + timestampDiffToMilliseconds(diff);

        return elapsed;
    }

    long long Timer::getInternalTimestamp() const
    {
        return _pTimeProvider->getTimestamp();
    }

    double Timer::timestampDiffToMilliseconds(long long timeDiff) const
    {
        double resolution = _pTimeProvider->getMillisecondsPerTick();
        double result = clampTimeDiff(timeDiff * resolution);
        return result;
    }

    double Timer::clampTimeDiff(double rawDiff) const
    {
        double clamped = rawDiff < 0 ? 0 : rawDiff;
        clamped = clamped > _clampDiffAt ? _clampDiffAt : clamped;
        return clamped;
    }
}
