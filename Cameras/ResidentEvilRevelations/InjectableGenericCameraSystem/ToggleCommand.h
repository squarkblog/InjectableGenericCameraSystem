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
#include <functional>
#include "Toggler.h"
#include "Throttler.h"
#include "Timer.h"
#include "Command.h"

namespace IGCS::Input
{
    /// Represents no preconditions (always returns 'true')
    bool noPreconditions();

    class ToggleCommand : public Command
    {
    public:

        /*
          * Creates command
          *
          * @param checkIfInvokeRequested - a function that indicates that an invocation of this command was requested.
          * @param actions - an initializer list of actions with the signature 'void action()' (one of these will be called for each toggle).
          * @param checkPreconditions - a function that checks for any preconditions that need to be satisfied before the command can be invoked.
                                           Use IGCS::Input::noPreconditions to specify no preconditions.
          */
        ToggleCommand(
            std::initializer_list<std::function<void()> > actions,
            std::function<bool()> checkIfInvokeRequested,
            std::function<bool()> checkPreconditions = noPreconditions
        );

        /*
          * Creates a throttled command (cannot be invoked more frequently than the specified delay)
          *
          * @param checkIfInvokeRequested - a function that indicates that an invocation of this command was requested.
          * @param actions - an initializer list of actions with the signature 'void action()' (one of these will be called for each toggle).
          * @param timeProvider - an ITimeProvider object to measure the delay.
          * @param throttleDelayInMilliseconds - the throttle delay.
          */
        ToggleCommand(
            std::initializer_list<std::function<void()> > actions,
            std::function<bool()> checkIfInvokeRequested,
            std::shared_ptr<IGCS::Timekeeping::ITimeProvider>& timeProvider,
            double throttleDelayInMilliseconds
        );

         /*
          * Creates a throttled command (cannot be invoked more frequently than the specified delay)
          * 
          * @param checkIfInvokeRequested - a function that indicates that an invocation of this command was requested.
          * @param actions - an initializer list of actions with the signature 'void action()' (one of these will be called for each toggle).
          * @param checkPreconditions - a function that checks for any preconditions that need to be satisfied before the command can be invoked.
                                           Use IGCS::Input::noPreconditions to specify no preconditions.
          * @param timeProvider - an ITimeProvider object to measure the delay.
          * @param throttleDelayInMilliseconds - the throttle delay.
          */
        ToggleCommand(
            std::initializer_list<std::function<void()> > actions,
            std::function<bool()> checkIfInvokeRequested,
            std::function<bool()> checkPreconditions,
            std::shared_ptr<IGCS::Timekeeping::ITimeProvider>& timeProvider,
            double throttleDelayInMilliseconds
        );

        bool checkIfInvokeRequested() const override;
        bool checkPreconditions() const override;
    
    protected:
        bool onInvoke(bool calledInUnconditionalContext) override;

    private:
        std::function<bool()> _checkPreconditions;
        std::function<bool()> _checkIfInvokeRequested;
        std::function<bool(bool)> _invokeWrappedAction;
    };
}
