/* 
  Ticker.h - esp32 library that calls functions periodically

  Copyright (c) 2017 Bert Melis. All rights reserved.
  
  Based on the original work of:
  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
  The original version is part of the esp8266 core for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef TICKER_H
#define TICKER_H

extern "C" {
  #include "esp_timer.h"
}
//#include <type_traits> // C++20 version has the std::type_identity_t built in

class Ticker
{
public:
  Ticker();
  ~Ticker();
  using callback_t = void (*)(void);
  using callback_with_arg_t = void (*)(void*);
  ////////////////////// OBSOLETE when xtensa32 toolchain has C++20 by default:
  // std::type_identity_t is part of C++20 but current toolchain uses C++11.
  // The following block is backporting the feature and will be obsolete soon.
  // When this is the case, include <type_traits> and use std::type_indentity_t
  template<typename T>
  struct type_identity {using type = T;};
  template<class T>
  using type_identity_t = typename type_identity<T>::type;
  ////////////////////// End soon to be obsolete part

  void attach(float seconds, callback_t callback)
  {
    _attach_ms(seconds * 1000, true, reinterpret_cast<callback_with_arg_t>(callback), 0);
  }

  void attach_ms(uint32_t milliseconds, callback_t callback)
  {
    _attach_ms(milliseconds, true, reinterpret_cast<callback_with_arg_t>(callback), 0);
  }

  template<typename TArg>
  // "type_identity_t": Prevents the compiler from attempting template type deduction
  // on the first argument. This allows us to directly pass-in a non-capturing
  // lambda function which can be implicitly converted into a void (*)(void*)
  // function pointer - which however prevents automatic template type deduction.
  // That is no problem when type can be deduced from second argument like follows:
  void attach(float seconds, void (*callback)(type_identity_t<TArg>), TArg arg)
  {
    static_assert(sizeof(TArg) <= sizeof(uint32_t), "attach() callback argument size must be <= 4 bytes");
    // C-cast serves two purposes:
    // static_cast for smaller integer types,
    // reinterpret_cast + const_cast for pointer types
    uint32_t arg32 = (uint32_t)arg;
    _attach_ms(seconds * 1000, true, reinterpret_cast<callback_with_arg_t>(callback), arg32);
  }

  template<typename TArg>
  void attach_ms(uint32_t milliseconds, void (*callback)(type_identity_t<TArg>), TArg arg)
  {
    static_assert(sizeof(TArg) <= sizeof(uint32_t), "attach_ms() callback argument size must be <= 4 bytes");
    uint32_t arg32 = (uint32_t)arg;
    _attach_ms(milliseconds, true, reinterpret_cast<callback_with_arg_t>(callback), arg32);
  }

  void once(float seconds, callback_t callback)
  {
    _attach_ms(seconds * 1000, false, reinterpret_cast<callback_with_arg_t>(callback), 0);
  }

  void once_ms(uint32_t milliseconds, callback_t callback)
  {
    _attach_ms(milliseconds, false, reinterpret_cast<callback_with_arg_t>(callback), 0);	
  }

  template<typename TArg>
  void once(float seconds, void (*callback)(type_identity_t<TArg>), TArg arg)
  {
    static_assert(sizeof(TArg) <= sizeof(uint32_t), "attach() callback argument size must be <= 4 bytes");
    uint32_t arg32 = (uint32_t)(arg);
    _attach_ms(seconds * 1000, false, reinterpret_cast<callback_with_arg_t>(callback), arg32);
  }

  template<typename TArg>
  void once_ms(uint32_t milliseconds, void (*callback)(type_identity_t<TArg>), TArg arg)
  {
    static_assert(sizeof(TArg) <= sizeof(uint32_t), "attach_ms() callback argument size must be <= 4 bytes");
    uint32_t arg32 = (uint32_t)(arg);
    _attach_ms(milliseconds, false, reinterpret_cast<callback_with_arg_t>(callback), arg32);
  }

  void detach();
  bool active();

protected:	
  void _attach_ms(uint32_t milliseconds, bool repeat, callback_with_arg_t callback, uint32_t arg);


protected:
  esp_timer_handle_t _timer;
};


#endif  // TICKER_H
