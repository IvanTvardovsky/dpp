#include "KeyEventHandler.hpp"

void KeyEventHandler::CheckState()
{
 for (int i = 0; i < 255; i++)
 {
 bool keyState = GetKeyState(i);
 if (keyState != keysState[i])
 {
  keysState[i] = keyState;
  auto tmp=std::make_shared< KeyDescriptor>();
  tmp->isPressed = keyState;
  tmp->key = i;
  eventQuery.push_back(tmp);
 }
 }
}
std::shared_ptr< KeyDescriptor> KeyEventHandler::PoolEvent()
{
 std::shared_ptr< KeyDescriptor> result = nullptr;
 if (!eventQuery.empty())
 {
 result=eventQuery.back();
 eventQuery.pop_back();
 }
 return result;
}
