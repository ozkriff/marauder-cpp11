// See LICENSE file for copyright and license details.

#ifndef CORE_PLAYER_H
#define CORE_PLAYER_H

#define HAVE_NOT_SEEN_ANY_EVENTS (-1)

struct Player {
  int id;
  int lastSeenEventID;
};

#endif