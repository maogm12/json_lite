///////////////////////////////////////////////////////////////////////////////
///  Copyright 2012 Garnel
///
///  Licensed under the Apache License, Version 2.0 (the "License");
///  you may not use this file except in compliance with the License.
///  You may obtain a copy of the License at
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
///  Unless required by applicable law or agreed to in writing, software
///  distributed under the License is distributed on an "AS IS" BASIS,
///  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///////////////////////////////////////////////////////////////////////////////

///
/// \file       json_lite.h
/// The declaration of json_value and json_class
/// \author     Garnel
/// \date       2012/09/15
/// \version    2.1
/// \copyright  Apache License, Version 2.0
///

#ifndef JSON_LITE
#define JSON_LITE

#include <iostream>
#include <string>
#include <cassert>
#include <fstream>

const int BUF_SIZE = 1024;  ///< the size of buffer

///
/// \namespace  json_lite
/// \brief      Include json_value and json_parser
///
namespace json_lite
{
    ///
    /// \enum   json_type
    /// \brief  The types of json element
    ///
    enum json_type
    {
        JSON_STRING,
        JSON_NUMBER,
        JSON_OBJECT,
        JSON_ARRAY,
        JSON_TRUE,
        JSON_FALSE,
        JSON_NULL
    };
    
    ///
    /// \enum   json_parse_error
    /// \brief  The errors in json parse
    ///
    enum json_parse_error
    {
        SHOULD_BE_OBJECT_OR_ARRAY,

        INVALID_TYPE,
        INVALID_CHARACTER,
        INVALID_ESCAPE_CHARACTER,

        INVALID_BASE,
        INVALID_EXPONENT,
        TOO_MANY_DOTS_IN_NUMBER,
        LEADING_ZERO,
        INVALID_SIGN_POSITION,
        MISSING_DIGIT,
        TOO_MANY_SIGNS_IN_NUMBER,
        TOO_MANY_EXPONENTS,

        ERROR_IN_TRUE,
        ERROR_IN_FALSE,
        ERROR_IN_NULL,

        MISSING_COLON,
        MISSING_QUOTATION,
        EXTRA_COMMA,
        EMPTY_VALUE,
        
        UNCLOSED_OBJECT,
        UNCLOSED_ARRAY,

        EXTRA_CONTENT_AFTER_JSON
    };

    ///
    /// \fn         error_value
    /// \brief      Return the parse error
    /// \param      error_type  The type of the error
    /// \return     The error message
    ///
    std::string error_value(json_parse_error error_type);


    ///////////////////////////////////////////////////////////////////////////
    /// json_value
    ///////////////////////////////////////////////////////////////////////////
    
    ///
    /// \class  json_value
    /// \brief  The json element
    /// \todo   Maybe there should be class json_string, json_number blah blah
    ///         to clear the operations of each type of json element
    ///
    class json_value
    {
    public:
         ///
         /// \fn         json_value(json_type _type)
         /// \brief      The constructor of json_value
         /// \param      _type  The type of the element
         ///
         json_value(json_type _type);

        ///
        /// \overload    json_value(json_type _type, std::string _value)
        /// \brief       The constructor of json_value
        /// \param       _type  The type of the element
        /// \param       _value The value of the element
        ///
        json_value(json_type _type, std::string _value);

        ///
        /// \fn         ~json_value
        /// \brief      The destructor of json_value
        ///             It will free its children first
        ///
        ~json_value();

        ///
        /// \fn         get_type
        /// \brief      Return the type of the element
        ///
        json_type get_type() const;

        ///
        /// \fn         set_value
        /// \brief      Set the value of the element.
        ///             Set the value for the element by the type.
        /// \warning    DO NOT do check for the value passed in
        /// \todo       Check the value passed in
        ///
        void set_value(std::string _value);

        ///
        /// \fn         get_value
        /// \brief      Return the value of the element
        /// \warning    If the element is a string, the value will contain quotations
        ///             objects and arrays have NO value, the value will be a empty string
        ///
        std::string get_value() const;
        
        ///
        /// \fn         set_next
        /// \brief      Set the next node for the element
        ///
        void set_next(json_value *_next);  
        
        ///
        /// \fn         set_prev
        /// \brief      Set the previous node for the element
        ///
        void set_prev(json_value *_prev);

