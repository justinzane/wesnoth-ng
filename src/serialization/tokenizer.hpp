/**
 * @authors
 *    Philippe Plantier <ayin@anathas.org>,
 *    Guillaume Melquiond <guillaume.melquiond@gmail.com>
 * @copybrief Copyright (C) 2004 - 2009 by Philippe Plantier <ayin@anathas.org>
 * @copybrief Copyright (C) 2010 - 2013 by Guillaume Melquiond <guillaume.melquiond@gmail.com>
 * @file tokenizer.hpp
 * TODO: WRITEME
 * Part of the Battle for Wesnoth Project http://www.wesnoth.org
 * @section LICENSE GPLv2
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY.
 *    See the COPYING file for more details.
 */

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED

#include "buffered_istream.hpp"

#include <istream>
#include <string>

/**
 * TODO: WRITEME
 */
class config;

/**
 * Class enumerating all token types.
 * @note: cxx0x only?
 */
enum class token_type {
    STRING,                 //!< STRING
    QSTRING,                //!< QSTRING
    UNTERMINATED_QSTRING,   //!< UNTERMINATED_QSTRING
    MISC,                   //!< MISC

    LF = '\n',              //!< LF
    EQUALS = '=',           //!< EQUALS
    COMMA = ',',            //!< COMMA
    PLUS = '+',             //!< PLUS
    SLASH = '/',            //!< SLASH
    OPEN_BRACKET = '[',     //!< OPEN_BRACKET
    CLOSE_BRACKET = ']',    //!< CLOSE_BRACKET
    UNDERSCORE = '_',       //!< UNDERSCORE
    END                     //!< END
};



/**
 * @struct token
 */
struct token {
    token_type type;
    std::string value;
};

/** Abstract base class for the tokenizer. */
class tokenizer
{
    public:
        /** Constructor
         * @param in_strm [in] input string stream to be tokenized. */
        tokenizer(std::istream& in_strm);

        /** Destructor */
        ~tokenizer();

        /** @return the next token from the input stream */
        const token &next_token();

        /** @return the current token from the input stream */
        const token &current_token() const
        { return token_; }

        /** @return the current token from the input stream */
        const token &previous_token() const
        { return previous_token_; }

        /** @return the textdomain of TODO: WRITEME */
        const std::string &textdomain() const
        { return textdomain_; }

        /** @return TODO: WRITEME */
        const std::string &get_file() const
        { return file_; }

        /** @return TODO: WRITEME */
        int get_start_line() const
        { return start_line_num_; }

    private:
        /** Private Members ==================================================================*/
        int current_;           /**< TODO: WRITEME */
        int cur_line_num_;      /**< TODO: WRITEME */
        int start_line_num_;    /**< TODO: WRITEME */
        std::string textdomain_;
        std::string file_;
        token token_;
        token previous_token_;
        buffered_istream in_;
        char char_types_[128];

        /** Private Methods ==================================================================*/

        /** Private Default Constructor. */
        tokenizer();

        void next_char();

        void next_char_fast() {
            do {
                current_ = in_.get();
            } while (UNLIKELY(current_ == '\r'));
#if 0
            /// @todo disabled untill campaign server is fixed
            if(LIKELY(in_.good())) {
                current_ = in_.get();
                if (UNLIKELY(current_ == '\r'))
                {
                    // we assume that there is only one '\r'
                    if(LIKELY(in_.good())) {
                        current_ = in_.get();
                    } else {
                        current_ = EOF;
                    }
                }
            } else {
                current_ = EOF;
            }
#endif
        }

        int peek_char()
        {
            return in_.peek();
        }

        enum
        {
            TOK_SPACE = 1,
                TOK_NUMERIC = 2,
                TOK_ALPHA = 4
        };

        int char_type(unsigned c) const
                      {
            return c < 128 ? char_types_[c] : 0;
        }

        bool is_space(int c) const
                      {
            return char_type(c) & TOK_SPACE;
        }

        bool is_num(int c) const
                    {
            return char_type(c) & TOK_NUMERIC;
        }

        bool is_alnum(int c) const
                      {
            return char_type(c) & (TOK_ALPHA | TOK_NUMERIC);
        }

        void skip_comment();

        /**
         * Returns true if the next characters are the one from @a cmd
         * followed by a space. Skips all the matching characters.
         */
        bool skip_command(char const *cmd);

};

#endif

