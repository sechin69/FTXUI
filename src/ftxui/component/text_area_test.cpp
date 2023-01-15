#include <gtest/gtest.h>
#include <memory>  // for __shared_ptr_access, shared_ptr, allocator
#include <string>  // for string

#include "ftxui/component/component.hpp"       // for Input
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowLeft, Event::ArrowRight, Event::Backspace, Event::Delete, Event::End, Event::Home
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Button, Mouse::Left, Mouse::Motion, Mouse::Pressed
#include "ftxui/dom/elements.hpp"   // for Fit
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/screen.hpp"  // for Fixed, Screen, Pixel
#include "ftxui/util/ref.hpp"       // for Ref

namespace ftxui {

TEST(TextAreaTest, Init) {
  std::string content;
  auto option = TextAreaOption();
  Component textarea = TextArea(&content, &option);

  EXPECT_EQ(option.cursor_column(), 0);
  EXPECT_EQ(option.cursor_line(), 0);
}

TEST(TextAreaTest, Type) {
  std::string content;
  std::string placeholder;
  auto option = TextAreaOption();
  Component textarea = TextArea(&content, &option);

  textarea->OnEvent(Event::Character("a"));
  EXPECT_EQ(content, "a");
  EXPECT_EQ(option.cursor_line(), 0u);
  EXPECT_EQ(option.cursor_column(), 1u);

  textarea->OnEvent(Event::Character('b'));
  EXPECT_EQ(content, "ab");
  EXPECT_EQ(option.cursor_line(), 0u);
  EXPECT_EQ(option.cursor_column(), 2u);
  
  textarea->OnEvent(Event::Return);
  EXPECT_EQ(content, "ab\n");
  EXPECT_EQ(option.cursor_line(), 1u);
  EXPECT_EQ(option.cursor_column(), 0u);

  textarea->OnEvent(Event::Character('c'));
  EXPECT_EQ(content, "ab\nc");
  EXPECT_EQ(option.cursor_line(), 1u);
  EXPECT_EQ(option.cursor_column(), 1u);

  auto document = textarea->Render();
  //auto screen = Screen::Create(Dimension::Fit(document));
  auto screen = Screen::Create(Dimension::Fixed(10), Dimension::Fixed(2));
  Render(screen, document);
  EXPECT_EQ(screen.PixelAt(0, 0).character, "a");
  EXPECT_EQ(screen.PixelAt(1, 0).character, "b");
  EXPECT_EQ(screen.PixelAt(0, 1).character, "c");
  EXPECT_EQ(screen.PixelAt(1, 1).character, " ");
}

TEST(TextAreaTest, Arrow1) {
  std::string content;
  auto option = TextAreaOption();
  auto textarea = TextArea(&content, &option);

  textarea->OnEvent(Event::Character('a'));
  textarea->OnEvent(Event::Character('b'));
  textarea->OnEvent(Event::Character('c'));

  EXPECT_EQ(option.cursor_column(), 3u);

  textarea->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(option.cursor_column(), 2u);

  textarea->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(option.cursor_column(), 1u);

  textarea->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(option.cursor_column(), 0u);

  textarea->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(option.cursor_column(), 0u);

  textarea->OnEvent(Event::ArrowRight);
  EXPECT_EQ(option.cursor_column(), 1u);

  textarea->OnEvent(Event::ArrowRight);
  EXPECT_EQ(option.cursor_column(), 2u);

  textarea->OnEvent(Event::ArrowRight);
  EXPECT_EQ(option.cursor_column(), 3u);

  textarea->OnEvent(Event::ArrowRight);
  EXPECT_EQ(option.cursor_column(), 3u);
}

TEST(TextAreaTest, Insert) {
  std::string content;
  Component textarea = TextArea(&content);

  textarea->OnEvent(Event::Character('a'));
  textarea->OnEvent(Event::Character('b'));
  textarea->OnEvent(Event::Character('c'));
  EXPECT_EQ(content, "abc");

  textarea->OnEvent(Event::ArrowLeft);
  textarea->OnEvent(Event::ArrowLeft);
  textarea->OnEvent(Event::Character('-'));
  EXPECT_EQ(content, "a-bc");

  textarea->OnEvent(Event::ArrowLeft);
  textarea->OnEvent(Event::Character('-'));
  EXPECT_EQ(content, "a--bc");

  textarea->OnEvent(Event::ArrowLeft);
  textarea->OnEvent(Event::ArrowLeft);
  textarea->OnEvent(Event::ArrowLeft);
  textarea->OnEvent(Event::Character('-'));
  EXPECT_EQ(content, "-a--bc");
}

TEST(TextAreaTest, Home) {
  std::string content;
  auto option = TextAreaOption();
  auto textarea = TextArea(&content, &option);

  textarea->OnEvent(Event::Character('a'));
  textarea->OnEvent(Event::Character('b'));
  textarea->OnEvent(Event::Character('c'));
  textarea->OnEvent(Event::Return);
  textarea->OnEvent(Event::Character('a'));
  textarea->OnEvent(Event::Character('b'));
  textarea->OnEvent(Event::Character('c'));
  EXPECT_EQ(content, "abc\nabc");
  EXPECT_EQ(option.cursor_line(), 1u);
  EXPECT_EQ(option.cursor_column(), 3u);

  textarea->OnEvent(Event::Home);
  EXPECT_EQ(option.cursor_line(), 0u);
  EXPECT_EQ(option.cursor_column(), 0u);

  textarea->OnEvent(Event::Character('-'));
  EXPECT_EQ(content, "-abc\nabc");
}

TEST(TextAreaTest, End) {
  std::string content;
  std::string placeholder;
  auto option = TextAreaOption();
  auto textarea = TextArea(&content, &option);

  textarea->OnEvent(Event::Character('a'));
  textarea->OnEvent(Event::Character('b'));
  textarea->OnEvent(Event::Character('c'));
  textarea->OnEvent(Event::Return);
  textarea->OnEvent(Event::Character('a'));
  textarea->OnEvent(Event::Character('b'));
  textarea->OnEvent(Event::Character('c'));
  textarea->OnEvent(Event::ArrowUp);
  textarea->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(content, "abc\nabc");
  EXPECT_EQ(option.cursor_line(), 0u);
  EXPECT_EQ(option.cursor_column(), 2u);

  textarea->OnEvent(Event::End);
  EXPECT_EQ(option.cursor_line(), 1u);
  EXPECT_EQ(option.cursor_column(), 3u);
}

TEST(TextAreaTest, Delete) {
  std::string content;
  std::string placeholder;
  auto option = TextAreaOption();
  auto textarea = TextArea(&content, &option);

  textarea->OnEvent(Event::Character('a'));
  textarea->OnEvent(Event::Character('b'));
  textarea->OnEvent(Event::Character('c'));
  textarea->OnEvent(Event::Return);
  textarea->OnEvent(Event::Character('a'));
  textarea->OnEvent(Event::Character('b'));
  textarea->OnEvent(Event::Character('c'));

  EXPECT_EQ(content, "abc\nabc");
  EXPECT_EQ(option.cursor_line(), 1u);
  EXPECT_EQ(option.cursor_column(), 3u);

  textarea->OnEvent(Event::Delete);
  EXPECT_EQ(content, "abc\nabc");
  EXPECT_EQ(option.cursor_line(), 1u);
  EXPECT_EQ(option.cursor_column(), 3u);

  textarea->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(content, "abc\nabc");
  EXPECT_EQ(option.cursor_line(), 1u);
  EXPECT_EQ(option.cursor_column(), 2u);

  textarea->OnEvent(Event::Delete);
  EXPECT_EQ(content, "abc\nab");
  EXPECT_EQ(option.cursor_line(), 1u);
  EXPECT_EQ(option.cursor_column(), 2u);

  textarea->OnEvent(Event::ArrowUp);
  EXPECT_EQ(content, "abc\nab");
  EXPECT_EQ(option.cursor_line(), 0u);
  EXPECT_EQ(option.cursor_column(), 2u);

  textarea->OnEvent(Event::Delete);
  EXPECT_EQ(content, "ab\nab");
  EXPECT_EQ(option.cursor_line(), 0u);
  EXPECT_EQ(option.cursor_column(), 2u);

  textarea->OnEvent(Event::Delete);
  EXPECT_EQ(content, "abab");
  EXPECT_EQ(option.cursor_line(), 0u);
  EXPECT_EQ(option.cursor_column(), 2u);

  textarea->OnEvent(Event::Delete);
  EXPECT_EQ(content, "abb");
  EXPECT_EQ(option.cursor_line(), 0u);
  EXPECT_EQ(option.cursor_column(), 2u);
  
  textarea->OnEvent(Event::Delete);
  EXPECT_EQ(content, "ab");
  EXPECT_EQ(option.cursor_line(), 0u);
  EXPECT_EQ(option.cursor_column(), 2u);

  textarea->OnEvent(Event::Delete);
  EXPECT_EQ(content, "ab");
  EXPECT_EQ(option.cursor_line(), 0u);
  EXPECT_EQ(option.cursor_column(), 2u);
}

TEST(TextAreaTest, Backspace) {
  std::string content;
  std::string placeholder;
  auto option = TextAreaOption();
  auto textarea = TextArea(&content, &option);

  textarea->OnEvent(Event::Character('a'));
  textarea->OnEvent(Event::Character('b'));
  textarea->OnEvent(Event::Character('c'));
  textarea->OnEvent(Event::Return);
  textarea->OnEvent(Event::Character('a'));
  textarea->OnEvent(Event::Character('b'));
  textarea->OnEvent(Event::Character('c'));

  EXPECT_EQ(content, "abc\nabc");
  EXPECT_EQ(option.cursor_line(), 1u);
  EXPECT_EQ(option.cursor_column(), 3u);

  textarea->OnEvent(Event::Backspace);
  EXPECT_EQ(content, "abc\nab");
  EXPECT_EQ(option.cursor_line(), 1u);
  EXPECT_EQ(option.cursor_column(), 2u);

  textarea->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(content, "abc\nab");
  EXPECT_EQ(option.cursor_line(), 1u);
  EXPECT_EQ(option.cursor_column(), 1u);

  textarea->OnEvent(Event::Backspace);
  EXPECT_EQ(content, "abc\nb");
  EXPECT_EQ(option.cursor_line(), 1u);
  EXPECT_EQ(option.cursor_column(), 0u);

  textarea->OnEvent(Event::ArrowLeft);
  textarea->OnEvent(Event::Backspace);
  EXPECT_EQ(content, "abcb");
  EXPECT_EQ(option.cursor_line(), 0u);
  EXPECT_EQ(option.cursor_column(), 3u);
}

TEST(TextAreaTest, MouseClick) {
  std::string content;
  std::string placeholder;
  auto option = TextAreaOption();
  auto textarea = TextArea(&content, &option);

  textarea->OnEvent(Event::Character("a"));
  textarea->OnEvent(Event::Character("b"));
  textarea->OnEvent(Event::Character("c"));
  textarea->OnEvent(Event::Character("d"));
  textarea->OnEvent(Event::Return);
  textarea->OnEvent(Event::Character("a"));
  textarea->OnEvent(Event::Character("b"));
  textarea->OnEvent(Event::Character("c"));
  textarea->OnEvent(Event::Character("d"));
  textarea->OnEvent(Event::Return);

  EXPECT_EQ(content, "abcd\nabcd\n");
  EXPECT_EQ(option.cursor_column(), 0u);
  EXPECT_EQ(option.cursor_line(), 2u);

  auto render = [&] {
    auto document = textarea->Render();
    auto screen = Screen::Create(Dimension::Fixed(10), Dimension::Fixed(0));
    Render(screen, document);
  };
  render();

  Mouse mouse;
  mouse.button = Mouse::Button::Left;
  mouse.motion = Mouse::Motion::Pressed;
  mouse.x = 0;
  mouse.y = 0;
  mouse.shift = false;
  mouse.meta = false;
  mouse.control = false;

  mouse.x = 0;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_column(), 0u);

