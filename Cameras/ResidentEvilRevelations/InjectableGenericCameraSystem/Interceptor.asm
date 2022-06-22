;////////////////////////////////////////////////////////////////////////////////////////////////////////
;// Part of Injectable Generic Camera System
;// Copyright(c) 2018, Frans Bouma
;// All rights reserved.
;// https://github.com/FransBouma/InjectableGenericCameraSystem
;//
;// Redistribution and use in source and binary forms, with or without
;// modification, are permitted provided that the following conditions are met :
;//
;//  * Redistributions of source code must retain the above copyright notice, this
;//	  list of conditions and the following disclaimer.
;//
;//  * Redistributions in binary form must reproduce the above copyright notice,
;//    this list of conditions and the following disclaimer in the documentation
;//    and / or other materials provided with the distribution.
;//
;// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
;// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
;// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
;// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
;// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
;// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;////////////////////////////////////////////////////////////////////////////////////////////////////////
;---------------------------------------------------------------
; Game specific asm file to intercept execution flow to obtain addresses, prevent writes etc.
;---------------------------------------------------------------

; In a 32-bit environment, functions are decorated; __cdecl calling convention adds a leading underscore (_), 
; which is why everything here is prefixed with '_' - otherwise, the linker would complain
; See also: https://stackoverflow.com/a/62754736, 
;      which in turn quotes: https://docs.microsoft.com/en-us/cpp/build/reference/decorated-names?view=vs-2019#FormatC
; If supporting different architectures, have different asm files for each architecture: x86, amd64

;---------------------------------------------------------------
; Public definitions so the linker knows which names are present in this file
PUBLIC _cameraAddressInterceptor
PUBLIC _fovAddressInterceptor
;PUBLIC runFramesAddressInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN _g_cameraStructAddress: dword
EXTERN _g_fovStructAddress: dword
EXTERN _g_runFramesStructAddress: dword
EXTERN _g_cameraEnabled: byte
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp (with an extra leading _ due to name mangling rules for x86)
EXTERN __cameraStructInterceptionContinue: dword
EXTERN __fovAddressInterceptionContinue: dword
EXTERN __runFramesAddressInterceptionContinue: dword

; Scratch pad
;
;---------------------------------------------------------------
.486                ; cpu mode
.xmm                ; Enable xmm instructions
.model flat

; x64 doesn't have these; in x86, without the instructions above, the linker complains with the 
; "invalid fixup" error. Found the solution here: https://stackoverflow.com/a/17517274

.code

_cameraAddressInterceptor PROC
;rerev.exe+7E4EC - F3 0F11 43 08         - movss [ebx+08],xmm0      << updates cam postion z          <========= INTERCEPT HERE
;rerev.exe+7E4F1 - 0F57 C0               - xorps xmm0,xmm0
;rerev.exe+7E4F4 - F3 0F11 43 0C         - movss [ebx+0C],xmm0      << camera position w (unused)? just padding?
;rerev.exe+7E4F9 - F3 0F58 D4            - addss xmm2,xmm4
;rerev.exe+7E4FD - F3 0F11 53 04         - movss [ebx+04],xmm2      << updates cam position y 
;rerev.exe+7E502 - F3 0F58 CB            - addss xmm1,xmm3
;rerev.exe+7E506 - F3 0F11 0B            - movss [ebx],xmm1         << updates cam position x              <---- END INTERCEPT
;rerev.exe+7E50A - F3 0F10 B6 10010000   - movss xmm6,[esi+00000110]                              <============= CONTINUE HERE
	mov [_g_cameraStructAddress], ebx		; store the camera struct in the variable (camera x at offset 0x00)
	cmp byte ptr [_g_cameraEnabled], 1		; if our camera is enabled, skip writes
	je exit
originalCode:
	movss dword ptr [ebx+08h],xmm0 			        ; write to camera position.z    ; original code, intercepted
	xorps xmm0,xmm0
	movss dword ptr [ebx+0Ch],xmm0
	addss xmm2,xmm4
	movss dword ptr [ebx+04h],xmm2                   ; write to camera position.y
	addss xmm1,xmm3
    movss dword ptr [ebx],xmm1                      ; write to camera position.x
exit:
	jmp dword ptr [__cameraStructInterceptionContinue] ; jmp back into the original game code, which is the location after the original statements above.
_cameraAddressInterceptor ENDP

