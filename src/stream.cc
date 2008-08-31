/*
 * The ckCore library provides core software functionality.
 * Copyright (C) 2006-2008 Christian Kindahl
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

#include "ckcore/system.hh"
#include "ckcore/stream.hh"

namespace ckcore
{
    namespace stream
    {
        /**
         * Copies the contents of the input stream to the output stream. An
         * internal buffer is used to optimize the process.
         * @param [in] from The source stream.
         * @param [in] to The target stream.
         * @return If successfull true is returned, otherwise false is
         *         returned.
         */
        bool copy(InStream &from,OutStream &to)
        {
            tuint32 buffer_size = System::Cache(System::ckLEVEL_1);
            if (buffer_size == 0)
                buffer_size = 8192;

            unsigned char *buffer = new unsigned char[buffer_size];
            if (buffer == NULL)
                return false;

            tint64 res = 0;
            while (!from.End())
            {
                res = from.Read(buffer,buffer_size);
                if (res == -1)
                {
                    delete [] buffer;
                    return false;
                }

                res = to.Write(buffer,(tuint32)res);
                if (res == -1)
                {
                    delete [] buffer;
                    return false;
                }
            }

            delete [] buffer;
            return true;
        }

		/**
         * Copies the contents of the input stream to the output stream. An
         * internal buffer is used to optimize the process. Progress is
		 * reported through the Progress interface.
         * @param [in] from The source stream.
         * @param [in] to The target stream.
		 * @param [in] progress The progress interface to report progress to.
         * @return If successfull true is returned, otherwise false is
         *         returned. Cancelling the operation is considered a failure.
         */
        bool copy(InStream &from,OutStream &to,Progress &progress)
        {
            tuint32 buffer_size = System::Cache(System::ckLEVEL_1);
            if (buffer_size == 0)
                buffer_size = 8192;

            unsigned char *buffer = new unsigned char[buffer_size];
            if (buffer == NULL)
                return false;

			// Initialize progress.
			tint64 total = from.Size(),written = 0;
			progress.SetMarquee(total == -1);

            tint64 res = 0;
            while (!from.End())
            {
				// Check if we should cancel.
				if (progress.Cancelled())
					return false;

                res = from.Read(buffer,buffer_size);
                if (res == -1)
                {
                    delete [] buffer;
                    return false;
                }

                res = to.Write(buffer,(tuint32)res);
                if (res == -1)
                {
                    delete [] buffer;
                    return false;
                }

				// Update progress.
				if (total != -1)
				{
					written += res;
					progress.SetProgress((unsigned char)((written * 100)/total));
				}
            }

			// Set to 100 in case of rounding errors.
			if (total != -1)
				progress.SetProgress(100);

            delete [] buffer;
            return true;
        }

		/**
         * Copies the contents of the input stream to the output stream. An
         * internal buffer is used to optimize the process. Progress is
		 * reported through a Progresser object.
         * @param [in] from The source stream.
         * @param [in] to The target stream.
		 * @param [in] progresser A reference to the progresser object to use
		 *                        for reporting progress.
         * @return If successfull true is returned, otherwise false is
         *         returned. Cancelling the operation is considered a failure.
         */
        bool copy(InStream &from,OutStream &to,Progresser &progresser)
        {
            tuint32 buffer_size = System::Cache(System::ckLEVEL_1);
            if (buffer_size == 0)
                buffer_size = 8192;

            unsigned char *buffer = new unsigned char[buffer_size];
            if (buffer == NULL)
                return false;

            tint64 res = 0;
            while (!from.End())
            {
				// Check if we should cancel.
				if (progresser.Cancelled())
					return false;

                res = from.Read(buffer,buffer_size);
                if (res == -1)
                {
                    delete [] buffer;
                    return false;
                }

                res = to.Write(buffer,(tuint32)res);
                if (res == -1)
                {
                    delete [] buffer;
                    return false;
                }

				// Update progress.
				progresser.Update(res);
            }

            delete [] buffer;
            return true;
        }
    };
};

