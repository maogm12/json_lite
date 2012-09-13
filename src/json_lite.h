/**
 * @author  Garnel	
 * @date    2012/09/12
 * @version 2.0
 */

#ifndef JSON_LITE
#define JSON_LITE

#include <iostream>
#include <string>
#include <cassert>
#include <fstream>

const int BUF_SIZE = 1024;

/**
 * json_lite
 * Include json_value and json_parser
 */
namespace json_lite
{
    /**
     * The types of json element
     */
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
    
    /**
     * The errors in json parse
     */
    enum json_parse_error
    {
        SHOULD_BE_OBJECT_OR_ARRAY,

        INVALID_TYPE,
        INVALID_CHARACTER,
        INVALID_ESCAPE_CHARACTER,

        INVALID_BASE,
        INVALID_EXPONENT,
        TOO_MANY_DOTS_IN_NUMBER,

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

    /**
     * @brief       Print the parse error.
     * @param       error_type - The type of the error
     */
    void print_error(json_parse_error error_type);


    /* ********************************************************************************************* *
     *                             json_value
     * ********************************************************************************************* */
    /**
     * The json element
     */
    class json_value
    {
    public:
        /**
         * @brief       The constructor of json_value.
         * @param        _type - the type of the element
         */
        json_value(json_type _type);

        /**
         * @brief       The constructor of json_value.
         * @param       _type - the type of the element
         * @param       _value - the value of the element
         */
        json_value(json_type _type, std::string _value);

        /**
         * @brief       The destructor of json_value.
         * It will free its children
         */
        ~json_value();

        /**
         * getType
         * Return the type of the element
         */
        json_type get_type() const;

        /**
         * @brief       Set the value of the element.
         * Set the value for the element by the type.
         * 
         * @warning
         * It cannot handle the error in the value passed in, such as a error number
         * @todo
         * handle errors in the value passed in
         */
        void set_value(std::string _value);

        /**
         * @brief       Return the value of the element
         *  
         * @warning
         * If the element is a string, the value will contain " of two sides
         * objects and arrays have no value, the value will be a empty string
         */
        std::string get_value() const;
        
        /**
         * Set the connections to the related elements
         */
        void set_next(json_value *_next);
        void set_prev(json_value *_prev);
        void set_parent(json_value *_parent);
        void set_first_child(json_value *_first_child);
        void set_last_child(json_value *_last_child);

        /**
         * Get the related elements
         */
        json_value* get_next() const;
        json_value* get_prev() const;
        json_value* get_parent() const;
        json_value* get_first_child() const;
        json_value* get_last_child() const;

        /**
         * add_child
         * Add a new child to the current element
         * 
         * @param       _child - The pointer to the child
         *  
         * @note
         * The pointer passed in must not be NULL
         */
        void add_child(json_value* _child);

        /**
         * output
         * Print the json tree
         * 
         * @param       out_file - the output file name
         * @param       format - format the document in the output file
         * @param       indent_level - the levels of indentation, default value is 0
         *
         * @return      true for success, false for failure
         */
        bool output(const std::string out_file, bool format = true, int indent_level = 0);
        
        /**
         * output
         * Print the json tree
         * 
         * @param       out_file - the output file name
         * @param       format - format the document in the output file
         * @param       indent_level - the levels of indentation, default value is 0
         *
         * @return      true for success, false for failure
         */
        bool output(const char* out_file, bool format = true, int indent_level = 0);

        /**
         * Print the json tree to the standard output
         * 
         * @param       format - format the document in the output file
         * @param       indent_level - the levels of indentation, default value is 0
         *
         * @return      true for success, false for failure
         */
        bool output(bool format = true, int indent_level = 0);

        /**
         * @brief       Print the json object
         * 
         * @param       obj - the json object
         * @param       format - format the document in the output file
         * @param       indent_level - the levels of indentation, default value is 0
         *
         * @return      true for success, false for failure
         */
        friend bool print_json_object(json_value *obj, bool format, int indent_level = 0);
        
        /**
         * @brief       Print the json array
         * 
         * @param       arr - the json array
         * @param       format - format the document in the output file
         * @param       indent_level - the levels of indentation, default value is 0
         *
         * @return      true for success, false for failure
         */
        friend bool print_json_array(json_value *arr, bool format, int indent_level = 0);

        /**
         * @brief       Print the json value
         * 
         * @param       elem - the json element
         * @param       format - format the document in the output file
         * @param       indent_level - the levels of indentation, default value is 0
         *
         * @return      true for success, false for failure
         */
        friend bool print_json_value(json_value *elem, bool format, int indent_level = 0);

