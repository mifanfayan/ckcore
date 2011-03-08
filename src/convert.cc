/*
 * The ckCore library provides core software functionality.
 * Copyright (C) 2006-2011 Christian Kindahl
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <algorithm>
#include <assert.h>
#include "ckcore/convert.hh"

namespace ckcore
{
    namespace convert
    {
        /**
         * A safe platform intependant version of the sprintf function.
         * @param [in] buffer The target string buffer.
         * @param [in] size The size of the buffer in bytes.
         * @param [in] format Format string.
         */
        void sprintf(tchar *buffer,size_t size,const tchar *format,...)
        {
            va_list ap;
            va_start(ap,format);

#ifdef _WINDOWS
#ifdef _UNICODE
            _vsnwprintf_s(buffer,size/sizeof(tchar),size/sizeof(tchar) - 1,format,ap);
#else
            _vsnprintf_s(buffer,size,size - 1,format,ap);
#endif
#else
            vsnprintf(buffer,size - 1,format,ap);
#endif
            va_end(ap);
        }

        /**
         * Converts the specified boolean value into a string.
         */
        void b_to_str2(bool value, tchar * buffer)
        {
            buffer[0] = value ? '1' : '0';
            buffer[1] = '\0';
        }

        /**
         * Most implementations of itoa() and printf() lock the locale settings
         * and are therefore much slower than this routine.
         */
        void ui64_to_str2(tuint64 value, tchar * buffer)
        {
          // Short-circuit the zero value, for it's very common.
          if (!value)
          {
            buffer[0]='0';
            buffer[1]='\0';
            return;
          }

          tchar * p=buffer;

          do
          {
            *p=tchar('0' + value%10);
            ++p;
            value/=10;
  
          } while(value);

          std::reverse(buffer, p);
          *p='\0';

          assert(p-buffer < INT_TO_STR_BUFLEN);
        }
      
        /**
         * Converts the specified big endian unsigned 32-bit integer value into
         * a little endian 32-bit unsigned integer value.
         * @param [in] value The integer value to convert.
         * @return The converted integer value.
         */
        tuint32 be_to_le32(tuint32 value)
        {
            unsigned char *temp = (unsigned char *)&value;

            return ((tuint32)temp[0] << 24) | ((tuint32)temp[1] << 16) |
                   ((tuint32)temp[2] <<  8) | temp[3];
        }

        /**
         * Converts the specified big endian unsigned 16-bit integer value into
         * a little endian 16-bit unsigned integer value.
         * @param [in] value The integer value to convert.
         * @return The converted integer value.
         */
        tuint16 be_to_le16(tuint16 value)
        {
            unsigned char *temp = (unsigned char *)&value;

            return ((tuint16)temp[0] << 8) | temp[1];
        }

        /**
         * Converts a tm structure into DOS date and time format.
         * @param [in] tm The input time structure.
         * @param [out] dos_date The date in DOS format.
         * @param [out] dos_time The time in DOS format.
         */
        void tm_to_dostime(struct tm &time,unsigned short &dos_date,
                           unsigned short &dos_time)
        {
            dos_date = time.tm_mday;
            dos_date |= (unsigned short)(time.tm_mon + 1) << 5;
            dos_date |= (unsigned short)(time.tm_year - 80) << 9;

            dos_time = time.tm_sec >> 1;
            dos_time |= (unsigned short)time.tm_min << 5;
            dos_time |= (unsigned short)time.tm_hour << 11;
        }
    }
}
