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
#include "Toggler.h"
#include "Throttler.h"
#include "Timer.h"*/
#include "ToggleCommand.h"

namespace IGCS::Input
{
    // Note: declared in the header, to make some of the constructors easier to use in client code
    bool noPreconditions() { return true; }  

#pragma region Helper funcs forward declarations

    std::shared_ptr<IGCS::Utils::Throttler> createThrottledToggler(std::function<bool()> isInvokeRequestedCallback,
        std::initializer_list<std::function<void()> > actions,
        std::function<bool()> preconditionsCallback,
        std::shared_ptr<IGCS::Timekeeping::ITimeProvider>& timeProvider,
        double throttleDelayInMilliseconds);

    std::function<bool(bool)> createAction(std::shared_ptr<IGCS::Utils::Toggler> pToggler);
    std::function<bool(bool)> createAction(std::shared_ptr<IGCS::Utils::Throttler> pThrottler);

#pragma endregion

    ToggleCommand::ToggleCommand(
        std::initializer_list<std::function<void()> > actions,
        std::function<bool()> checkIfInvokeRequested,
        std::function<bool()> checkPreconditions
    )
        : _invokeWrappedAction(createAction(std::make_unique<IGCS::Utils::Toggler>(actions)))
    {
    }

    ToggleCommand::ToggleCommand(
        std::initializer_list<std::function<void()> > actions,
        std::function<bool()> checkIfInvokeRequested,
        std::shared_ptr<IGCS::Timekeeping::ITimeProvider>& timeProvider,
        double throttleDelayInMilliseconds
    )
        : ToggleCommand(actions, checkIfInvokeRequested, noPreconditions, timeProvider, throttleDelayInMilliseconds)
    {

    }

    ToggleCommand::ToggleCommand(
        std::initializer_list<std::function<void()> > actions,
        std::function<bool()> checkIfInvokeRequested,
        std::function<bool()> checkPreconditions,
        std::shared_ptr<IGCS::Timekeeping::ITimeProvider>& timeProvider,
        double throttleDelayInMilliseconds
    )
        : _checkPreconditions(checkPreconditions), _checkIfInvokeRequested(checkIfInvokeRequested),
        _invokeWrappedAction(createAction(createThrottledToggler(checkIfInvokeRequested, actions, checkPreconditions, timeProvider, throttleDelayInMilliseconds)))
    {

    }

    bool ToggleCommand::checkIfInvokeRequested() const
    {
        return _checkIfInvokeRequested();
    }

    bool ToggleCommand::checkPreconditions() const
    {
        return _checkPreconditions();
    }

    bool ToggleCommand::onInvoke(bool unconditionally)
    {
        return _invokeWrappedAction(unconditionally);
    }

    //// Helpers //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<IGCS::Utils::Throttler> createThrottledToggler(
        std::function<bool()> checkIfInvokeRequested,
        std::initializer_list<std::function<void()> > actions,
        std::function<bool()> checkPreconditions,
        std::shared_ptr<IGCS::Timekeeping::ITimeProvider>& timeProvider,
        double throttleDelayInMilliseconds
    )
    {
        auto toggler = std::make_shared<IGCS::Utils::Toggler>(actions);
        auto toggleAction = [t = std::move(toggler)]() { t->toggle(); };

        auto throttler = std::make_shared<IGCS::Utils::Throttler>(
            toggleAction,
            timeProvider, 
            throttleDelayInMilliseconds
        );

        return throttler;
    }

    // helper func
    std::function<bool(bool)> createAction(std::shared_ptr<IGCS::Utils::Toggler> pToggler)
    {
        return [t = std::move(pToggler)](bool calledInUnconditionalContext) {
            t->toggle(); 
            return true;
        };
    }

    // helper func
    std::function<bool(bool)> createAction(std::shared_ptr<IGCS::Utils::Throttler> pThrottler)
    {
        return [t = std::move(pThrottler)](bool calledInUnconditionalContext) {
            if (calledInUnconditionalContext)
            {
                t->invokeUnconditionally();
                return true;
            }

            return t->attemptInvoke(); 
        };
    }
}
