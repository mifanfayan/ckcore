/*
 * The ckCore library provides core software functionality.
 * Copyright (C) 2006-2009 Christian Kindahl
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

#include <ckcore/exception.hh>  // The include file for this module comes first.
#include <assert.h>
#include <stdarg.h>
#ifdef _WINDOWS
#include <comdef.h>
#endif
#include <ckcore/string.hh>

namespace ckcore
{
    /**
     * Construct an Exception object.
     * @param [in] err_msg The error message.
     */
    Exception2::Exception2(const ckcore::tchar * const err_msg)
    {
#if defined(_WINDOWS) && defined(_UNICODE)
        int utf16_size = lstrlen(err_msg) + 1;
        int utf8_size = WideCharToMultiByte(CP_UTF8,0,err_msg,utf16_size,NULL,
                                            0,NULL,NULL);

        assert(utf8_size != 0);

        err_msg_.reserve(utf8_size);

        utf8_size = WideCharToMultiByte(CP_UTF8,0,err_msg,utf16_size,
                                        const_cast<char *>(err_msg_.c_str()),
                                        utf8_size,NULL,NULL);
#else
		err_msg_ = err_msg;
#endif
    }

    /**
     * Construct an Exception object.
     * @param [in] err_msg The error message.
     */
    Exception2::Exception2(const ckcore::tstring &err_msg)
    {
#if defined(_WINDOWS) && defined(_UNICODE)
		int utf16_size = static_cast<int>(err_msg.size()) + 1;
		int utf8_size = WideCharToMultiByte(CP_UTF8,0,err_msg.c_str(),
											utf16_size,NULL,
                                            0,NULL,NULL);

        assert(utf8_size != 0);

        err_msg_.reserve(utf8_size);

		utf8_size = WideCharToMultiByte(CP_UTF8,0,err_msg.c_str(),utf16_size,
                                        const_cast<char *>(err_msg_.c_str()),
                                        utf8_size,NULL,NULL);
#else
		err_msg_ = err_msg;
#endif
    }

    /**
     * Returns the error message ANSI/UTF-8 format.
     * @return The error message.
     */
    const char *Exception2::what(void) const throw()
    {
       return err_msg_.c_str();
    }

	/**
     * Returns the error message in tstring format.
     * @return The error message.
     */
	tstring Exception2::message() const
	{
#if defined(_WINDOWS) && defined(_UNICODE)
		const int utf8_size = static_cast<int>(err_msg_.size()) + 1;

		int utf16_size = MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,
											 err_msg_.c_str(),utf8_size,NULL,0);

		assert(utf16_size != 0);

		tstring result;
		result.reserve(utf16_size + 1);

		utf16_size = MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,
										 err_msg_.c_str(),utf8_size,
										 const_cast<wchar_t *>(result.c_str()),
										 utf16_size);

		return result;
#else
		return err_msg_;
#endif
	}

    /**
     * Returns the exception message of the given exception object. The
     * function tests if the exception is an Exception2 and in that case
     * returns its message.
     * @param [in] e The exception object.
     * @return The error message of the exception.
     */
    ckcore::tstring get_except_msg(const std::exception &e)
    {
        const Exception2 *const ptr = dynamic_cast<const Exception2 *>(&e);
		if (ptr != NULL)
			return ptr->message();

		return ckcore::string::ansi_to_auto<1024>(e.what());
    }

    /**
     * Rethrows the exception with a prefix message.
     * @param [in] e The exception to rethrow.
     * @param [in] szFormatStr The message format string.
     */
    void rethrow_with_pfx(const std::exception &e,const ckcore::tchar * const fmt,...)
    {
        ckcore::tstring msg;

        va_list args;
        va_start(args,fmt);

        ckcore::string::vformatstr(msg,fmt,args);

        va_end(args);

        const ckcore::tstring err_msg = get_except_msg(e);
        msg.append(err_msg);

        throw Exception2(msg);
    }

#ifdef _WINDOWS
    void throw_from_hresult(const HRESULT res,const tchar * const pfx_fmt,...)
    {
        tstring msg;

        if (pfx_fmt != NULL)
        {
            va_list args;
            va_start(args,pfx_fmt);

			string::vformatstr(msg,pfx_fmt,args);

            va_end(args);
        }

        const tstring err_msg = _com_error(res).ErrorMessage();
        msg.append(err_msg);

        throw Exception2(msg);
    }
#endif  // #ifdef _WINDOWS
}  // namespace ckcore