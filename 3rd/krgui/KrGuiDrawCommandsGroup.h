#ifndef __KK_KRGUI_DRAWCOMMANDSGROUP_H__
#define __KK_KRGUI_DRAWCOMMANDSGROUP_H__

namespace Kr
{
	namespace Gui
	{
		struct DrawCommandsGroup
		{
			DrawCommandsGroup(){}
			~DrawCommandsGroup()
			{
				for( size_t i = 0, sz = m_commands.size(); i < sz; ++i )
				{
					delete m_commands[i];
				}
			}

			std::vector<DrawCommands*> m_commands;
			size_t m_capacity = 0;
			size_t m_size = 0;

			// every render() drawCommands must be sorted
			// main group must have m_priority==0
			// any other groups (currently when beginNode), must have >0, depends on z order
			int m_priority = 0; 

			void _clear()
			{
				m_size = 0;
			}

			DrawCommands * _getDrawCommand()
			{
				++m_size;
				if( m_size > m_capacity ) // grow
				{
					++m_capacity;
					m_commands.push_back(new DrawCommands);
				}
				return m_commands[m_size-1];
			}

			DrawCommandsGroup* m_prevDrawGroup= nullptr;
		};
	}
}

#endif