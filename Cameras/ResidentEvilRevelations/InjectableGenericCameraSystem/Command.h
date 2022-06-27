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

namespace IGCS::Input
{
    // Abstract Command base class, meant to be used in an update loop–style code
    class Command
    {
    public:
        /** 
         * Checks for any preconditions that need to be satisfied before the command can be invoked.
         * Does not invoke any action and does not change the state of the command.
         * @return A 'bool' indicating if the preconditions are met at call time.
         */ 
        virtual bool checkPreconditions() const = 0;

        /** 
         * Checks if there is, at call time, an active request to invoke this command. 
         * (E.g. in an update loop, the user is pressing the associated input key.)
         * Does not invoke any action and does not change the state of the command.
         * @return A 'bool' indicating if there's an active request.
         */
        virtual bool checkIfInvokeRequested() const = 0;

        /**
         * Checks if there's an active request to invoke this command, then check if the preconditions are met,
         * then invokes the underlying action if all off the checks pass.
         * @return Returns 'true' if the action was invoked.
         */
        bool attemptInvokeIfRequested() { return checkIfInvokeRequested() ? attemptInvoke() : false; }

        /// Invokes the underlying action if preconditions are met. @return Returns 'true' if the action was invoked
        bool attemptInvoke() { return checkPreconditions() ? onInvoke() : false; }

        /// Invokes the underlying action unconditionally
        void invokeUnconditionally() { onInvoke(true); };

    protected:
        /*
         * To be implemented by derivatives. Should return a bool indicating if the action was invoked.
         * @param calledInUnconditionalContext - allows the implementation to determine the context in which the method was called 
         * (e.g. attemptInvoke() vs invokeUnconditionally()). This lets it make additional decisions based on that context,
         * e.g. it can choose to ignore the input throttling timeout (if any). 
         */
        virtual bool onInvoke(bool calledInUnconditionalContext = false) = 0;
    };
}
