
#include <windows.h>

using namespace std;


char errorDescribe[][0x30]
{
    {"UNDEFINED"},
    {"All correct."},
    {"Can't open!  (INVALID_HANDLE_VALUE)"},//{"INVALID_HANDLE_VALUE"},
    {"Incorrect size!  (0xFFFFFFFF)"},      //{"INCORRECT_SIZE"},
    {"NO MEMORY!"},                         //{"NO_MEMORY"},
    {"Can't read file!"},
    {"Not all data has been loaded!"}
};


class jf_fileData
{
    enum errorType
    {
        ERR_UNDEF = -1,
        ERR_NO_ERROR,
        ERR_INVALID_HANDLE_VALUE,
        ERR_INCORRECT_SIZE,
        ERR_NO_MEMORY,
        ERR_CANT_READ,
        ERR_NOT_ALL_DATA
    };


    private:
    HANDLE      fileHandle  = INVALID_HANDLE_VALUE;
    DWORD       size        = NULL;
    char*       data        = NULL;
    bool        isText      = false;


    public:
    errorType   lastError   = ERR_NO_ERROR;


    void    Remove  ()
    {
        size      = NULL;
        lastError = ERR_NO_ERROR;

        if (data)
            delete data;

        if (fileHandle != INVALID_HANDLE_VALUE )
            CloseHandle (fileHandle);
    }


    ~jf_fileData()
    {
        Remove ();
    }

    jf_fileData ()
    {
        Remove ();
    }

    jf_fileData (const jf_fileData& otherData)
    {
        Remove ();

        if (otherData.IsGood())
        {
            size = otherData.size;
            data = new char [size];
            memcpy (data, otherData.data, size);
            isText = otherData.isText;
            lastError = otherData.lastError;
        }
        else
        {
            Remove ();
            lastError = ERR_UNDEF;
        }
    }

    private:
    // ------------------- OPEN AND ALLOCATE -------------------
    errorType LoadFile (const string& path, int sizeAddition)
    {
        Remove();  //If some data is loaded, it will be removed for new data.

        fileHandle = CreateFile (&path[0],        //File path
                                 GENERIC_READ,    //The target is read data.
                                 FILE_SHARE_READ, //Other programs can also read this file.
                                 NULL,            //NOT IMPORTANT
                                 OPEN_EXISTING,   //Opens file, and if it not exist, returns error.
                                 NULL,            //NOT IMPORTANT
                                 NULL);           //NOT IMPORTANT

        if (fileHandle == INVALID_HANDLE_VALUE )
            return ERR_INVALID_HANDLE_VALUE;

        DWORD fileSize = GetFileSize (fileHandle, NULL);

        if (fileSize == -1  ||  fileSize == 0)
            return ERR_INCORRECT_SIZE;

        data = new char[fileSize+sizeAddition];

        if (data == NULL)
            return ERR_NO_MEMORY;

        DWORD loadedCount = 0;

        if (!ReadFile (fileHandle, data, fileSize, &loadedCount, NULL))
            return ERR_CANT_READ;

        if (loadedCount != fileSize)
            return ERR_NOT_ALL_DATA;

        size = loadedCount + sizeAddition;   //Odczyt siê powiód³, wiêc mo¿na podaæ ile odczytano bajtów.

        return ERR_NO_ERROR;
    }



    bool TryToLoadFile (const string& path, int sizeAddition)
    {
        errorType result = LoadFile(path, sizeAddition);

        if (result != ERR_NO_ERROR)
        {
            Remove ();
            lastError = result;
            return false;
        }

        return true;
    }



    public:

    // ------------------- BINARY DATA -------------------

    bool LoadBinaryFile (const string& path)
    {
        return TryToLoadFile (path, 0);
    }



    // ------------------- TEXT DATA -------------------

    void ExchangeNullChars ()
    {
        if (IsGood())
        {
            char* current  = data;
            char* end      = data + size - 1;

            while (current != end)
                (*current == '\0')  ? *current++='\n'  : *current++;

            data[size] = '\0';
        }
    }

    bool LoadTextFile (const string& path)
    {
        bool result = TryToLoadFile (path, 1);

        isText = true;
        ExchangeNullChars ();

        return result;
    }



    // ------------------- INTERACT WITH DATA -------------------

    int Size        ()  const   { return size; }
    int GetSize     ()  const   { return size; }
    int Length      ()  const   { return size; }
    int GetLength   ()  const   { return size; }

    bool IsGood     ()  const   { return ( size > 0 && data != 0); }
    bool IsText     ()  const   { return isText; }

    char* GetErrorDescribe ()
    {
        return &errorDescribe [lastError+1][0];
    }

    char& operator [] (int i)
    {
        return this->data[i];
    }

    jf_fileData& operator = (const  jf_fileData&  other)
    {
        if (this == &other) return *this;

        this->Remove();
        this->data = new char [other.size];

        memcpy (this->data, other.data, other.size);
        this->size = other.size;

        lastError = other.lastError;
        return *this;
    }

    jf_fileData& operator += (const  jf_fileData&  other)
    {
        int   newSize = size + other.size;

        if (isText) newSize -= 1;   //Remove NULL char

        char* newData = new char [newSize];

        memcpy (newData, data, size);
        if (isText)
            memcpy (newData+size-1, other.data, other.size);
        else
            memcpy (newData+size, other.data, other.size);

        delete data;
        data = newData;
        size = newSize;

        if (isText)
            ExchangeNullChars ();

        return *this;
    }

    friend jf_fileData operator + (const  jf_fileData&  left,  const  jf_fileData&  right)
    {
        jf_fileData newFileData (left);

        newFileData += right;

        return newFileData;
    }

    bool operator == (const  jf_fileData&  other)
    {
        if (this->size != other.size)
        {
            return false;
        }

        return  !memcmp (this->data, other.data, this->size);
    }
};


// -------------------  LIBRARY  TEST  -------------------


void jf_LibraryTest ()
{
    cout << " ----- JanFile - Library Test ----- \n\n";

    jf_fileData fd1, fd2;

    cout << "Try to open files: jf_test1.txt and jf_test2.txt\n\n";
    fd1.LoadTextFile("jf_test1.txt");
    fd2.LoadTextFile("jf_test2.txt");


    // -------------------- OPEN FILES --------------------
    if (!fd1.IsGood())
    {
        cout << "jf_test1.txt  load  failed \n";
        return;
    }
    else
        cout << "jf_test1.txt  load  success \n";

    if (!fd2.IsGood())
    {
        cout << "jf_test2.txt  load  failed \n";
        return;
    }
    else
        cout << "jf_test2.txt  load  success \n";


    // -------------------- PRINT DATA --------------------

    cout << "jf_test1.txt  data:" << &fd1[0] << endl;
    cout << "jf_test2.txt  data:" << &fd2[0] << endl;


    // -------------------- COMPARE FILES --------------------
    if (fd1 == fd2)
        cout << "They are the same.\n";
    else
        cout << "They AREN't the same.\n";


    // --------------------  --------------------

    cout << endl;
    jf_fileData  previous_fd1 = fd1;
    fd1 += fd2;
    cout << "File1 += File2:" << &fd1[0] << endl << endl;
    fd1 = fd2;
    cout << "File1 = File2:" << &fd1[0] << endl << endl;
    fd1 = fd2 + previous_fd1 + fd2;
    cout << "File1 = File2 + previous File1 + File2:" << &fd1[0] << endl << endl;
}
