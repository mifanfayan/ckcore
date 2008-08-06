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

#include <cxxtest/TestSuite.h>
#include <stdlib.h>
#include "../src/types.hh"
#include "../src/file.hh"

class FileTestSuite : public CxxTest::TestSuite
{
public:
    void testOpenCloseTest()
    {
        // Test non-existing file.
        ckCore::File file1(ckT("data/file/non-existent"));
        TS_ASSERT(!file1.Test());

        TS_ASSERT(!file1.Open(ckCore::File::ckOPEN_READ));
        TS_ASSERT(!file1.Test());
        TS_ASSERT(!file1.Close());
        TS_ASSERT(!file1.Test());

        TS_ASSERT(file1.Open(ckCore::File::ckOPEN_WRITE));
        TS_ASSERT(file1.Test());
        TS_ASSERT(file1.Close());
        TS_ASSERT(!file1.Test());

        TS_ASSERT(file1.Remove());
        TS_ASSERT(!file1.Test());
        TS_ASSERT(!file1.Close());

        // Test existing file.
        ckCore::File file2(ckT("data/file/0bytes"));
        TS_ASSERT(!file2.Test());

        TS_ASSERT(file2.Open(ckCore::File::ckOPEN_READ));
        TS_ASSERT(file2.Test());
        TS_ASSERT(file2.Close());
        TS_ASSERT(!file2.Test());

        TS_ASSERT(file2.Open(ckCore::File::ckOPEN_WRITE));
        TS_ASSERT(file2.Test());
        TS_ASSERT(file2.Close());
        TS_ASSERT(!file2.Test());
    }

    void testReadWrite()
    {
        ckCore::File file(ckT("data/file/new"));
        TS_ASSERT(file.Open(ckCore::File::ckOPEN_WRITE));
        const char out_data[] = "abcdefghijklmnopqrstuvwxyz0123456789";

        size_t tot_write = 0;
        while (tot_write < 37)
        {
            size_t write = file.Write(out_data + tot_write,37 - tot_write);
            TS_ASSERT(write != -1);

            tot_write += write;
        }

        file.Close();
        file.Open(ckCore::File::ckOPEN_READ);
        char in_data[37];

        size_t tot_read = 0;
        while (tot_read < 37)
        {
            size_t read = file.Read(in_data,37 - tot_read);
            TS_ASSERT(read != -1);

            tot_read += read;
        }

        file.Close();
        file.Remove();

        TS_ASSERT_SAME_DATA(in_data,out_data,37);
    }

    void testSeekTell()
    {
        ckCore::File file(ckT("data/file/8253bytes"));
        TS_ASSERT(file.Open(ckCore::File::ckOPEN_READ));
        TS_ASSERT(file.Test());

        // Test exteme values.
        TS_ASSERT(file.Seek(0,ckCore::File::ckFILE_BEGIN) == 0);
        TS_ASSERT(file.Tell() == 0);
        TS_ASSERT(file.Seek(0,ckCore::File::ckFILE_CURRENT) == 0);
        TS_ASSERT(file.Tell() == 0);

        TS_ASSERT(file.Seek(0,ckCore::File::ckFILE_END) == file.Size());
        TS_ASSERT(file.Tell() == file.Size());

        int file_size = 8253;

        // Test random seeking.
        for (unsigned int i = 0; i < 100; i++)
        {
            TS_ASSERT(file.Seek(0,ckCore::File::ckFILE_BEGIN) == 0);

            int seek1 = rand() % (file_size >> 1);
            int seek2 = rand() % (file_size >> 1);
            int seekckTot = seek1 + seek2;

            // Seek forward from the current position.
            TS_ASSERT(file.Seek(seek1,ckCore::File::ckFILE_CURRENT) == seek1);
            TS_ASSERT(file.Tell() == seek1);

            TS_ASSERT(file.Seek(seek2,ckCore::File::ckFILE_CURRENT) == seekckTot);
            TS_ASSERT(file.Tell() == seekckTot);

            // Seek backward from the current position.
            TS_ASSERT(file.Seek(-seek1,ckCore::File::ckFILE_CURRENT) == seek2);
            TS_ASSERT(file.Tell() == seek2);

            TS_ASSERT(file.Seek(-seek2,ckCore::File::ckFILE_CURRENT) == 0);
            TS_ASSERT(file.Tell() == 0);
        }
    }

