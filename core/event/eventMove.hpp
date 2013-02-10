// See LICENSE file for copyright and license details.

#ifndef CORE__EVENT__EVENT_MOVE_HPP
#define CORE__EVENT__EVENT_MOVE_HPP

#include <vector>
#include "core/v2i.hpp"
#include "core/dir.hpp"
#include "core/event.hpp"

class Unit;
class Core;

class EventMove : public Event {
public:
  EventMove(int id);
  virtual ~EventMove();

  int unitID() const;
  const std::vector<V2i>& path() const;

  virtual void apply(Core& core) const;
  virtual void undo(Core& core) const;
  virtual bool isVisible(const Core& core) const;

  static void generate(
      Core& core, const Unit& u, const V2i& destination);

private:
  int mUnitID;
  std::vector<V2i> mPath;
  int mCost;
  Dir mInitialDirection;
};

#endif
