module;
#ifndef CMAKE_IMPORT_STD
#  include <string_view>
#  include <unordered_map>
#endif
#include <SDL3/SDL_init.h>
module App:Events;

#ifdef CMAKE_IMPORT_STD
import std;
#endif

import :Variants;

using namespace std;

namespace Windows {

class Container
{
  static unordered_map< SDL_WindowID, unique_ptr< Variants > > instances;

public:
  using iterator = decltype(instances)::iterator;

  static iterator::value_type::second_type const &
  Get(SDL_WindowID windowID);

  static iterator
  Remove(SDL_WindowID windowID);

  //Note: Fails with msvc when defined in :Events.implementation
  template< typename Variant, typename... Args >
  static pair< iterator, bool >
  Emplace(Args &&...args)
  {
    using value_type = iterator::value_type;
    auto value       = make_unique< value_type::second_type::element_type >(
      in_place_type< Variant >, std::forward< Args >(args)...);

    value_type::first_type const key =
      visit([](auto const &window) { return window.GetID(); }, *value);

    return instances.emplace(key, std::move(value));
  }

  static iterator
  End()
  {
    return instances.end();
  }
};

struct Events
{
  virtual void
  OnShow(SDL_WindowEvent &) {};

  virtual void
  OnHide(SDL_WindowEvent &) {};

  virtual void
  OnExposed(SDL_WindowEvent &) {};

  virtual void
  OnMoved(SDL_WindowEvent &) {};

  virtual void
  OnResized(SDL_WindowEvent &) {};

  virtual void
  OnPixelSizeChanged(SDL_WindowEvent &) {};

  virtual void
  OnMetalViewResized(SDL_WindowEvent &) {};

  virtual void
  OnMinimized(SDL_WindowEvent &) {};

  virtual void
  OnMaximized(SDL_WindowEvent &) {};

  virtual void
  OnRestored(SDL_WindowEvent &) {};

  virtual void
  OnMouseEnter(SDL_WindowEvent &) {};

  virtual void
  OnMouseLeave(SDL_WindowEvent &) {};

  virtual void
  OnFocusGained(SDL_WindowEvent &) {};

  virtual void
  OnFocusLost(SDL_WindowEvent &) {};

  virtual void
  OnCloseRequest(SDL_WindowEvent &) {};

  virtual void
  OnHitTest(SDL_WindowEvent &) {};

  virtual void
  OnICCProfileChanged(SDL_WindowEvent &) {};

  virtual void
  OnDisplayChanged(SDL_WindowEvent &) {};

  virtual void
  OnDisplayScaleChanged(SDL_WindowEvent &) {};

  virtual void
  OnSafeAreaChanged(SDL_WindowEvent &) {};

  virtual void
  OnOccluded(SDL_WindowEvent &) {};

  virtual void
  OnEnterFullscreen(SDL_WindowEvent &) {};

  virtual void
  OnLeaveFullscreen(SDL_WindowEvent &) {};

  virtual void
  OnHDRStateChanged(SDL_WindowEvent &) {};

  template< typename Self >
  SDL_AppResult
  Event(this Self &&self, SDL_WindowEvent &event)
  {
    switch (event.type) {
      using TYPE = SDL_EventType;
      case TYPE::SDL_EVENT_WINDOW_SHOWN:
        self.OnShow(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_HIDDEN:
        self.OnHide(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_EXPOSED:
        self.OnExposed(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_MOVED:
        self.OnMoved(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_RESIZED:
        self.OnResized(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        self.OnPixelSizeChanged(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_METAL_VIEW_RESIZED:
        self.OnMetalViewResized(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_MINIMIZED:
        self.OnMinimized(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_MAXIMIZED:
        self.OnMaximized(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_RESTORED:
        self.OnRestored(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_MOUSE_ENTER:
        self.OnMouseEnter(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_MOUSE_LEAVE:
        self.OnMouseLeave(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_FOCUS_GAINED:
        self.OnFocusGained(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_FOCUS_LOST:
        self.OnFocusLost(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        self.OnCloseRequest(event);
        if (self.minimizeOnClose) {
          self.Minimize();
        }
        else {
          self.Destroy();
        }
        break;
      case TYPE::SDL_EVENT_WINDOW_HIT_TEST:
        self.OnHitTest(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_ICCPROF_CHANGED:
        self.OnICCProfileChanged(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_DISPLAY_CHANGED:
        self.OnDisplayChanged(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
        self.OnDisplayScaleChanged(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_SAFE_AREA_CHANGED:
        self.OnSafeAreaChanged(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_OCCLUDED:
        self.OnOccluded(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
        self.OnEnterFullscreen(event);
      case TYPE::SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
        self.OnLeaveFullscreen(event);
        break;
      case TYPE::SDL_EVENT_WINDOW_DESTROYED:
        Windows::Container::Remove(event.windowID);
        return SDL_AppResult::SDL_APP_SUCCESS;
      case TYPE::SDL_EVENT_WINDOW_HDR_STATE_CHANGED:
        self.OnHDRStateChanged(event);
        break;
      default:
        unreachable();
        break;
    }
    return SDL_AppResult::SDL_APP_CONTINUE;
  }
};

}
