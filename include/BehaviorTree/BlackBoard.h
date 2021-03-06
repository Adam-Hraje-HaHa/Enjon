#ifndef ENJON_BLACKBOARD_H
#define ENJON_BLACKBOARD_H

#include "System/Internals.h"
#include "Utils/Errors.h"

#include <unordered_map>
#include <vector>

// Need to keep track of K/V pairs
// Need the keys to be strings and the pairs to be whatever value

// class BlackBoardComponentBase
// {
// 	public:
// 		virtual void Init() 	= 0;
// };

// template <typename T>
// class BlackBoardComponent : public BlackBoardComponentBase
// {
// 	public:

// 		BlackBoardComponent(){}
// 		BlackBoardComponent(T Data)
// 		{
// 			this->Data = Data;
// 		}

// 		virtual void Init(){}

// 		inline T GetData() { return Data; }
// 		inline void SetData(T t) { Data = t; }

// 	protected:
// 		T Data;
// };


// namespace BT
// {
// 	class BlackBoard
// 	{
// 		public:
// 			BlackBoard(){}
// 			~BlackBoard()
// 			{
// 				RemoveComponents();
// 			}

// 			void AddComponent(std::string S, BlackBoardComponentBase* B)
// 			{
// 				Components[S] = B;
// 			}

// 			template <typename T>
// 			inline BlackBoardComponent<T>* GetComponent(std::string S) 
// 			{
// 				auto it = Components.find(S);
// 				if (it != Components.end()) return static_cast<BlackBoardComponent<T>*>(Components[S]);
// 				else Enjon::Utils::FatalError("BlackBoard Component Does Not Exist: " + S);
// 			}

// 			inline void RemoveComponents()
// 			{
// 				for (auto itr = Components.begin(); itr != Components.end(); ++itr)
// 				{
// 					auto val = (*itr).second;
// 					delete val;
// 				}
// 			}

// 		private:
// 			std::unordered_map<std::string, BlackBoardComponentBase*> Components;
// 	};	
// }

#endif