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
/// \file       json_lite.cpp
/// The implementation of class json_value and json_parser
/// \author     Garnel
/// \date       2012/09/12
/// \version    2.1
/// \copyright  Apache License, Version 2.0
///

#include "json_lite.h"

///
/// \namespace  json_lite
///
namespace json_lite
{
    ///
    /// \fn         safe_free
    /// \brief      Delete memory allocated safely
    /// \param      pT  The pointer to memory
    /// \note       NOT deal with arrays
    ///
    template <class T>
    void inline safe_free(T* &pT)
    {
        if (pT)
        {
            delete pT;
            pT = NULL;
        }
    }
    
    ///
    /// \overload   safe_free
    /// \brief      Delete dynamic c-style string safely
    ///
    template <>
    void inline safe_free(char* &p)
    {
        if (p)
        {
            delete[] p;
            p = NULL;
        }
    }

    // error_value
    std::string error_value(json_parse_error error_type)
    {
        switch (error_type)
        {
        case SHOULD_BE_OBJECT_OR_ARRAY:
            return "A json payload should be an object or array.";
            break;
        case INVALID_TYPE:
            return "There is no such a type.";
            break;
        case INVALID_CHARACTER:
            return "Sorry, I can not recognize the character.";
            break;
        case INVALID_ESCAPE_CHARACTER:
            return "The character is not supposed to exist after '\\'.";
            break;
        case INVALID_BASE:
            return "The base of the number is not correct.";
            break;
        case INVALID_EXPONENT:
            return "The exponent of the number is not correct.";
            break;
        case TOO_MANY_DOTS_IN_NUMBER:
            return "Some part has too many dots.";
            break;
        case LEADING_ZERO:
            return "Some part has leading zeros.";
            break;
        case INVALID_SIGN_POSITION:
            return "Signs should only appear in the front of numbers.";
            break;
        case MISSING_DIGIT:
            return "Some part has no digit.";
            break;
        case TOO_MANY_SIGNS_IN_NUMBER:
            return "Some part has too many signs.";
            break;
        case TOO_MANY_EXPONENTS:
            return "The number has too many exponents.";
            break;
        case ERROR_IN_TRUE:
            return "It is supposed to be \"true\", not anything else.";
            break;
        case ERROR_IN_FALSE:
            return "It is supposed to be \"false\", not anything else.";
            break;
        case ERROR_IN_NULL:
            return "It is supposed to be \"null\", not anything else.";
            break;
        case MISSING_COLON:
            return "It is supposed to be a ':'.";
            break;
        case MISSING_QUOTATION:
            return "It is supposed to be a '\"'.";
            break;
        case EXTRA_COMMA:
            return "There is extra comma in the object or array.";
            break;
        case EMPTY_VALUE:
            return "There seems to be nothing in the value.";
            break;
        case UNCLOSED_OBJECT:
            return "The object is unclosed.";
            break;
        case UNCLOSED_ARRAY:
            return "The array is unclosed.";
            break;
        case EXTRA_CONTENT_AFTER_JSON:
            return "There is needless content after json.";
            break;
        default:
            return "There must be some error.";
            break;
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // json_value
    ///////////////////////////////////////////////////////////////////////////

    // json_value
    json_value::json_value(json_type _type)
        :type(_type),
         next(NULL),
         prev(NULL),
         parent(NULL),
         first_child(NULL),
         last_child(NULL)
    {
        switch (_type)
        {
        case JSON_TRUE:
            value = "true";
            break;
        case JSON_FALSE:
            value = "false";
            break;
        case JSON_NULL:
            value = "null";
            break;
        default:
            value = "";
            break;
        }
    }

    json_value::json_value(json_type _type, std::string _value)
        :type(_type),
        next(NULL),
        prev(NULL),
        parent(NULL),
        first_child(NULL),
        last_child(NULL)
    {
        this->set_value(_value);
    }

    // ~json_value
    json_value::~json_value()
    {
        // delete the child first
        safe_free(first_child);
        first_child = NULL;
        last_child = NULL;

        // delete next node
        safe_free(next);
        next = NULL;

        // clear the connection to the previous node, and its parent
        prev = NULL;
        parent = NULL;
    }

    // get_type
    json_type json_value::get_type() const
    {
        return type;
    }

    // set_value
    void json_value::set_value(std::string _value)
    {
        switch (this->get_type())
        {
        case JSON_STRING:
        case JSON_NUMBER:
            value = _value;
            break;

        // leave the _value passed in alone
        case JSON_TRUE:
            value = "true";
            break;
        case JSON_FALSE:
            value = "false";
            break;
        case JSON_NULL:
            value = "null";
            break;
        case JSON_OBJECT:
        case JSON_ARRAY:
            value = "";
            break;
        
        // default
        default:
            break;
        }
    }
    
    // get_value
    std::string json_value::get_value() const
    {
        if (type == JSON_STRING)
        {
            return ("\"" + value + "\"");
        }
        else
        {
            return value;
        }
    }

    // set_next
    void json_value::set_next( json_value *_next )
    {
        assert(_next);
        next = _next;
    }
    
    // set_prev
    void json_value::set_prev( json_value *_prev )
    {
        assert(_prev);
        prev = _prev;
    }

    // set_parent
    void json_value::set_parent( json_value *_parent )
    {
        assert(_parent);
        parent = _parent;
    }
    
    // set_first_child
    void json_value::set_first_child( json_value *_first_child )
    {
        assert(_first_child);
        first_child = _first_child;
    }
    
    // set_last_child
    void json_value::set_last_child( json_value *_last_child )
    {
        assert(_last_child);
        last_child = _last_child;
    }
    
    // get_next
    json_value* json_value::get_next() const
    {
        return next;
    }
    
    // get_prev
    json_value* json_value::get_prev() const
    {
        return prev;
    }
    
    // get_parent
    json_value* json_value::get_parent() const
    {
        return parent;
    }
    
    // get_first_child
    json_value* json_value::get_first_child() const
    {
        return first_child;
    }
    
    // get_last_child
    json_value* json_value::get_last_child() const
    {
        return last_child;
    }
    
    // add_child
    void json_value::add_child( json_value* _child )
    {
        assert(_child);
        if (last_child)
        {
            this->last_child->set_next(_child);
            _child->set_prev(this->get_last_child());
            this->set_last_child(_child);
            _child->set_parent(this);
        }
        else  //has no child yet
        {
            this->set_first_child(_child);
            this->set_last_child(_child);
            _child->set_parent(this);
        }
    }

    // add_pair
    void json_value::add_pair(std::string _key, json_value* _value)
    {
        assert(this->get_type() == JSON_OBJECT);
        assert(_value != NULL);
        
        //key
        json_value *_k = new json_value(JSON_STRING, _key);

        //value
        _k->add_child(_value);

        //add pair
        this->add_child(_k);
    }
    
    // output
    bool json_value::output(const std::string out_file, bool format, int indent_level) const
    {
        // the standard output, restore it at the end
        std::streambuf *std_buf = std::cout.rdbuf();
        std::ofstream json_file(out_file.c_str());
        if (!json_file.is_open())
        {
            std::cout << "File cannot be opened." << std::endl;
            return false;
        }

        // redirect the standard output the the file
        std::cout.rdbuf(json_file.rdbuf());
        bool result = this->output(format, indent_level);

        // restore the standard output
        std::cout.rdbuf(std_buf);
        return result;
    }

    bool json_value::output(const char* out_file, bool format, int indent_level) const
    {
        std::string temp(out_file);
        return this->output(temp, format, indent_level);
    }

    bool json_value::output(bool format, int indent_level) const
    {
        assert(this);
        bool result;  // if the output is successful
        switch (this->get_type())
        {
        case JSON_STRING:
        case JSON_NUMBER:
        case JSON_TRUE:
        case JSON_FALSE:
        case JSON_NULL:
            result = print_json_value(this, format, indent_level);
            break;
        case JSON_OBJECT:
            result = print_json_object(this, format, indent_level);
            break;
        case JSON_ARRAY:
            result = print_json_array(this, format, indent_level);
            break;
        default:
            break;
        }
        return result;
    }
    
    // print_json_object
    bool print_json_object(const json_value *obj, bool format, int indent_level)
    {
        // the json element must be a object
        assert(obj->get_type() == JSON_OBJECT);

        if (format)
            for (int i = 0; i < indent_level; i++)
                std::cout << "\t";
        std::cout << '{';
        indent_level++;

        json_value *cur = obj->get_first_child(),
                   *cur_child;
        while (cur)  //children
        {
            if (format)
                std::cout << '\n';

            // output the label
            print_json_value(cur, format, indent_level);

            // the ':'
            std::cout << ':';
            if (format)
                std::cout << ' ';

            //the value
            cur_child = cur->get_first_child();
            json_type _type = cur_child->get_type();
            if ((_type == JSON_OBJECT || _type == JSON_ARRAY)
                && cur_child->get_first_child() != NULL)   //empty object or array will be printed directly as "{}","[]"
            {
                if (format)
                    std::cout << '\n';
                cur_child->output(format, indent_level);
            }
            else
                cur_child->output();
            
            cur = cur->next;
            // if it has more children, print ','
            if (cur)
            {
                std::cout << ',';
            }
            else 
            {
                indent_level--;
                if (format)
                {
                    std::cout << std::endl;
                    for (int i = 0; i < indent_level; i++)
                        std::cout << '\t';
                }
            }
        }
        
        // the end
        std::cout << '}';

        //flush the output
        std::cout.flush();
        return true;
    }
    
    // print_json_array
    bool print_json_array(const json_value *arr, bool format, int indent_level)
    {
        // the json element must be a array
        assert(arr->get_type() == JSON_ARRAY);

        if (format)
            for (int i = 0; i < indent_level; i++)
                std::cout << "\t";
        std::cout << '[';
        indent_level++;

        json_value *cur = arr->get_first_child();
        while (cur)  //children
        {
            if (format)
                std::cout << '\n';

            //the value
            cur->output(format, indent_level);

            cur = cur->next;

            // if it has more children, print ','
            if (cur)
            {
                std::cout << ',';
            }
            else 
            {
                indent_level--;
                if (format)
                {
                    std::cout << std::endl;
                    for (int i = 0; i < indent_level; i++)
                        std::cout << '\t';
                }
            }
        }

        // the end
        std::cout << ']';

        //flush the output
        std::cout.flush();
        return true;
    }
    
    // print_json_value
    bool print_json_value(const json_value *elem, bool format, int indent_level)
    {
        // only for strings, numbers, null, true and false
        json_type _type = elem->get_type();
        assert(_type != JSON_OBJECT && _type != JSON_ARRAY);

        // format the output
        if (format)
            for (int i = 0; i < indent_level; i++)
                std::cout << '\t';
        
        std::cout << elem->get_value(); 
        return true;
    }

    // operator<<
    std::ostream& operator<<(std::ostream& output, const json_value &value)
    {
        // the standard output, restore it at the end
        std::streambuf *std_buf = std::cout.rdbuf();
        std::cout.rdbuf(output.rdbuf());
        value.output(false);

        // restore the standard output
        std::cout.rdbuf(std_buf);
        return output;
    }

    ///////////////////////////////////////////////////////////////////////////
    // json_parser
    ///////////////////////////////////////////////////////////////////////////

    // json_parser
    json_parser::json_parser( const std::string file_name )
        :line(1),
         pos_in_line(1)
    {
        json_file.open(file_name.c_str());
        if (!json_file.is_open())
        {
            throw "The file cannot be opened!";
        }
        json_file.seekg(0, std::ios::beg);
        
        memset(buffer, 0, BUF_SIZE);
        if (!json_file.eof())
            json_file.read(buffer, BUF_SIZE - 1);
        
        current_char = buffer;
    }

    // ~json_parser
    json_parser::~json_parser()
    {
        //close the file
        if (json_file.is_open())
            json_file.close();
    }
    
    // parse_value
    json_value* json_parser::parse_value( json_type _type )
    {
        json_value *_value = NULL;
        try
        {
            switch (_type)
            {
            case JSON_STRING:
                _value = new json_value(JSON_STRING, this->parse_string());
                break;
            case JSON_NUMBER:
                _value = new json_value(JSON_NUMBER, this->parse_number());
                break;
            case JSON_OBJECT:
                _value = this->parse_object();
                break;
            case JSON_ARRAY:
                _value = this->parse_array();
                break;
            case JSON_TRUE:
                _value = new json_value(JSON_TRUE);
                this->parse_true();
                break;
            case JSON_FALSE:
                _value = new json_value(JSON_FALSE);
                this->parse_false();
                break;
            case JSON_NULL:
                _value = new json_value(JSON_NULL);
                this->parse_null();
                break;
            default:
                throw INVALID_TYPE;
            }
            return _value;
        }
        catch (json_parse_error error_type)
        {
            this->print_error(error_type);
            safe_free(_value);
            return (json_value*)NULL;
        }
    }

    // run
    json_value* json_parser::run()
    {
        //escape blank characters
        this->escape_blank();

        json_type _type;
        json_value *_value = NULL;
        try
        {
            switch (this->get_char())
            {
            case '{':
                _type = JSON_OBJECT;
                break;
            case '[':
                _type = JSON_ARRAY;
                break;
            default:
                throw SHOULD_BE_OBJECT_OR_ARRAY;
            }
            _value =  this->parse_value(_type);
            if (_value != NULL   /*no error in parse*/
                && this->escape_blank() != '\0')  //after the json should be only blank characters
                throw EXTRA_CONTENT_AFTER_JSON;
            return _value;
        }
        catch (json_parse_error error_type)
        {
            this->print_error(error_type);
            return (json_value*)NULL;
        }
    }

    // escape_blank
    char json_parser::escape_blank()
    {
        char temp_char = this->get_current_char();
        while (temp_char == ' ' || temp_char == '\n' ||
            temp_char == '\r' || temp_char == '\t')
        {
            this->get_char();
            temp_char = this->get_current_char();
        }
        return temp_char;
    }

    // parse_string
    std::string json_parser::parse_string()
    {
        std::string _value;         // the value of the string to parse
        char temp[BUF_SIZE],        // string buffer
             *p = temp;             // cursor to the current position of buffer
        memset(temp, 0, BUF_SIZE);
        int cursor = 0;             // current character position of buffer

        char temp_char = this->get_char();
        while (temp_char)
        {
            //The end of the string
            if (temp_char == '\"')
                break;
            
            //copy the character to the string(through buffer)
            *p++ = temp_char;
            cursor++;
            if (cursor == BUF_SIZE - 1)  //buffer is full
            {
                _value += temp;
                memset(temp, 0, BUF_SIZE);
                p = temp;
                cursor = 0;
            }

            //The escape characters
            if (temp_char == '\\')
            {
                temp_char = this->get_char();
                switch (temp_char)
                {
                case '\"':
                case '\\':
                case '/':
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                case 'u':
                    *p++ = temp_char;
                    cursor++;
                    if (cursor == BUF_SIZE - 1)  //buffer is full
                    {
                        _value += temp;
                        memset(temp, 0, BUF_SIZE);
                        p = temp;
                        cursor = 0;
                    }
                    break;
                default:
                    throw INVALID_ESCAPE_CHARACTER;
                    break;
                }
            }

            //read the next character
            temp_char = this->get_char();
        }
        _value += temp;
        return _value;
    }

    // parse_number
    std::string json_parser::parse_number()
    {
        std::string _number;        // the value of the number to parse
        char temp[BUF_SIZE],        // string buffer
             *p = temp;             // cursor to the current position of buffer
        memset(temp, 0, BUF_SIZE);
        int cursor = 0;             // current character position of buffer

        bool dot_parsed = false,    // if the dot is parsed
             in_exponent = false,   // while parsing exponent, it turns true
             leading_zero = false,  // if it has leading zeros
             has_digit = false,     // if the part has digit
             has_sign = false;      // if the number has sign(+/-)

        char temp_char = this->get_current_char();
        while (temp_char)
        {
            //a number can only exit in some pair of a object or a element,
            //so ',', ']', '}' and blank characters will end a number
            if (temp_char == ',' || temp_char == ']' || temp_char == '}' ||
                temp_char == ' ' || temp_char == '\t' || temp_char == '\n' ||
                temp_char == '\r')
            {
                if (!has_digit)  //number has no digits
                    throw MISSING_DIGIT;

                if (in_exponent && !has_digit) //exponent has nothing or only a sign
                    throw INVALID_EXPONENT;

                break;
            }

            //copy the character to the string(through buffer)
            *p++ = temp_char;
            cursor++;
            if (cursor == BUF_SIZE - 1)  //buffer is full
            {
                _number += temp;
                memset(temp, 0, BUF_SIZE);
                p = temp;
                cursor = 0;
            }

            switch (temp_char)
            {
            //digits
            case '0':
                if (leading_zero)  //leading zero
                    throw LEADING_ZERO;
                
                if (!leading_zero && !has_digit)  //the first one is a zero
                {
                    leading_zero = true;
                    has_digit = true;
                }
                break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if (leading_zero)  //leading zero
                    throw LEADING_ZERO;
                has_digit = true;
                break;

            //has sign
            case '+':
            case '-':
                if (has_digit)  //sign should in front of a number
                    throw INVALID_SIGN_POSITION;
                else if (has_sign)  //more than one sign
                    throw TOO_MANY_SIGNS_IN_NUMBER;
                else    
                    has_sign = true;
                break;

            //The dot
            case  '.':
                leading_zero = false;
                if (dot_parsed)
                    throw TOO_MANY_DOTS_IN_NUMBER;
                else
                    dot_parsed = true;
                break;

            //exponent
            case 'e':
            case 'E':
                if (!has_digit)  //the base has no digit
                    throw MISSING_DIGIT;
                
                if (in_exponent)  //already has a exponent
                    throw TOO_MANY_EXPONENTS;
                
                in_exponent = true;
                dot_parsed = false;
                has_digit = false;
                has_sign = false;
                leading_zero = false;
                break;
            default:
                if (in_exponent)
                    throw INVALID_EXPONENT;
                else
                    throw INVALID_BASE;
                break;
            }

            //read the next character
            this->get_char();
            temp_char = this->get_current_char();
        }
        _number += temp;
        return _number;
    }

    // parse_true
    std::string json_parser::parse_true()
    {
        char temp[5],
             *p = temp;
        memset(temp, 0, 5);

        *p++ = this->get_char();    // 't'
        *p++ = this->get_char();    // 'r'
        *p++ = this->get_char();    // 'u'
        *p++ = this->get_char();    // 'e'
        
        if (strcmp(temp, "true") != 0)
            throw ERROR_IN_TRUE;

        return "true";
    }
    
    // parse_false
    std::string json_parser::parse_false()
    {
        char temp[6],
             *p = temp;
        memset(temp, 0, 6);

        *p++ = this->get_char();       // 'f'
        *p++ = this->get_char();    // 'a'
        *p++ = this->get_char();    // 'l'
        *p++ = this->get_char();    // 's'
        *p++ = this->get_char();    // 'e'
        
        if (strcmp(temp, "false"))
            throw ERROR_IN_FALSE;

        return "false";
    }

    // parse_null
    std::string json_parser::parse_null()
    {
        char temp[5],
             *p = temp;
        memset(temp, 0, 5);

        *p++ = this->get_char();       // 'n'
        *p++ = this->get_char();    // 'u'
        *p++ = this->get_char();    // 'l'
        *p++ = this->get_char();    // 'l'
        
        if (strcmp(temp, "null"))
            throw ERROR_IN_FALSE;

        return "null";
    }

    // parse_object
    json_value* json_parser::parse_object()
    {
        json_value *obj = new json_value(JSON_OBJECT),
                   *_key, *_value;
        
        try 
        {
            //escape blank characters
            char temp_char = this->escape_blank();

            //if the character is '}', the object is empty
            while (temp_char != '}')
            {
                if (temp_char != '\"')
                    throw MISSING_QUOTATION;
                //escape the left quotation
                this->get_char();

                //label
                _key = new json_value(JSON_STRING, this->parse_string());

                //escape blank characters
                temp_char = this->escape_blank();

                //the current char must be ':'
                if (temp_char != ':')
                {
                    safe_free(_key);
                    throw MISSING_COLON;
                }

                //escape ':'
                this->get_char();

                //escape blank characters
                temp_char = this->escape_blank();

                //value in the pair
                switch (temp_char)
                {
                //only strings begin with a quotation
                case '\"':
                    // escape the quotation
                    this->get_char();
                    _value = new json_value(JSON_STRING, this->parse_string());
                    break;

                //numbers
                case '+':
                case '-':
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    _value = new json_value(JSON_NUMBER, this->parse_number());
                    break;

                // true
                case 't':
                    _value = new json_value(JSON_TRUE, this->parse_true());
                    break;
                
                // false
                case 'f':
                    _value = new json_value(JSON_FALSE, this->parse_false());
                    break;
                
                // null
                case 'n':
                    _value = new json_value(JSON_NULL, this->parse_null());
                    break;

                // objects
                case '{':
                    // escape the {
                    this->get_char();
                    _value = this->parse_object();
                    break;

                // arrays
                case '[':
                    // escape the [
                    this->get_char();
                    _value = this->parse_array();
                    break;
               
                // unexpected end
                case '\0':
                    safe_free(_key);
                    throw UNCLOSED_OBJECT;
                    break;

                /**
                 * error
                 * @todo        unicode handle capability
                 */
                default:
                    safe_free(_key);
                    throw INVALID_CHARACTER;
                    break;
                }

                // construct the json tree
                _key->add_child(_value);
                obj->add_child(_key);

                //escape blank characters and the pair separator ','
                temp_char = this->escape_blank();
                if (temp_char == ',')
                {
                    this->get_char();
                    temp_char = this->escape_blank();
                    if (temp_char == '}')  //extra comma (like this: "XXX, }")
                        throw EXTRA_COMMA;
                }
                else if (temp_char != '}')  //@todo Detail the error
                    throw INVALID_CHARACTER;
            }
            
            // read a character for next parse process
            this->get_char();
            return obj;
        }
        catch (json_parse_error error_type)
        {
            safe_free(obj);
            throw error_type;
        }
    }
    
    // parse_array
    json_value* json_parser::parse_array()
    {
        json_value *arr = new json_value(JSON_ARRAY),
                   *elem;
        
        try
        {
            char temp_char = this->escape_blank();
            //if the character is ']', the array is empty
            while (temp_char != ']')
            {
                switch (temp_char)
                {
                // only strings begin with a quotation
                case '\"':
                    //escape the quotation
                    this->get_char();
                    elem = new json_value(JSON_STRING, this->parse_string());
                    break;

                // numbers
                case '+':
                case '-':
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    elem = new json_value(JSON_NUMBER, this->parse_number());
                    break;

                // true
                case 't':
                    elem = new json_value(JSON_TRUE, this->parse_true());
                    break;
                
                // false
                case 'f':
                    elem = new json_value(JSON_FALSE, this->parse_false());
                    break;
                
                // null
                case 'n':
                    elem = new json_value(JSON_NULL, this->parse_null());
                    break;

                // objects
                case '{':
                    //escape the {
                    this->get_char();
                    elem = this->parse_object();
                    break;

                // arrays
                case '[':
                    //escape the [
                    this->get_char();
                    elem = this->parse_array();
                    break;
                
                // spaces, line-end or ,
                case ' ':
                case '\n':
                case '\r':
                case '\t':
                    //escape this character
                    this->get_char();
                    continue;
                
                // empty value
                case ',':
                    throw EMPTY_VALUE;
                    break;

                // unexpected end
                case '\0':
                    throw UNCLOSED_ARRAY;
                    break;

                // error
                default:
                    throw INVALID_CHARACTER;
                    break;
                }

                // construct the json tree
                arr->add_child(elem);

                //escape blank characters and the element separator ','
                temp_char = this->escape_blank();
                if (temp_char == ',')
                {
                    this->get_char();  //escape ','
                    temp_char = this->escape_blank();
                    if (temp_char == ']')  //extra comma (like this: "XXX, ]")
                        throw EXTRA_COMMA;
                }
                else if (temp_char != ']')  //@todo Detail the error
                {
                    if (temp_char == '\0')
                        throw UNCLOSED_ARRAY;
                    throw INVALID_CHARACTER;
                }
            }
            
            // escape the ']'
            this->get_char();
            return arr;
        }
        catch (json_parse_error error_type)
        {
            safe_free(arr);
            throw error_type;
        }
    }

    // locate_element_by_label
    std::streampos json_parser::locate_element_by_label(const char* label, std::streampos offset)
    {
        if (offset != json_file.tellg())
            json_file.seekg(offset);

        while (!json_file.eof())
        {
            char temp = get_char();
            if (temp == '\\')  //escape characters
            {
                this->get_char();
                continue;
            }
            
            if (temp == '"') //a string
            {
                if (this->parse_string() == label)
                {
                    if (this->escape_blank() != ':')  //the string is not a label
                        continue;

                    this->get_char();  //escape ':'
                    this->escape_blank();
                    break;
                }
            }  
        }

        return json_file.tellg();
    }

    // get_char
    const char json_parser::get_char()
    {
        char temp_char = 0;
        temp_char = *current_char++;
        if (!temp_char)  //the end of the buffer
        {
            if (!json_file.eof())
            {
                memset(buffer, 0, BUF_SIZE);
                json_file.read(buffer, BUF_SIZE - 1);
                current_char = buffer;
                temp_char = *current_char++;
            }
        }
        
        pos_in_line++;
        if (temp_char == '\n')
        {
            line++;
            pos_in_line = 1;
        }

        return temp_char;
    }

    // get_current_char
    const char json_parser::get_current_char()
    {
        char temp_char = 0;
        temp_char = *current_char;
        if (!temp_char)  //the end of the buffer
        {
            if (!json_file.eof())
            {
                memset(buffer, 0, BUF_SIZE);
                json_file.read(buffer, BUF_SIZE - 1);
                current_char = buffer;
                temp_char = *current_char;
            }
        }
        return temp_char;
    }

    // print_error
    void json_parser::print_error(json_parse_error error_type) const
    {
        std::cout << "Error in json at line " << line 
            << ", position " << pos_in_line << " :" << std::endl;
        std::cout << error_value(error_type) << std::endl;
    }
}