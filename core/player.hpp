// See LICENSE file for copyright and license details.

#ifndef CORE__PLAYER_HPP
#define CORE__PLAYER_HPP

#define HAVE_NOT_SEEN_ANY_EVENTS (-1)

typedef int PlayerID;

class Player {
public: // TODO: make private
  PlayerID id;
  int lastSeenEventID;
};

#endif