  mouse.x = 2;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_column(), 2u);

  mouse.x = 2;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_column(), 2u);

  mouse.x = 1;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_column(), 1u);

  mouse.x = 3;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_column(), 3u);

  mouse.x = 4;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_column(), 4u);

  mouse.x = 5;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_column(), 4u);
}

TEST(TextAreaTest, MouseClickComplex) {
  std::string content;
  std::string placeholder;
  auto option = InputOption();
  option.cursor_position = 0;
  auto textarea = Input(&content, &placeholder, &option);

  textarea->OnEvent(Event::Character("测"));
  textarea->OnEvent(Event::Character("试"));
  textarea->OnEvent(Event::Character("a⃒"));
  textarea->OnEvent(Event::Character("ā"));

  EXPECT_EQ(option.cursor_position(), 4u);

  auto render = [&] {
    auto document = textarea->Render();
    auto screen = Screen::Create(Dimension::Fixed(10), Dimension::Fixed(1));
    Render(screen, document);
  };
  render();

  Mouse mouse;
  mouse.button = Mouse::Button::Left;
  mouse.motion = Mouse::Motion::Pressed;
  mouse.y = 0;
  mouse.shift = false;
  mouse.meta = false;
  mouse.control = false;

  mouse.x = 0;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 0u);

  mouse.x = 0;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 0u);

  mouse.x = 1;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 0u);

  mouse.x = 1;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 0u);

  mouse.x = 2;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 1u);

  mouse.x = 2;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 1u);

  mouse.x = 1;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 0u);

  mouse.x = 4;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 2u);

  mouse.x = 5;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 3u);

  mouse.x = 6;
  textarea->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 4u);
}

