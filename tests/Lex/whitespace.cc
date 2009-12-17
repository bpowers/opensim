// Copyright 2009 Bobby Powers
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject
// to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include <opensim/lex.h>
#include <opensim/token.h>
using opensim::Scanner;
using opensim::Token;

#include <gtest/gtest.h>

#include <cstring>


#define STR_END(x) x + sizeof(x)/sizeof(x[0])


class WhitespaceTest: public testing::Test {
protected:

  Token *firstToken(const char *str) {

    Scanner scanner("", str, str+strlen(str));

    return scanner.getToken();
  }

  void validateToken(Token *tok, const char *str,
		     uint32_t line, uint32_t pos) {

    size_t len = strlen(str);
    ASSERT_NE((void *)NULL, tok);

    // lines are indexed from 1...
    EXPECT_EQ(line, tok->start.line);
    EXPECT_EQ(pos, tok->start.pos);

    EXPECT_EQ(tok->start.line, tok->end.line);
    EXPECT_EQ(tok->start.pos + len, tok->end.pos);

    EXPECT_EQ(0, tok->iden.compare(str));
  }
};


TEST_F(WhitespaceTest, NoSpace) {

  Token *tok = firstToken("test");

  validateToken(tok, "test", 1, 1);

  delete tok;
}


TEST_F(WhitespaceTest, LeadingSpace) {

  Token *tok = firstToken("   test");

  validateToken(tok, "test", 1, 4);

  delete tok;
}


TEST_F(WhitespaceTest, TrailingSpace) {

  Token *tok = firstToken("test   ");

  validateToken(tok, "test", 1, 1);

  delete tok;
}


TEST_F(WhitespaceTest, LeadingTabs) {

  Token *tok = firstToken("\t\t\ttest");

  validateToken(tok, "test", 1, 4);

  delete tok;
}


TEST_F(WhitespaceTest, TrailingTabs) {

  Token *tok = firstToken("test\t\t\t");

  validateToken(tok, "test", 1, 1);

  delete tok;
}


TEST_F(WhitespaceTest, LeadingMixed) {

  Token *tok = firstToken("\t\t  \t   test");

  validateToken(tok, "test", 1, 9);

  delete tok;
}


TEST_F(WhitespaceTest, LeadingNewlines) {

  Token *tok = firstToken("\n\n\ntest");

  validateToken(tok, "test", 4, 1);

  delete tok;
}


TEST_F(WhitespaceTest, LeadingNewlinesAndSpaces) {

  Token *tok = firstToken("\n\n\n\t   test");

  validateToken(tok, "test", 4, 5);

  delete tok;
}