        ///
        /// \fn         set_parent
        /// \brief      Set the parent node for the element
        ///
        void set_parent(json_value *_parent);

        ///
        /// \fn         set_first_child
        /// \brief      Set the first child node for the element
        ///
        void set_first_child(json_value *_first_child);
        
        ///
        /// \fn         set_last_child
        /// \brief      Set the last child node for the element
        ///
        void set_last_child(json_value *_last_child);

        ///
        /// \fn         get_next
        /// \brief      Get the next node of the element
        ///
        json_value* get_next() const;

        ///
        /// \fn         get_prev
        /// \brief      Get the previous node of the element
        ///
        json_value* get_prev() const;

        ///
        /// \fn         get_parent
        /// \brief      Get the parent node of the element
        ///
        json_value* get_parent() const;

        ///
        /// \fn         get_first_child
        /// \brief      Get the first child node of the element
        ///
        json_value* get_first_child() const;

        ///
        /// \fn         get_last_child
        /// \brief      Get the last child node of the element
        ///
        json_value* get_last_child() const;

        ///
        /// \fn         add_child
        /// \brief      Add a new child to the current element
        /// \param      _child  The pointer to the child
        /// \warning    Element pass should NOT be a NULL
        ///
        void add_child(json_value* _child);

        ///
        /// \fn         add_pair
        /// \brief      Add a new pair to the object
        /// \param      _key    The string of the pair
        /// \param      _value  The value of the pair
        /// \warning    Element pass should NOT be a NULL
        ///             Only works for object
        ///
        void add_pair(std::string _key, json_value* _value);

        ///
        /// \fn         output(const std::string out_file, bool format = true, int indent_level = 0)
        /// \brief      Print the json tree
        /// \param      out_file        The output file name
        /// \param      format          Format the document
        /// \param      indent_level    The levels of indentation, default value is 0
        /// \return     true for success, false for failure
        ///
        bool output(const std::string out_file, bool format = true, int indent_level = 0);
        
        ///
        /// \overload   output(const char* out_file, bool format = true, int indent_level = 0)
        /// \brief      Print the json tree
        /// \param      out_file        The output file name
        /// \param      format          Format the document
        /// \param      indent_level    The levels of indentation, default value is 0
        /// \return     true for success, false for failure
        ///
        bool output(const char* out_file, bool format = true, int indent_level = 0);

        ///
        /// \overload   output(bool format = true, int indent_level = 0)
        /// \brief      Print the json tree to the standard output
        /// \param      format          Format the document, default value is true
        /// \param      indent_level    The levels of indentation, default value is 0
        /// \return     true for success, false for failure
        ///
        bool output(bool format = true, int indent_level = 0);

        ///
        /// \fn         print_json_object
        /// \brief      Print the json object
        /// \param      obj             The json object
        /// \param      format          Format the document
        /// \param      indent_level    The level of indentation default value is 0
        /// \return     true for success, false for failure
        ///
        friend bool print_json_object(json_value *obj, bool format, int indent_level = 0);
        
        ///
        /// \fn         print_json_array
        /// \brief      Print the json array
        /// \param      arr             The json array
        /// \param      format          Format the document in the output file
        /// \param      indent_level    The levels of indentation, default value is 0
        /// \return     true for success, false for failure
        ///
        friend bool print_json_array(json_value *arr, bool format, int indent_level = 0);

        ///
        /// \brief      Print the json value
        /// \param      elem            The json element
        /// \param      format          Format the document in the output file
        /// \param      indent_level    The levels of indentation, default value is 0
        /// \return     true for success, false for failure
        ///
        friend bool print_json_value(json_value *elem, bool format, int indent_level = 0);

    private:
        json_value(){}  ///< default constructor is not allowed to use

    private:
        json_type type;             ///< the type of the element
        std::string value;          ///< the value of the element
        json_value *next;           ///< the next element
        json_value *prev;           ///< the previous element
        json_value *parent;         ///< the parent element
        json_value *first_child;    ///< the first child
        json_value *last_child;     ///< the last_child
    };

    ///////////////////////////////////////////////////////////////////////////
    ///json_parser
    ///////////////////////////////////////////////////////////////////////////
   
