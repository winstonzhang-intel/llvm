//===--- ClangCommentHTMLTagsEmitter.cpp - Generate HTML tag list for Clang -=//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This tablegen backend emits efficient matchers for HTML tags that are used
// in documentation comments.
//
//===----------------------------------------------------------------------===//

#include "TableGenBackends.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/StringMatcher.h"
#include "llvm/TableGen/TableGenBackend.h"
#include <vector>

using namespace llvm;

void clang::EmitClangCommentHTMLTags(const RecordKeeper &Records,
                                     raw_ostream &OS) {
  ArrayRef<const Record *> Tags = Records.getAllDerivedDefinitions("Tag");
  std::vector<StringMatcher::StringPair> Matches;
  for (const Record *Tag : Tags) {
    Matches.emplace_back(Tag->getValueAsString("Spelling").str(),
                         "return true;");
  }

  emitSourceFileHeader("HTML tag name matcher", OS, Records);

  OS << "bool isHTMLTagName(StringRef Name) {\n";
  StringMatcher("Name", Matches, OS).Emit();
  OS << "  return false;\n"
     << "}\n\n";
}

void clang::EmitClangCommentHTMLTagsProperties(const RecordKeeper &Records,
                                               raw_ostream &OS) {
  ArrayRef<const Record *> Tags = Records.getAllDerivedDefinitions("Tag");
  std::vector<StringMatcher::StringPair> MatchesEndTagOptional;
  std::vector<StringMatcher::StringPair> MatchesEndTagForbidden;
  for (const Record *Tag : Tags) {
    std::string Spelling = Tag->getValueAsString("Spelling").str();
    StringMatcher::StringPair Match(Spelling, "return true;");
    if (Tag->getValueAsBit("EndTagOptional"))
      MatchesEndTagOptional.push_back(Match);
    if (Tag->getValueAsBit("EndTagForbidden"))
      MatchesEndTagForbidden.push_back(Match);
  }

  emitSourceFileHeader("HTML tag properties", OS, Records);

  OS << "bool isHTMLEndTagOptional(StringRef Name) {\n";
  StringMatcher("Name", MatchesEndTagOptional, OS).Emit();
  OS << "  return false;\n"
     << "}\n\n";

  OS << "bool isHTMLEndTagForbidden(StringRef Name) {\n";
  StringMatcher("Name", MatchesEndTagForbidden, OS).Emit();
  OS << "  return false;\n"
     << "}\n\n";
}
