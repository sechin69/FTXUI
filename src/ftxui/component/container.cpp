#include <algorithm>  // for max, min
#include <cstddef>    // for size_t
#include <memory>  // for make_shared, __shared_ptr_access, allocator, shared_ptr, allocator_traits<>::value_type
#include <utility>  // for move
#include <vector>   // for vector, __alloc_traits<>::value_type

#include "ftxui/component/component.hpp"  // for Horizontal, Vertical, Tab
//#include "ftxui/component/component_base.hpp"  // for Components, Component, ComponentBase

namespace ftxui {

namespace Container {

/// @brief A list of components, drawn one by one vertically and navigated
/// vertically using up/down arrow key or 'j'/'k' keys.
/// @param children the list of components.
/// @ingroup component
/// @see ContainerBase
///
/// ### Example
///
/// ```cpp
/// auto container = Container::Vertical({
///   children_1,
///   children_2,
///   children_3,
///   children_4,
/// });
/// ```
Component Vertical(Components children) {
  return Vertical(std::move(children), nullptr);
}

/// @brief A list of components, drawn one by one vertically and navigated
/// vertically using up/down arrow key or 'j'/'k' keys.
/// This is useful for implementing a Menu for instance.
/// @param children the list of components.
/// @param selector A reference to the index of the selected children.
/// @ingroup component
/// @see ContainerBase
///
/// ### Example
///
/// ```cpp
/// auto container = Container::Vertical({
///   children_1,
///   children_2,
///   children_3,
///   children_4,
/// });
/// ```
Component Vertical(Components children, int* selector) {
  return std::make_shared<VerticalContainer>(std::move(children), selector);
}

/// @brief A list of components, drawn one by one horizontally and navigated
/// horizontally using left/right arrow key or 'h'/'l' keys.
/// @param children the list of components.
/// @ingroup component
/// @see ContainerBase
///
/// ### Example
///
/// ```cpp
/// int selected_children = 2;
/// auto container = Container::Horizontal({
///   children_1,
///   children_2,
///   children_3,
///   children_4,
/// }, &selected_children);
/// ```
Component Horizontal(Components children) {
  return Horizontal(std::move(children), nullptr);
}

/// @brief A list of components, drawn one by one horizontally and navigated
/// horizontally using left/right arrow key or 'h'/'l' keys.
/// @param children the list of components.
/// @param selector A reference to the index of the selected children.
/// @ingroup component
/// @see ContainerBase
///
/// ### Example
///
/// ```cpp
/// int selected_children = 2;
/// auto container = Container::Horizontal({
///   children_1,
///   children_2,
///   children_3,
///   children_4,
/// }, selected_children);
/// ```
Component Horizontal(Components children, int* selector) {
  return std::make_shared<HorizontalContainer>(std::move(children), selector);
}

/// @brief A list of components, where only one is drawn and interacted with at
/// a time. The |selector| gives the index of the selected component. This is
/// useful to implement tabs.
/// @param children The list of components.
/// @param selector The index of the drawn children.
/// @ingroup component
/// @see ContainerBase
///
/// ### Example
///
/// ```cpp
/// int tab_drawn = 0;
/// auto container = Container::Tab({
///   children_1,
///   children_2,
///   children_3,
///   children_4,
/// }, &tab_drawn);
/// ```
Component Tab(Components children, int* selector) {
  return std::make_shared<TabContainer>(std::move(children), selector);
}

}  // namespace Container

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