    ///
    /// \class  json_parser
    /// \brief  The json parser
    /// \todo   Mainly focus on number parsing
    ///
    class json_parser
    {
    public:
        /// \fn         json_parser
        /// \brief      The constructor of json_parser
        /// \param      file_name   The json file name
        /// \exception  char*       If the file cannot be opened, throw a message
        ///
        json_parser(const std::string file_name);
        
        ///
        /// \fn         ~json_parser
        /// \brief      Destructer of the parser
        ///
        ~json_parser();

        ///
        /// \fn         run
        /// \brief      This function begins to parse the whole json file
        /// \return     The json element parsed
        ///
        json_value* run();

        ///
        /// \fn         escape_blank
        /// \brief      Escape blank characters and return the first none-blank character
        /// \note       Blank characters are: ' '(space), '\n', '\r', '\t'
        /// \warning    When the function ends, the cursor points to the FIRST none-blank character in the buffer
        /// \todo       There seems to be more blank characters
        /// \return     The first none-blank character in the parsing stream
        char escape_blank();

        ///
        /// \fn         parse_value
        /// \brief      Parse a complete element from buffer
        /// \param      _type   The type of element to be parsed
        /// \return     The json element parsed.
        ///
        json_value* parse_value(json_type _type);

        ///
        /// \fn         parse_string
        /// \brief      Parse a string
        /// \exception  json_parse_error    INVALID_ESCAPE_CHARACTER    A invalid character after '\'
        /// \return     The string value without quotations on two sides
        ///
        std::string parse_string();

        ///
        /// \brief      Parse a number.
        /// \exception  json_parse_error    TOO_MANY_DOTS_IN_NUMBER     More than one '.' exist in a number
        ///                                 INVALID_EXPONENT            Error in the exponent of a number
        ///                                 INVALID_BASE                Error in the base of a number
        /// \return     The value of the number in string
        ///
        std::string parse_number();

        ///
        /// \fn         parse_true
        /// \brief      Parse a true.
        /// \exception  json_parse_error    ERROR_IN_TRUE   Error in parsing "true"
        /// \return     String "true"
        ///
        std::string parse_true();

        ///
        /// \fn         parse_false
        /// \brief      Parse a false
        /// \exception  json_parse_error    ERROR_IN_FALSE  Error in parsing "false"
        /// \return     String "false"
        ///
        std::string parse_false();
        
        ///
        /// \fn         parse_null
        /// \brief      Parse a null
        /// \exception  json_parse_error    ERROR_IN_NULL   Error in parsing "null"
        /// \return     String "null"
        ///
        std::string parse_null();

        ///
        /// \fn         parse_object
        /// \brief      Parse a object
        /// \exception  json_parse_error    MISSING_QUOTATION   The string of a pair lose it left double quotation
        ///                                 MISSING_COLON       A pair lose ':'
        ///                                 INVALID_CHARACTER   Some unknown character in the element of a pair
        ///                                 Any exception in parsing string, number, true, false, null
        /// \return     A pointer to the json object
        ///
        json_value* parse_object();

        ///
        /// \fn         parse_array
        /// \brief      Parse a array
        /// \exception  json_parse_error    INVALID_CHARACTER   Some unknown character in the element
        ///                                 Any exception in parsing string, number, true, false, null
        /// \return     A pointer to the json object
        ///
        json_value* parse_array();

        ///
        /// \fn         get_char
        /// \brief      Get the current character in the buffer and move the cursor to the next character
        /// \warning    It will move the cursor to the next character
        /// \return     The current character
        ///
        const char get_char();

        ///
        /// \fn         get_current_char
        /// \brief      Just get the current character in the buffer and do nothing else
        /// \warning    It will NOT move the cursor to the next character
        /// \return     The current character
        ///
        const char get_current_char();

        ///
        /// \fn         print_error
        /// \@brief     Print the error message
        ///
        void print_error(json_parse_error error_type) const;

    private:
        std::ifstream json_file;    ///< The input stream of json file
        char buffer[BUF_SIZE];      ///< Two buffers reading json file
        int line;                   ///< The line number
        int pos_in_line;            ///< The position in current line
        char *current_char;         ///< The current char(buffer + pos_in_buf)
    };
}

#endif // JSON_LITE
