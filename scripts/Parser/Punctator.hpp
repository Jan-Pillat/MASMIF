#ifndef _HPP_Punctator_
#define _HPP_Punctator_

#include "../Token.hpp"

    enum ReadType
    {
        ACCEPT_LINE_END_AND_FILE_END,   //Save every line end and file end ('\0') is also termination - it will be not incorrect
        DONT_ACCEPT_FILE_END,           //Save line ends, but if you found file end instead own terminate, state that it's ERROR
        DONT_ACCEPT_LINE_END,           //If you found line end ('\r'or'\n') or file end ('\0'), state that it's ERROR
        LINE_END_IS_ALSO_TERMINATION,   //Apart from own termination, line end and file end are also correct termination
        LINE_END_AND_FILE_END_ARE_ALSO_TERMINATION = LINE_END_IS_ALSO_TERMINATION,
    };

    enum Special
    {
        SAVE_ALL_SPECIALS,
        APPLY_ALL_SPECIALS,
        APPLY_ONLY_OWN_TERMINATE_SPECIAL,
    };

    enum SpecialBegins
    {
        NONE,
        IGNORE_ANOTHER_BEGIN,
        DONT_ACCEPT_ANOTHER_BEGIN,
    };

    struct punctator
    {
        char                sign;
        char                termination;
        enum TokenType      type;
        enum ReadType       attitudeTowardsLineEndsAndFileEnd;
        enum Special        attitudeTowardsSpecials;
        enum SpecialBegins  attitudeTowardsAnotherBegin;
        char                name[0x10];
    };

#endif