    void testExistRemove()
    {
        ckCore::File file1(ckT("data/file/non-existent"));
        TS_ASSERT(!file1.Exist());
        TS_ASSERT(!file1.Remove());
        TS_ASSERT(file1.Open(ckCore::File::ckOPEN_WRITE));
        TS_ASSERT(file1.Exist());
        TS_ASSERT(file1.Remove());
        TS_ASSERT(!file1.Exist());

        ckCore::File file2(ckT("data/file/0bytes"));
        TS_ASSERT(file2.Exist());

        // Test static functions.
        TS_ASSERT(ckCore::File::Exist(ckT("data/file/0bytes")));
        TS_ASSERT(!ckCore::File::Exist(ckT("data/file/non-existent")));

        TS_ASSERT(!ckCore::File::Remove(ckT("data/file/non-existent")));
        ckCore::File file3(ckT("data/file/new"));
        TS_ASSERT(file3.Open(ckCore::File::ckOPEN_WRITE));
        TS_ASSERT(file3.Close());
        TS_ASSERT(ckCore::File::Remove(ckT("data/file/new")));
        TS_ASSERT(!ckCore::File::Remove(ckT("data/file/non-existent")));
        TS_ASSERT(!ckCore::File::Remove(ckT("")));
    }

    void testRename()
    {
        // Rename file in existing folder (should succeed).
        ckCore::File file1(ckT("data/file/new"));
        TS_ASSERT(file1.Open(ckCore::File::ckOPEN_WRITE));

        TS_ASSERT(file1.Rename(ckT("data/file/new2")));
        TS_ASSERT(ckCore::File::Exist(ckT("data/file/new2")));
        TS_ASSERT(!file1.Close());

        // Rename file in new non-existing folder (should fail).
        ckCore::File file2(ckT("data/file/new"));
        TS_ASSERT(file2.Open(ckCore::File::ckOPEN_WRITE));

        TS_ASSERT(!file2.Rename(ckT("data/file/new/new2")));
        TS_ASSERT(!ckCore::File::Exist(ckT("data/file/new/new2")));
        TS_ASSERT(!file2.Close());

        // Try to rename file to existing file (should fail).
        TS_ASSERT(!file2.Rename(ckT("data/file/new2")));

        // Clean up.
        TS_ASSERT(file1.Remove());
        TS_ASSERT(file2.Remove());

        // Try to rename non-existent file.
        ckCore::File file3(ckT("data/file/non-existent"));
        TS_ASSERT(!file3.Rename(ckT("data/file/new")));
        TS_ASSERT(!ckCore::File::Exist(ckT("data/file/new")));
        TS_ASSERT(!file3.Rename(ckT("data/file/non-existent")));
        TS_ASSERT(!ckCore::File::Exist(ckT("data/file/non-existent")));

        // Try to rename without name.
        TS_ASSERT(!file3.Rename(ckT("")));

        // Test static function.
        ckCore::File file4(ckT("data/file/new"));
        TS_ASSERT(file4.Open(ckCore::File::ckOPEN_WRITE));
        TS_ASSERT(file4.Close());
        TS_ASSERT(ckCore::File::Rename(ckT("data/file/new"),ckT("data/file/new2")));
        TS_ASSERT(!ckCore::File::Rename(ckT("data/file/new2"),ckT("data/file/new2")));
        TS_ASSERT(!ckCore::File::Rename(ckT("data/file/new2"),ckT("")));
        TS_ASSERT(!ckCore::File::Rename(ckT("data/file/new2"),ckT("data/file/new/new2")));
        TS_ASSERT(ckCore::File::Remove(ckT("data/file/new2")));
    }

    void testSize()
    {
        const ckCore::tchar *file_paths[] =
        {
            ckT("data/file/0bytes"),
            ckT("data/file/53bytes"),
            ckT("data/file/123bytes"),
            ckT("data/file/8253bytes")
        };

        const ckCore::tint64 file_sizes[] =
        {
            0,
            53,
            123,
            8253
        };

        for (unsigned int i = 0; i < 4; i++)
        {
            ckCore::File file(file_paths[i]);
            TS_ASSERT_EQUALS(file.Size(),file_sizes[i]);
            file.Open(ckCore::File::ckOPEN_READ);
            TS_ASSERT_EQUALS(file.Size(),file_sizes[i]);
            file.Close();
            /*file.Open(ckCore::File::ckOPEN_WRITE);
            TS_ASSERT_EQUALS(file.Size(),file_sizes[i]);
            file.Close();*/

            // Test static function.
            TS_ASSERT_EQUALS(ckCore::File::Size(file_paths[i]),file_sizes[i]);
        }
    }
};
