/* -*- mode: c; indent-tabs-mode: nil; tab-width: 2
  sdl-fakeqwerty/xlib-hooks.c
  Copyright 2017 Michael Farrell <micolous+git@gmail.com>
 
  Hook some Xlib related input functionality and make it look like there is a
  QWERTY keyboard attached for applications which read the "KeySym".

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
  
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
  
  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
     
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
     
  3. This notice may not be removed or altered from any source distribution.
*/

#define _GNU_SOURCE
#include <dlfcn.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

const char* qwerty_syms = "1234567890-=\0\0qwertyuiop[]\0\0asdfghjkl;'`\0\\zxcvbnm,./";
const char* qwerty_syms_shifted = "!@#$%^&*()_+\0\0QWERTYUIOP{}\0\0ASDFGHJKL:\"~\0|ZXCVBNM<>?";


typedef KeySym* (*orig_XGetKeyboardMapping_sig)(Display *display, KeyCode first_keycode, int keycode_count, int *keysyms_per_keycode_return);
typedef int (*orig_XLookupString_sig)(XKeyEvent *event_struct, char *buffer_return, int bytes_buffer, KeySym *keysym_return, XComposeStatus *status_in_out);



KeySym* XGetKeyboardMapping(Display *display, KeyCode first_keycode, int keycode_count, int *keysyms_per_keycode_return) {
  orig_XGetKeyboardMapping_sig orig_XGetKeyboardMapping = (orig_XGetKeyboardMapping_sig)dlsym(RTLD_NEXT, "XGetKeyboardMapping");
  
  KeySym* ret = orig_XGetKeyboardMapping(display, first_keycode, keycode_count, keysyms_per_keycode_return);
  
  for (int x=0; x<keycode_count * (*keysyms_per_keycode_return); x++) {
    int keycode = first_keycode + (x / (*keysyms_per_keycode_return));
    int keysym_no = x % (*keysyms_per_keycode_return);
    
    if (keycode >= 0x0a && keycode <= 0x3d) {
      unsigned long newcode = 0;
      switch (keysym_no) {
        case 0:
        case 2: // Regular
          newcode = (unsigned long)qwerty_syms[keycode - 0x0a];
          break;
        
        case 1:
        case 3: // SHIFT
          newcode = (unsigned long)qwerty_syms_shifted[keycode - 0x0a];
          break;
      }
      
      if (newcode != 0) {
        ret[x] = newcode;
      }
    }
  }
  
  return ret;
}


int XLookupString(XKeyEvent *event_struct, char *buffer_return, int bytes_buffer, KeySym *keysym_return, XComposeStatus *status_in_out) {

  // Try translating the event by a QWERTY keymap first
  if (event_struct->keycode >= 0x0a && event_struct->keycode <= 0x3d) {
    if (event_struct->state & ShiftMask) {
      buffer_return[0] = qwerty_syms_shifted[event_struct->keycode - 0x0a];    
    } else {
      buffer_return[0] = qwerty_syms[event_struct->keycode - 0x0a];
    }
    
    if (buffer_return[0] != '\0') {
      if (keysym_return != NULL) {
        *keysym_return = (unsigned long)buffer_return[0];
      }
      //printf("%04x : %c (%02x)\n", event_struct->keycode, buffer_return[0], buffer_return[0]);
      return 1;
    }
  }

  // Fall back to calling the real function
  orig_XLookupString_sig orig_XLookupString = (orig_XLookupString_sig)dlsym(RTLD_NEXT, "XLookupString");

  int ret = orig_XLookupString(event_struct, buffer_return, bytes_buffer, keysym_return, status_in_out);
  //printf("%04x : fallback : %c (%02x)\n", event_struct->keycode, buffer_return[0], buffer_return[0]);
  return ret;
}

int XmbLookupString(XIC ic, XKeyPressedEvent *event, char *buffer_return, int bytes_buffer, KeySym *keysym_return, Status *status_return) {
  // Wrap this into XLookupString.  This breaks multi-byte input.
  int len = XLookupString(event, buffer_return, bytes_buffer, keysym_return, NULL);

  if (status_return != NULL) {
    if (len > 0) {
      *status_return = XLookupBoth;
    } else {
      *status_return = XLookupKeySym;
    }
  }

  return len;
}  

int Xutf8LookupString(XIC ic, XKeyPressedEvent *event, char *buffer_return, int bytes_buffer, KeySym *keysym_return, Status *status_return) {
  return XmbLookupString(ic, event, buffer_return, bytes_buffer, keysym_return, status_return);
}  

