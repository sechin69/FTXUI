#include <functional>  // for function
#include <iostream>  // for basic_ostream::operator<<, operator<<, endl, basic_ostream, basic_ostream<>::__ostream_type, cout, ostream
#include <string>    // for string, basic_string, allocator
#include <vector>    // for vector

#include "ftxui/component/captured_mouse.hpp"      // for ftxui
#include "ftxui/component/component.hpp"           // for Menu
#include "ftxui/component/component_options.hpp"   // for MenuOption
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto screen = ScreenInteractive::Fullscreen();

  int angle = 180.f;
  float start = 0.f;
  float end = 1.f;
  auto slider_angle = Slider("Angle:        ", &angle, 0, 360);
  auto slider_start = Slider("Red position: ", &start, 0.f, 1.f);
  auto slider_end =   Slider("Blue position:", &end, 0.f, 1.f);

  auto layout = Container::Vertical({
    slider_angle,
    slider_start,
    slider_end,
  });

  auto renderer = Renderer(layout, [&] {
    auto background = text("Gradient") | center |
                      bgcolor(LinearGradient(
                          angle, {{Color::Red, start}, {Color::Blue, end}}));
    return vbox({
               background | flex,
               separator(),
               layout->Render(),
           }) |
           flex;
  });

  screen.Loop(renderer);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