    private:
        json_value(){}  //default constructor is not allowed to use

    private:
        json_type type;             /**< the type of the element. */
        std::string value;          /**< the value of the element. */
        json_value *next;           /**< the next element. */
        json_value *prev;           /**< the previous element. */
        json_value *parent;         /**< the parent element. */
        json_value *first_child;    /**< the first child. */
        json_value *last_child;     /**< the last_child. */
    };


    /* ********************************************************************************************* *
     *                             json_parser
     * ********************************************************************************************* */
    /**
     * the json parser
     */
    class json_parser
    {
    public:
        /**
         * @brief       The constructor of json_parser.
         * @param       file_name - The json file name
         * @exception
         * char* err - If the file cannot be opened, throw a message
         */
        json_parser(const std::string file_name);

        ~json_parser();

        /**
         * @brief       Run the parse process.
         * This function begins the parse process.
         * @return      The json element parsed.
         */
        json_value* run();

        /**
         * @brief       Escape blank characters.
         * Escape blank characters and return the first none-blank character.
         * @warning 
         * When the function ends, the cursor points to the FIRST none-blank character in the buffer. 
         * blank characters are:
         * ' ', '\n', '\r', '\t'
         * @return      The first none-blank character in the parsing stream.
         */
        char escape_blank();

        /**
         * @brief       Parse a complete element from buffer.
         * @param       _type - The type of element to be parsed.
         * @return      The json element parsed.
         */
        json_value* parse_value(json_type _type);

        /**
         * @brief       Parse a string.
         * @exception   
         * json_parse_error INVALID_ESCAPE_CHARACTER - A invalid character after '\'.
         * @return      The string value without quotations on two sides.
         */
        std::string parse_string();

        /**
         * @brief       Parse a number.
         * @exception   
         * json_parse_error TOO_MANY_DOTS_IN_NUMBER - More than one '.' exist in a number(integer part or fractional part).
         * json_parse_error INVALID_EXPONENT - Error in the exponent of a number.
         * json_parse_error INVALID_BASE - Error in the base of a number.
         * @todo 
         * 1. Numbers cannot have leading zeroes
         * 2. handle something like 0e+/0e+-1
         * @return      The value of the number in string.
         */
        std::string parse_number();

        /**
         * @brief       Parse a true.
         * @exception   
         * json_parse_error ERROR_IN_TRUE - Error in parsing "true".
         * @return      String "true".
         */
        std::string parse_true();

        /**
         * @brief       Parse a false.
         * @exception   
         * json_parse_error ERROR_IN_FALSE - Error in parsing "false".
         * @return      String "false".
         */
        std::string parse_false();
        
        /**
         * @brief       Parse a null.
         * @exception   
         * json_parse_error ERROR_IN_NULL - Error in parsing "null".
         * @return      String "null".
         */std::string parse_null();

        /**
         * @brief       Parse a object.
         * @exception   
         * json_parse_error MISSING_QUOTATION - The string of a pair lose it left double quotation.
         * json_parse_error MISSING_COLON - A pair lose ':'.
         * json_parse_error INVALID_CHARACTER - Some unknown character in the element of a pair.
         * Any exception in parsing string, number, true, false, null
         * @return      A pointer to the json object.
         */
        json_value* parse_object();

        /**
         * @brief       Parse a array.
         * @exception   
         * json_parse_error INVALID_CHARACTER - Some unknown character in the element.
         * Any exception in parsing string, number, true, false, null
         * @return      A pointer to the json object.
         */
        json_value* parse_array();

        /**
         * @brief       Get the current character in the buffer.
         * Get the current character in the buffer and move the cursor to the next character.
         *  
         * @warning
         * It will move the cursor to the next character.
         * @return      The current character
         */
        const char get_char();

        /**
         * @brief       Just get the current character in the buffer.
         * Get the current character in the buffer and do nothing else.
         *  
         * @warning
         * It will NOT move the cursor to the next character.
         * @return      The current character
         */
        const char get_current_char();

        /**
         * @brief       Print the error message. 
         */
        void error_message(json_parse_error error_type) const;

    private:
        std::ifstream json_file;
        char buffer[BUF_SIZE];      /**< Two buffers reading json file. */
        int line;                   /**< The line number. */
        int pos_in_line;            /**< The position in current line. */
        char *current_char;         /**< The current char(buffer + pos_in_buf). */
    };
}

#endif // JSON_LITE
