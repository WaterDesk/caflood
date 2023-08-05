/*

Copyright (c) 2013 Centre for Water Systems,
                   University of Exeter

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#ifndef _CA_UTILITIES_HPP_
#define _CA_UTILITIES_HPP_


//! \file Utilities.hpp
//! Contain various utilities macros, classes and methods 
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2011-07


#include<string>
#include<sstream>
#include<cstdlib>
#include<functional>
#include<cmath>
#include<vector>
#include<iostream>


// --- MACROS -- //  


//! \def CA_QUOTE
//! Macro that "quote" the given name.
#ifdef  CA_QUOTE
#undef  CA_QUOTE
#endif
#define  CA_QUOTE(name) #name


//! \def CA_QUOTE_MACRO
//! Macro that "quote" a macro
#ifdef  CA_QUOTE_MACRO
#undef  CA_QUOTE_MACRO
#endif
#define  CA_QUOTE_MACRO(name) CA_QUOTE(name)


//! \def CA_DUMP_BUFF
//! Dunp a Cell/Edge/Vertex buffer into a file.
#define CA_DUMP_BUFF(name, iteration)                       \
    {                                                       \
        std::stringstream filename;                         \
        filename<<CA_QUOTE(name)<<"_"<<iteration<<".txt";   \
        std::ofstream _file(filename.str().c_str());        \
        if(_file) {                                         \
            name.dump(_file, "\t");                         \
            }                                               \
        _file.close();                                      \
    }


namespace CA {

    // --- CLASSES -- //  

    //! Base class that if is inherited make the derived class not
    //! copyable. The class is similar to the one used in item 6 of book
    //! Effective c++ Third Edition.

    class Uncopyable
    {
    protected:
        Uncopyable() {}
        ~Uncopyable() {}
    private:
        Uncopyable(const Uncopyable&);
        Uncopyable& operator=(const Uncopyable&);
    };


    // --- FUNCTORS -- //  

    //! This functor is used to deallocate a container of pointers
    template<typename T>
    struct Dealloc
#if __cplusplus < 201703L
        : public std::unary_function<T*, void>
#endif
    {
        void operator() (T&) {}
        void operator() (const T&) {}
    };

    template<typename T>
    struct Dealloc<T *>
#if __cplusplus < 201703L
        : public std::unary_function<T*, void>
#endif
    {
        void operator() (T*& p) { if (p != 0) delete p; p = 0; }
        void operator() (const T* const& p) { if (p != 0) delete p; }
    };


    namespace OP {

        //! This functor is used to assign the value of the first argument
        //! equal to the value of the second argument.
        template<typename T>
        struct Equal
#if __cplusplus < 201703L
            : public std::binary_function<T, T, void>
#endif
        {
            void operator() (T& a, T& b) { a = b; }
        };


        //! This functor is used to assign the value of the first argument
        //! equal to the original value plus the value of the second
        //! argument.
        template<typename T>
        struct AddEqual
#if __cplusplus < 201703L
            : public std::binary_function<T, T, void>
#endif
        {
            void operator() (T& a, T& b) { a += b; }
        };


        //! This functor is used to assign the value of the first argument
        //! equal to the original value minus the value of the second
        //! argument.
        template<typename T>
        struct SubEqual
#if __cplusplus < 201703L
            : public std::binary_function<T, T, void>
#endif
        {
            void operator() (T& a, T& b) { a -= b; }
        };


        //! This functor is used to assign the value of the first argument
        //! equal to the original value multiplies by the value of the second
        //! argument.
        template<typename T>
        struct MulEqual
#if __cplusplus < 201703L
            : public std::binary_function<T, T, void>
#endif
        {
            void operator() (T& a, T& b) { a *= b; }
        };


        //! This functor is used to assign the value of the first argument
        //! equal to the original value divided by the value of the second
        //! argument.
        template<typename T>
        struct DivEqual
#if __cplusplus < 201703L
            : public std::binary_function<T, T, void>
#endif
        {
            void operator() (T& a, T& b) { a /= b; }
        };


        //! This functor is used to assign the value of the first argument
        //! equal to the minum value between the two given values.
        template<typename T>
        struct MinEqual
#if __cplusplus < 201703L
            : public std::binary_function<T, T, void>
#endif
        {
            void operator() (T& a, T& b) { a = std::min(a, b); }
        };

        //! This functor is used to assign the value of the first argument
        //! equal to the minum value between the two given absolute values.
        template<typename T>
        struct MinAbsEqual
#if __cplusplus < 201703L
            : public std::binary_function<T, T, void>
#endif
        {
            void operator() (T& a, T& b) { a = std::min(std::abs(a), std::abs(b)); }
        };


        //! This functor is used to assign the value of the first argument
        //! equal to the maximum value between the two given values.
        template<typename T>
        struct MaxEqual
#if __cplusplus < 201703L
            : public std::binary_function<T, T, void>
#endif
        {
            void operator() (T& a, T& b) { a = std::max(a, b); }
        };

        //! This functor is used to assign the value of the first argument
        //! equal to the maximum value between the two given absolute values.
        template<typename T>
        struct MaxAbsEqual
#if __cplusplus < 201703L
            : public std::binary_function<T, T, void>
#endif
        {
            void operator() (T& a, T& b) { a = std::max(std::abs(a), std::abs(b)); }
        };

    }

    // --- METHODS -- //  


    //! Compare two strings without considering the case.  \warning if
    //! the third parameter is true, this method will check only if the
    //! str1 is a substring of str2.
    //! \param str1 The string to compare.
    //! \param str2 The reference string.
    //! \param substring If str1 can be a substring of str2.
    //! \return The comparison.
    inline bool compareCaseInsensitive(const std::string& str1, const std::string& str2, bool substring = false)
    {
        if (str1.empty()) return false;

        if (substring)
        {
            // If str1 can be a substring and is bigger than str2, then the
            // former is not substring of str2.
            if (str1.size() > str2.size())
                return false;
        }
        else
        {
            // If str1 cannot be a substring, then its size must be the same of str2.
            if (str1.size() != str2.size())
                return false;
        }

        // Compare character by character the two string for the size of str1.
        std::string::const_iterator i_str1 = str1.begin();
        std::string::const_iterator i_str2 = str2.begin();

        for (; i_str1 != str1.end(); ++i_str1)
        {
            if (::toupper((*i_str1)) != ::toupper((*i_str2)))
                return false;

            ++i_str2;
        }

        return true;
    }


    //! Compare two strings without considering the case.  \warning if
    //! the third parameter is true, this method will check only if the
    //! str1 is a substring of str2.
    //! \attention the str2 must be NULL terminated.
    //! \param str1 The string to compare.
    //! \param str2 The reference string in a null terminated char*.
    //! \param substring If str1 can be a substring of str2.
    //! \return The comparison.    
    inline bool compareCaseInsensitive(const std::string& str1, const char* str2, bool substring = false)
    {
        if (str1.empty()) return false;

        // Compare character by character the two string for the size of str1.
        size_t index;

        for (index = 0; index < str1.size(); ++index)
        {
            if (str2[index] == 0 || ::toupper(str1[index]) != ::toupper(str2[index]))
                return false;
        }

        // If str1 cannot be a substring, then if there are still
        // character left in str2 (different size) return false.
        if (!substring && str2[index] != 0)
            return false;

        return true;
    }


    //! Return true if the first string is less than a second string
    //! without considering the case.
    inline bool lessThanCaseInsensitive(const std::string& str1, const std::string& str2)
    {
        // Compare character by character the two string for the size of str1.
        std::string::const_iterator i_str1 = str1.begin();
        std::string::const_iterator i_str2 = str2.begin();

        for (; i_str1 != str1.end() && i_str2 != str2.end(); ++i_str1, ++i_str2)
        {
            if (::toupper((*i_str1)) < ::toupper((*i_str2)))
                return true;
            if (::toupper((*i_str1)) > ::toupper((*i_str2)))
                return false;
        }

        if (str1.size() < str2.size())
            return true;

        return false;
    }


    //! Convert a string to a value of type T. \attention. It converts
    //! using decimal base..
    //! \param t is the converted value. 
    //! \param s is the strin to convert.   
    //! \return true If the conversion succeeded.
    template<typename T>
    inline bool fromString(T& t, const std::string& s)
    {
        std::istringstream iss(s);
        return !(iss >> t).fail();
    }


    //! From string to a string specialisation.
    template <>
    inline bool fromString<std::string>(std::string& t, const std::string& s)
    {
        t = s;
        return true;
    }


    //! From string to a string specialisation.
    template <>
    inline bool fromString<bool>(bool& t, const std::string& s)
    {
        // Try to retrieve a value
        std::istringstream iss(s);

        // If can be convert to boolean return.
        if (!(iss >> t).fail())
            return true;

        // Check if it is the string "true"
        if (compareCaseInsensitive(s, "true"))
        {
            t = true;
            return true;
        }

        // Check if it is the string "false"
        if (compareCaseInsensitive(s, "false"))
        {
            t = false;
            return true;
        }

        // There is a problem.
        return false;
    }


    //! Convert a value of type T to a string. \attention. It converts
    //! using decimal base.
    //! \param s is the converted string. 
    //! \param t is the value to convert. 
    //! \return true If the conversion succeeded.
    template<typename T>
    inline bool toString(std::string& s, const T& t)
    {
        std::ostringstream oss;
        if ((oss << t).fail()) return false;
        s = oss.str();
        return true;
    }

    //! Trim the leading and trailing characters from the string.
    inline std::string trimToken(const std::string& str, const std::string& whitespace = " \t\r")
    {
        size_t strBegin = str.find_first_not_of(whitespace);
        if (strBegin == std::string::npos)
            return ""; // no content

        size_t strEnd = str.find_last_not_of(whitespace);
        size_t strRange = strEnd - strBegin + 1;

        return str.substr(strBegin, strRange);
    }


    //! Take the next line from the given file stream and split it in tokens
    //! using the given separator.
    inline std::vector<std::string> getLineTokens(std::istream& str, char sep)
    {
        std::vector<std::string>   result;
        std::string                line;
        std::getline(str, line);

        std::stringstream          lineStream(line);
        std::string                cell;

        while (std::getline(lineStream, cell, sep))
        {
            cell = trimToken(cell);

            if (cell.size() > 0)
                result.push_back(cell);
        }
        return result;
    }


    //! Pair like class that contains four elements.
    template <class T1, class T2, class T3, class T4>
    struct  quadro
    {
        typedef T1 first_type;
        typedef T2 second_type;
        typedef T3 third_type;
        typedef T4 fourth_type;

        T1 first;
        T2 second;
        T3 third;
        T4 fourth;

        // Constructors
        quadro() : first(T1()), second(T2()), third(T3()), fourth(T4()) {}
        quadro(const T1& a, const T2& b, const T3& c, const T4& d) :
            first(a), second(b), third(c), fourth(d) {}
    };
}

#endif  // _CA_UTILITIES_HPP_
