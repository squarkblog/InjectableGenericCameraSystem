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
//      list of conditions and the following disclaimer.
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

namespace IGCS::GameSpecific
{
    // Mandatory constants to define for a game
    #define GAME_NAME                                   "Resident Evil: Revelations"
    #define CAMERA_VERSION                              "1.0.0"
    #define CAMERA_CREDITS                              "resquark, Otis_Inf"
    #define GAME_WINDOW_TITLE                           "RESIDENT EVIL REVELATIONS / BIOHAZARD REVELATIONS UE"

    // TODO: LEFTOVER from Wolfenstain 2 - see what's needed here
    #define INITIAL_PITCH_RADIANS                       0.0f
    #define INITIAL_YAW_RADIANS                         0.0f
    #define INITIAL_ROLL_RADIANS                        0.0f
    #define CONTROLLER_Y_INVERT                         true
    #define FASTER_MULTIPLIER                           6.0f
    #define SLOWER_MULTIPLIER                           0.04f
    #define MOUSE_SPEED_CORRECTION                      0.2f    // to correct for the mouse-deltas related to normal rotation.
    #define DEFAULT_MOVEMENT_SPEED                      1.8f
    #define DEFAULT_ROTATION_SPEED                      0.03f
    #define DEFAULT_FOV_SPEED                           0.1f
    #define DEFAULT_UP_MOVEMENT_MULTIPLIER              0.5f
    #define DEFAULT_SKIP_FRAMES_COUNT                   5
    // End Mandatory constants

    // AOB Keys for interceptor's AOB scanner
    #define FOV_ADDRESS_INTERCEPT_KEY                   "AOB_FOV_ADDRESS_INTERCEPT"
    #define FOV_IN_FOV_STRUCT_OFFSET                    0x10   // represented by a single float, half angle in deg

    #define CAMERA_ADDRESS_INTERCEPT_KEY                "AOB_CAMERA_ADDRESS_INTERCEPT"
    #define CAMERA_TARGET_ADDRESS_INTERCEPT_KEY         "AOB_CAMERA_TARGET_ADDRESS_INTERCEPT_KEY"
    #define CAMERA_UPVEC_ADDRESS_INTERCEPT_KEY          "AOB_CAMERA_UPVEC_ADDRESS_INTERCEPT_KEY"

    // Camera in RE:Rev is represented as: 
    //    { 
    //      positionVector3:        float (x, y, z) + either w (unused) or 1 float padding, translation of the camera in world coords,
    //      upVector3:              float (x, y, z) + either w (unused) or 1 float padding, (near?) normalized up direction vector,
    //      cameraTargetVector3:    float (x, y, z) + either w (unused) or 1 float padding, translation of the "look-at" target
    //    }
    // These are derived from player position, and are being constantly perturbed to simulate breathing / camera bob 
    // (which is why I'm not sure that the up vector is strictly normalized - but it has length near 1). 
    // A number of methods write to these - some are doing constant interpolation, some are only triggered when aiming, 
    // or during cutscenes, door animations, etc...
    #define CAMERA_STRUCT_POS_X_OFFSET                  0x00
    #define CAMERA_STRUCT_UP_X_OFFSET                   0x10
    #define CAMERA_STRUCT_TARGET_X_OFFSET               0x20

    #define GAME_TIMER_INTERCEPT_KEY                    "AOB_GAME_TIMER_INTERCEPT_KEY"
    #define GAME_TIMER_STRUCT_OFFSET                    0x1C0E8            // stores QueryPerformanceCounter output (qword LARGE_INTEGER)
    #define GAME_TIMER_STRUCT_PREV_OFFSET               0x1C0F0            // same as above, but for the prev frame (used for diff)
    #define GAME_TIMER_GAMEPLAY_FPS_OFFSET              0x38               // float

    // TODO: leftower from Wolfenstain - not yet sure how to do these in RE:Rev
    #define SHOWHUD_CVAR_ADDRESS_INTERCEPT_KEY          "AOB_SHOWHUD_ADDRESS_INTERCEPT"
    #define STOPTIME_CVAR_ADDRESS_INTERCEPT_KEY         "AOB_STOPTIME_CVAR_ADDRESS_INTERCEPT_KEY"
    #define RUNFRAMES_CVAR_ADDRESS_INTERCEPT_KEY        "AOB_RUNFRAMES_CVAR_ADDRESS_INTERCEPT_KEY"
                                                        
    // Indices in the structures read by interceptors 
    #define ROTATION_MATRIX_IN_CAMERA_STRUCT_OFFSET     0xF4        
    #define CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET       0xE8        
    #define RUNFRAMES_CVAR_IN_STRUCT_OFFSET             0x30
}