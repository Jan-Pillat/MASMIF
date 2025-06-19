#ifndef _HPP_StrUniLoadNumber_
#define _HPP_StrUniLoadNumber_

void ASM_CODE ();

extern "C" void Get_number (long double* destination, char* txt);

template <typename T>
T StrGetNum (char* txt)
{
    long double result = 0.0;

    Get_number  (&result, txt); //result will be in RCX, and txt will be in RDX

    return (T)result;
}

#endif //_HPP_StrUniLoadNumber_
