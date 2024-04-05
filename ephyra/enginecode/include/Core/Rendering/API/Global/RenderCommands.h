/** \ file renderCommands.h */
#pragma once

#include <functional>
#include <memory>

namespace Engine
{
	class RendererCommon;

	class RenderCommand
	{
	public:

		enum class Commands {clearDepthBuffer, clearColourBuffer, clearColourAndDepthBuffer, setClearColour, setBlendFunc, Enable, Disable}; // Add Commands
	private:

		std::function<void(void)> m_action; // The action of the render command

		friend class RenderCommandFactory;
		friend class RendererCommon;
	};

	class RenderCommandFactory
	{
	public:
		template<typename ...Args> static RenderCommand * createCommand(RenderCommand::Commands command, Args&& ...args)
		{
			RenderCommand * result = new RenderCommand;

			switch (command)
			{
			case RenderCommand::Commands::clearColourAndDepthBuffer:
				result->m_action = getClearColourAndDepthBufferCommand();
				return result;
				break;

			case RenderCommand::Commands::setBlendFunc:
				result->m_action = setBlendFuncCommand();
				return result;
				break;

			case RenderCommand::Commands::Enable:
			{
				GLenum command;

				auto argTuple = std::make_tuple(args...);

				getTupleValue<GLenum, 0>(command, argTuple);

				result->m_action = glEnableCommand((int&&)command);
				return result;
				break;
			}

			case RenderCommand::Commands::Disable:
			{
				GLenum command;

				auto argTuple = std::make_tuple(args...);

				getTupleValue<GLenum, 0>(command, argTuple);

				result->m_action = glDisableCommand((int&&)command);
				return result;
				break;
			}

			case RenderCommand::Commands::setClearColour:
				{
					float r, g, b, a;

					auto argTuple = std::make_tuple(args...);

					getTupleValue<float, 0>(r, argTuple);
					getTupleValue<float, 1>(g, argTuple);
					getTupleValue<float, 2>(b, argTuple);
					getTupleValue<float, 3>(a, argTuple);

					result->m_action = setClearColourCommand(r, g, b, a);
					return result;
					break;
				}

			}
		}

	private:
		static std::function<void(void)> getClearColourAndDepthBufferCommand();
		static std::function<void(void)> setClearColourCommand(float r, float g, float b, float a);
		static std::function<void(void)> setBlendFuncCommand();
		static std::function<void(void)> glEnableCommand(int&& command);
		static std::function<void(void)> glDisableCommand(int&& command);

		// Following Code Based On My Interpretation of Simon Couplands Code Based On Code From Geek For Geeks On How To Iterate Over Elements Of An StdTuple In C++

		template <typename G, size_t I, typename... Ts>
		typename std::enable_if<I >= sizeof...(Ts), void>::type
		static getTupleValue(G& result, std::tuple<Ts...> tup)
		{
			// If index is greater than or equal to tuple size, don't do anything
		}

		template <typename G, size_t I, typename... Ts>
		typename std::enable_if<(I < sizeof...(Ts)), void>::type
		static getTupleValue(G& result, std::tuple<Ts...> tup)
		{
			// Get the I thing in the tuple
			result = std::get<I>(tup);
		}

	};

}
