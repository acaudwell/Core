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

#include "regex.h"

Regex::Regex(std::string regex, bool test) {

    re = pcre_compile(
        regex.c_str(),
        0,
        &error,
        &erroffset,
        0
    );

    if(!re) {
        valid = false;

        if(!test) {
            throw RegexCompilationException(regex);
        }

    } else {
        valid = true;
    }

}

Regex::~Regex() {
    if(re != 0) pcre_free(re);
}

bool Regex::isValid() {
    return valid;
}

bool Regex::replace(std::string& str, const std::string& replacement_str) {

    int offset = replaceOffset(str, replacement_str, 0);
    
    return (offset != -1); 
}

bool Regex::replaceAll(std::string& str, const std::string& replacement_str) {
   

    int offset = -1;
    
    while((offset = replaceOffset(str, replacement_str, offset+1)) != -1 && offset < str.size());

    return (offset != -1);
}

int Regex::replaceOffset(std::string& str, const std::string& replacement_str, int offset) {
    
    int ovector[REGEX_MAX_MATCHES];

    int rc = pcre_exec(
        re,
        0,
        str.c_str(),
        str.size(),
        offset,
        0,
        ovector,
        REGEX_MAX_MATCHES
    );

    //failed match
    if(rc<1) {
        return -1;
    }
    
    // replace matched section of string
    std::string new_str = str;
    new_str.replace(ovector[0], ovector[1]-ovector[0], replacement_str);
    
    size_t end_offset = ovector[0] + replacement_str.size();
    
    for (int i = 1; i < rc; i++) {
        int match_start = ovector[2*i];
        int match_end   = ovector[2*i+1]; 

        std::string matched_str;
        
        if(match_start != -1) {
            matched_str = std::string(str, match_start, match_end-match_start);
        }
                
        // check if 'str' contains $i, if it does, replace with match string
        size_t string_size = new_str.size();
        
        for(size_t j=0; j<string_size-1; j++) {
            if(new_str[j] == '$' && atoi(&(new_str[j+1])) == i) {
                new_str.replace(j, 2, matched_str);
                size_t new_string_size = new_str.size();
                end_offset += (new_string_size-string_size);
                string_size = new_string_size;
            }
        }
    }

    str = new_str;
    
    return end_offset;        
}

bool Regex::match(const std::string& str, std::vector<std::string>* results) {

    if(results != 0) results->clear();
    int offset = matchOffset(str, results, 0);
    return offset != -1;
}

bool Regex::matchAll(const std::string& str, std::vector<std::string>* results) {
   
    int offset = 0;
    int match_count = 0;
    if(results != 0) results->clear();

    int str_size = str.size();
    
    while((offset = matchOffset(str, results, offset)) != -1) {
        match_count++;
        if(offset >= str_size) break;
    }

    return match_count>0;
}

int Regex::matchOffset(const std::string& str, std::vector<std::string>* results, int offset) {
    
    int ovector[REGEX_MAX_MATCHES];

    if(offset >= str.size()) return -1;
    
    // To allow ^ to match the start of the remaining string
    // we offset the string before passing it to pcre_exec
    
    int rc = pcre_exec(
        re,
        0,
        str.c_str() + offset,
        str.size()-offset,
        0,
        0,
        ovector,
        REGEX_MAX_MATCHES
    );

    //failed match
    if(rc<1) {
        return -1;
    }
   
    if(results!=0) {
        for (int i = 1; i < rc; i++) {
            int match_start = ovector[2*i];
            int match_end   = ovector[2*i+1]; 

            // insert a empty string for non-matching optional regex
            if(match_start == -1) {
                results->push_back(std::string(""));
            } else {
                std::string match(str, match_start+offset, match_end-match_start);
                results->push_back(match);
            }
        }
    }

    return ovector[1]+offset;
}
