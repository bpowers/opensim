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

#include "opensim/lex.h"
using opensim::Scanner;
#include "opensim/token.h"
using opensim::Token;

#include <gtest/gtest.h>

#include <string>
using std::string;

#define STR_END(x) x + sizeof(x)/sizeof(x[0])


TEST(ScannerTest, NoSpace) {

  const char testStr[] = "test";
  Scanner scanner("", testStr, STR_END(testStr));

  Token *tok = scanner.getToken();

  // lines are indexed from 1...
  EXPECT_EQ(1, tok->start.line);
  EXPECT_EQ(1, tok->start.pos);

  EXPECT_EQ(tok->start.line, tok->end.line);
  EXPECT_EQ(tok->start.pos + 4, tok->end.pos);

  EXPECT_EQ(0, tok->iden.compare("test"));

  delete tok;
}

TEST(ScannerTest, LeadingSpace) {

  const char testStr[] = "   test";
  Scanner scanner("", testStr, STR_END(testStr));

  Token *tok = scanner.getToken();

  // lines are indexed from 1...
  EXPECT_EQ(1, tok->start.line);
  EXPECT_EQ(4, tok->start.pos);

  EXPECT_EQ(tok->start.line, tok->end.line);
  EXPECT_EQ(tok->start.pos + 4, tok->end.pos);

  EXPECT_EQ(0, tok->iden.compare("test"));

  delete tok;
}

TEST(ScannerTest, TrailingSpace) {

  const char testStr[] = "test   ";
  Scanner scanner("", testStr, STR_END(testStr));

  Token *tok = scanner.getToken();

  // lines are indexed from 1...
  EXPECT_EQ(1, tok->start.line);
  EXPECT_EQ(1, tok->start.pos);

  EXPECT_EQ(tok->start.line, tok->end.line);
  EXPECT_EQ(tok->start.pos + 4, tok->end.pos);

  EXPECT_EQ(0, tok->iden.compare("test"));

  delete tok;
}

TEST(ScannerTest, LeadingTabs) {

  const char testStr[] =  "\t\t\ttest";
  Scanner scanner("", testStr, STR_END(testStr));

  Token *tok = scanner.getToken();

  // lines are indexed from 1...
  EXPECT_EQ(1, tok->start.line);
  EXPECT_EQ(4, tok->start.pos);

  EXPECT_EQ(tok->start.line, tok->end.line);
  EXPECT_EQ(tok->start.pos + 4, tok->end.pos);

  EXPECT_EQ(0, tok->iden.compare("test"));

  delete tok;
}

TEST(ScannerTest, TrailingTabs) {

  const char testStr[] =  "test\t\t\t";
  Scanner scanner("", testStr, STR_END(testStr));

  Token *tok = scanner.getToken();

  // lines are indexed from 1...
  EXPECT_EQ(1, tok->start.line);
  EXPECT_EQ(1, tok->start.pos);

  EXPECT_EQ(tok->start.line, tok->end.line);
  EXPECT_EQ(tok->start.pos+4, tok->end.pos);

  EXPECT_EQ(0, tok->iden.compare("test"));

  delete tok;
}

TEST(ScannerTest, LeadingMixed) {

  const char testStr[] =  "\t\t  \t   test";
  Scanner scanner("", testStr, STR_END(testStr));

  Token *tok = scanner.getToken();

  // lines are indexed from 1...
  EXPECT_EQ(1, tok->start.line);
  EXPECT_EQ(9, tok->start.pos);

  EXPECT_EQ(tok->start.line, tok->end.line);
  EXPECT_EQ(tok->start.pos + 4, tok->end.pos);

  EXPECT_EQ(0, tok->iden.compare("test"));

  delete tok;
}

TEST(ScannerTest, LeadingNewlines) {

  const char testStr[] =  "\n\n\ntest";
  Scanner scanner("", testStr, STR_END(testStr));

  Token *tok = scanner.getToken();

  // lines are indexed from 1...
  EXPECT_EQ(4, tok->start.line);
  EXPECT_EQ(1, tok->start.pos);

  EXPECT_EQ(tok->start.line, tok->end.line);
  EXPECT_EQ(tok->start.pos + 4, tok->end.pos);

  EXPECT_EQ(0, tok->iden.compare("test"));

  delete tok;
}