TEST(TextAreaTest, CtrlArrowLeft) {
  std::string content =
      "word word 测ord wo测d word\n"
      "coucou coucou coucou\n"
      "coucou coucou coucou\n";
  std::string placeholder;
  auto option = TextAreaOption();
  option.cursor_column = 22;
  option.cursor_line = 3;
  auto textarea = TextArea(&content, &option);

  // Use CTRL+Left several time
  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_line(), 2);
  EXPECT_EQ(option.cursor_column(), 20u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_line(), 2);
  EXPECT_EQ(option.cursor_column(), 14u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_line(), 2);
  EXPECT_EQ(option.cursor_column(), 7u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_line(), 2);
  EXPECT_EQ(option.cursor_column(), 0u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_line(), 1);
  EXPECT_EQ(option.cursor_column(), 20u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_line(), 1);
  EXPECT_EQ(option.cursor_column(), 14u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_line(), 1);
  EXPECT_EQ(option.cursor_column(), 7u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_line(), 1);
  EXPECT_EQ(option.cursor_column(), 0u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_line(), 0);
  EXPECT_EQ(option.cursor_column(), 24u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_line(), 0);
  EXPECT_EQ(option.cursor_column(), 20u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_line(), 0);
  EXPECT_EQ(option.cursor_column(), 15u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_line(), 0);
  EXPECT_EQ(option.cursor_column(), 10u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_line(), 0);
  EXPECT_EQ(option.cursor_column(), 5u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_line(), 0);
  EXPECT_EQ(option.cursor_column(), 0u);

  EXPECT_FALSE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_line(), 0);
  EXPECT_EQ(option.cursor_column(), 0u);
}