_fovAddressInterceptor PROC
; The original function seems to compute some kind of fov/camera interpolation
; NOTE 1: this is some sort of a master value or a "target" value -- looks like the value stored there is read in in the next cycle
; NOTE 2: this is what actually affects the screen, leave this in -- looks like "current" value
;-------------------------------------------------------------------------------------------------------------------------------------
;rerev.exe+7E6DE - F3 0F11 46 10         - movss [esi+10],xmm0            << FoV write, ===== INTERCEPT HERE  -- see NOTE 1 above
;rerev.exe+7E6E3 - F3 0F11 86 C4000000   - movss [esi+000000C4],xmm0              << writes to a copy of FoV -- see NOTE 2 above
;rerev.exe+7E6EB - D9 03                 - fld dword ptr [ebx]
;rerev.exe+7E6ED - D9 18                 - fstp dword ptr [eax]
;rerev.exe+7E6EF - D9 43 04              - fld dword ptr [ebx+04]
;rerev.exe+7E6F2 - D9 58 04              - fstp dword ptr [eax+04]        <<----------------- finsih here
;rerev.exe+7E6F5 - D9 43 08              - fld dword ptr [ebx+08]         <<================= CONTINUE HERE
;rerev.exe+7E6F8 - F3 0F11 48 0C         - movss [eax+0C],xmm1
;rerev.exe+7E6FD - D9 58 08              - fstp dword ptr [eax+08]
;rerev.exe+7E700 - 8B 44 24 14           - mov eax,[esp+14]
	mov [_g_fovStructAddress],esi            ; Store the address in the variable (+ FoV is at offset 0x10)
	cmp byte ptr [_g_cameraEnabled], 1		; if our camera is enabled, skip writes
	je exit
originalCode:
    movss dword ptr [esi+10h],xmm0 
    ; movss dword ptr [esi+000000C4h],xmm0  ; moved this down
    fld dword ptr [ebx]
    fstp dword ptr [eax]
    fld dword ptr [ebx+04h]
    fstp dword ptr [eax+04h]
exit:
    movss dword ptr [esi+000000C4h],xmm0
	jmp dword ptr [__fovAddressInterceptionContinue] ; jmp back into the original game code, which is the location after the original statements above.
_fovAddressInterceptor ENDP

;TODO: leftover from Wolfenstein 2 -- see what's needed (not used ATM)
;runFramesAddressInterceptor PROC
;NewColossus_x64vk.exe+11B5343 - 48 8B C8              - mov rcx,rax
;NewColossus_x64vk.exe+11B5346 - 48 8B 00              - mov rax,[rax]
;NewColossus_x64vk.exe+11B5349 - 48 8B 90 B8000000     - mov rdx,[rax+000000B8]
;NewColossus_x64vk.exe+11B5350 - 4C 8D 05 31195001     - lea r8,[NewColossus_x64vk.exe+26B6C88]
;NewColossus_x64vk.exe+11B5357 - 49 3B C0              - cmp rax,r8								<< INTERCEPT HERE
;NewColossus_x64vk.exe+11B535A - 75 09                 - jne NewColossus_x64vk.exe+11B5365
;NewColossus_x64vk.exe+11B535C - 48 8D 81 F0010000     - lea rax,[rcx+000001F0]
;NewColossus_x64vk.exe+11B5363 - EB 02                 - jmp NewColossus_x64vk.exe+11B5367
;NewColossus_x64vk.exe+11B5365 - FF D2                 - call rdx
;NewColossus_x64vk.exe+11B5367 - 48 8B 48 48           - mov rcx,[rax+48]
;NewColossus_x64vk.exe+11B536B - 8B 51 30              - mov edx,[rcx+30]						<< READ g_runFrames.
;NewColossus_x64vk.exe+11B536E - 85 D2                 - test edx,edx							<< CONTINUE HERE
;NewColossus_x64vk.exe+11B5370 - 7E 07                 - jle NewColossus_x64vk.exe+11B5379
;NewColossus_x64vk.exe+11B5372 - FF CA                 - dec edx
;NewColossus_x64vk.exe+11B5374 - E8 07AC0700           - call NewColossus_x64vk.exe+122FF80
;originalCode:
;	cmp rax,r8							
;	jne c1
;	lea rax,[rcx+000001F0h]
;	jmp c2
;c1:
;	call rdx
;c2:
;	mov rcx,[rax+48h]
;	mov edx,[rcx+30h]
;exit:
;	mov [_g_runFramesStructAddress], rcx    ; Store the address in the variable
;	jmp dword ptr [__runFramesAddressInterceptionContinue] ; jmp back into the original game code, which is the location after the original statements above.
;runFramesAddressInterceptor ENDP

END