/*
    Copyright (c) 2009 Andrew Caudwell (acaudwell@gmail.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. The name of the author may not be used to endorse or promote products
       derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef REGEX_H
#define REGEX_H

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include <string>
#include <vector>
#include <exception>

class RegexCompilationException : public std::exception {
protected:
    std::string regex;
public:
    RegexCompilationException(std::string& regex) : regex(regex) {}
    virtual ~RegexCompilationException() throw () {};

    virtual const char* what() const throw() { return regex.c_str(); }
};

class Regex {
protected:
    pcre2_code *re;
    bool valid;

    int replaceOffset(std::string& str, const std::string& replacement_str, int offset=0);
    int matchOffset(const std::string& str, std::vector<std::string>* results = 0, int offset=0);
public:
    Regex(std::string regex, bool test = false);
    Regex(const Regex& regex);
    ~Regex();

    bool match(const std::string& str, std::vector<std::string>* results = 0);
    bool matchAll(const std::string& str, std::vector<std::string>* results = 0);

    bool replace(std::string& str, const std::string& replacement_str);
    bool replaceAll(std::string& str, const std::string& replacement_str);

    bool isValid() const;

};

#endif