TEST(TextAreaTest, CtrlArrowLeft2) {
  std::string content =
    "   word  word  测ord  wo测d  word   ";
  auto option = TextAreaOption();
  option.cursor_column = 33;
  auto textarea = TextArea(&content, &option);

  // Use CTRL+Left several time
  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_column(), 27u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_column(), 21u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_column(), 15u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_column(), 9u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_column(), 3u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_column(), 0u);

  EXPECT_FALSE(textarea->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_column(), 0u);
}

TEST(TextAreaTest, CtrlArrowRight) {
  std::string content =
      "word word 测ord wo测d word\n"
      "coucou dfqdsf jmlkjm";

  auto option = TextAreaOption();
  option.cursor_column = 2;
  auto textarea = TextArea(&content, &option);

  // Use CTRL+Left several time
  EXPECT_TRUE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 4);
  EXPECT_EQ(option.cursor_line(), 0);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 9);
  EXPECT_EQ(option.cursor_line(), 0);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 14u);
  EXPECT_EQ(option.cursor_line(), 0);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 19u);
  EXPECT_EQ(option.cursor_line(), 0);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 24u);
  EXPECT_EQ(option.cursor_line(), 0);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 0u);
  EXPECT_EQ(option.cursor_line(), 1);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 6u);
  EXPECT_EQ(option.cursor_line(), 1);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 13u);
  EXPECT_EQ(option.cursor_line(), 1);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 20u);
  EXPECT_EQ(option.cursor_line(), 1);

  EXPECT_FALSE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 20u);
  EXPECT_EQ(option.cursor_line(), 1);
}

TEST(TextAreaTest, CtrlArrowRight2) {
  std::string content = "   word  word  测ord  wo测d  word   ";
  auto option = TextAreaOption();
  auto textarea = TextArea(&content, &option);

  // Use CTRL+Left several time
  EXPECT_TRUE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 7u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 13u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 19u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 25u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 31u);

  EXPECT_TRUE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 34u);

  EXPECT_FALSE(textarea->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_column(), 34u);
}

TEST(TextAreaTest, TypePassword) {
  std::string content;
  std::string placeholder;
  auto option = InputOption();
  option.cursor_position = 0;
  option.password = true;
  Component textarea = Input(&content, &placeholder, &option);

  textarea->OnEvent(Event::Character('a'));
  EXPECT_EQ(content, "a");
  EXPECT_EQ(option.cursor_position(), 1u);

  textarea->OnEvent(Event::Character('b'));
  EXPECT_EQ(content, "ab");
  EXPECT_EQ(option.cursor_position(), 2u);

  auto document = textarea->Render();
  auto screen = Screen::Create(Dimension::Fit(document));
  Render(screen, document);
  EXPECT_EQ(screen.PixelAt(0, 0).character, "•");
  EXPECT_EQ(screen.PixelAt(1, 0).character, "•");
}


}  // namespace ftxui

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
