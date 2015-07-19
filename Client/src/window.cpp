#include "window.hpp"

#include <sstream>
#include <GL\glew.h>

namespace lys
{
	Window::Window(const std::string &title, const Metric2 &size, const bool &visible)
		: _title(title), _size(size), _visible(visible)
	{
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			std::stringstream err;
			err << "Error initializing SDL (" << SDL_GetError() << ")";
			SDL_ClearError();
			throw std::exception(err.str().data());
		}

		{
			int error[3];

			error[0] = SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
			error[1] = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, LYS_GL_MAJOR_VERSION);
			error[2] = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, LYS_GL_MINOR_VERSION);

			if ((error[0] || error[1] || error[2]) != 0)
			{
				std::stringstream err;
				err << "Error setting OpenGL context attribute (" << SDL_GetError() << ")";
				SDL_ClearError();
				throw std::exception(err.str().data());
			}
		}

		_window = SDL_CreateWindow(_title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _size.x, _size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
		if (!_window)
		{
			std::stringstream err;
			err << "Error creating window (" << SDL_GetError() << ")";
			SDL_ClearError();
			throw std::exception(err.str().data());
		}

		_context = SDL_GL_CreateContext(_window);
		if (*SDL_GetError() != '\0')
		{
			std::stringstream err;
			err << "Error creating OpenGL context (" << SDL_GetError() << ")";
			SDL_ClearError();
			throw std::exception(err.str().data());
		}

		GLenum error = glewInit();
		if (error != GLEW_OK)
		{
			std::stringstream err;
			err << "Error initializing GLEW (" << glewGetErrorString(error) << ")";
			SDL_ClearError();
			throw std::exception(err.str().data());
		}

		GLint major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);

		LYS_LOG("Target OpenGL version (%d.%d), system version (%d.%d)", LYS_GL_MAJOR_VERSION, LYS_GL_MINOR_VERSION, major, minor);

		if (!(major > LYS_GL_MAJOR_VERSION)) if (minor < LYS_GL_MINOR_VERSION)
		{
			LYS_LOG_WARNING("Target OpenGL version not supported by your system");
		}

		setSwapInterval(0);
		setVisible(_visible);
		setFocus(true);

		_title = SDL_GetWindowTitle(_window);
		SDL_GetWindowSize(_window, &_size.x, &_size.y);
		SDL_GetWindowPosition(_window, &_position.x, &_position.y);
		SDL_GetRelativeMouseState(&_mouse.x, &_mouse.y);
	}

	Window::~Window()
	{
		SDL_GL_DeleteContext(_context);
		SDL_DestroyWindow(_window);
	}

	void Window::makeCurrent()
	{
		SDL_GL_MakeCurrent(_window, _context);
	}

	void Window::swapBuffers()
	{
		SDL_GL_SwapWindow(_window);
	}

	bool Window::pollMessages(WindowMessage &message)
	{
		if (SDL_PollEvent(&_event))
		{
			switch (_event.type)
			{
			case SDL_EventType::SDL_WINDOWEVENT:
			{
				switch (_event.window.event)
				{
				case SDL_WINDOWEVENT_CLOSE:
				{
					message = WindowMessage::CLOSE;
					break;
				}
				case SDL_WINDOWEVENT_FOCUS_GAINED:
				{
					_focus = true;
					message = WindowMessage::FOCUSGAINED;
					break;
				}
				case SDL_WINDOWEVENT_FOCUS_LOST:
				{
					_focus = false;
					message = WindowMessage::FOCUSLOST;
					break;
				}
				case SDL_WINDOWEVENT_MOVED:
				{
					_position.x = _event.window.data1;
					_position.y = _event.window.data2;
					message = WindowMessage::WINDOWMOVED;
					break;
				}
				case SDL_WINDOWEVENT_SIZE_CHANGED:
				{
					_size.x = _event.window.data1;
					_size.y = _event.window.data2;
					message = WindowMessage::WINDOWSIZECHANGED;
					break;
				}
				default:
				{
					message = WindowMessage::NOTHING;
					break;
				}
				}
				break;
			}
			case SDL_EventType::SDL_MOUSEMOTION:
			{
				_mouse.x = _event.motion.x;
				_mouse.y = _event.motion.y;
				message = WindowMessage::MOUSEMOTION;
				break;
			}
			case SDL_EventType::SDL_MOUSEBUTTONDOWN:
			{
				_buttons[_event.button.button] = true;
				message = WindowMessage::BUTTONDOWN;
				break;
			}
			case SDL_EventType::SDL_MOUSEBUTTONUP:
			{
				_buttons[_event.button.button] = false;
				message = WindowMessage::BUTTONUP;
				break;
			}
			case SDL_EventType::SDL_KEYDOWN:
			{
				_keys[_event.key.keysym.scancode] = true;
				message = WindowMessage::KEYDOWN;
				break;
			}
			case SDL_EventType::SDL_KEYUP:
			{
				_keys[_event.key.keysym.scancode] = false;
				message = WindowMessage::KEYUP;
				break;
			}
			default:
			{
				message = WindowMessage::NOTHING;
				break;
			}
			}
			return true;
		}
		message = WindowMessage::NOTHING;
		return false;
	}

	const std::string &Window::getTitle() const
	{
		return _title;
	}

	void Window::setTitle(const std::string &title)
	{
		_title = title;
		SDL_SetWindowTitle(_window, _title.data());
	}

	const Metric2 &Window::getSize() const
	{
		return _size;
	}

	void Window::setSize(const Metric2 &size)
	{
		_size = size;
		SDL_SetWindowSize(_window, _size.x, _size.y);
	}

	const bool &Window::getVisible() const
	{
		return _visible;
	}

	void Window::setVisible(const bool &visible)
	{
		_visible = visible;
		if (_visible) SDL_ShowWindow(_window);
		else SDL_HideWindow(_window);
	}

	const Metric2 &Window::getPosition() const
	{
		return _position;
	}

	void Window::setPosition(const Metric2 &position)
	{
		_position = position;
		SDL_SetWindowPosition(_window, _position.x, _position.y);
	}

	const bool &Window::getFocus() const
	{
		return _focus;
	}

	void Window::setFocus(const bool &focus)
	{
		_focus = focus;
		if (_focus) SDL_RaiseWindow(_window);
		else SDL_HideWindow(_window);
	}

	const int &Window::getSwapInterval() const
	{
		return _swapInterval;
	}

	void Window::setSwapInterval(const int &swapInterval)
	{
		_swapInterval = swapInterval;
		SDL_GL_SetSwapInterval(_swapInterval);
	}

	const Metric2 &Window::getMouse() const
	{
		return _mouse;
	}

	void Window::setMouse(const Metric2 &mouse)
	{
		_mouse = mouse;
		SDL_WarpMouseInWindow(_window, _mouse.x, _mouse.y);
	}

	const bool &Window::getButton(const unsigned int &button) const
	{
		return _buttons[button];
	}

	const bool &Window::getKey(const unsigned int &key) const
	{
		return _keys[key];
	}

}