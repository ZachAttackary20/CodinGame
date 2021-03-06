#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <map>
#include <utility>
#include <list>

using namespace std;

const int RANGE = 1000;

struct Coord {

  int x, y;
  Coord(int x, int y) : x(x), y(y) {}
  Coord(){}
  Coord& operator +=(const Coord& rhs) {
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
  }
  Coord operator /=(const int divisor) {
    this->x /= divisor;
    this->y /= divisor;
    return *this;
  }
  friend bool operator ==(const Coord& l, const Coord& r) {
    return l.x == r.x && l.y == r.y;
  }
  friend bool operator !=(const Coord& l, const Coord& r) {
    return l.x != r.x && l.y != r.y;
  }
};

struct Zombie {
  Coord cur, next, target;
  int turnsToKill;
  Zombie(Coord cur, Coord next, Coord target, int turnsToKill) :
    cur(cur), next(next), target(target), turnsToKill(turnsToKill) {}
  Zombie(){}
};



map<int, Coord> humans;
map<int, Zombie> zombies;
Coord me;//Person me(0,0);


double calcDist(Coord c1, Coord c2) {
  return sqrt(pow(c1.x - c2.x, 2) + pow(c1.y - c2.y, 2));
}

Coord closestPointInRange(Coord me, Coord other) {
  double dist = calcDist(me, other);
  Coord val(me.x + RANGE/dist*(other.x - me.x), me.y + RANGE/dist*(other.y - me.y));
  return val;
}

pair<int,Coord> findAttacking(Coord cur, Coord next) {
  cerr << "cur: " << cur.x << ", " << cur.y << endl;
  cerr << "next: " << next.x << ", " << next.y << endl;
  long double slope = ((float)cur.y - next.y)/(cur.x - next.x);
  cerr << cur.y << " - " << next.y << " / " << cur.x << " - " << next.x << " = " << slope << endl;
  cerr << slope << endl;
  double yIntercept = cur.y - slope*cur.x;
  int turnsToKill = 100;
  Coord target(0,0);
  for (auto mapPair : humans) {
    Coord person = mapPair.second;
    cerr << "person.y: " << person.y << endl
         << "(slope*person.x + yIntercept): " << (slope*person.x + yIntercept) << endl;
    if (abs(person.y - (slope*person.x + yIntercept)) > 10) continue; //this means the person is in its line of attack
    double dist = calcDist(cur, person);
    cerr << "dist from zombie: " << dist << endl;
    int curTurnsToKill = dist/400;
    if ((int)dist%400 != 0) curTurnsToKill++;
    if (curTurnsToKill < turnsToKill) {
      turnsToKill = curTurnsToKill;
      target = person;
    }
  }
  if (target.x == 0) {
    target.x = me.x;
    target.y = me.y;
    turnsToKill = 1000;
  }
  return make_pair(turnsToKill, target);
}

void getInData() {
  cin >> me.x >> me.y; cin.ignore();
  humans.clear();
  int humanCount;
  cin >> humanCount; cin.ignore();
  int humanId, humanX, humanY;
  for (int i = 0; i < humanCount; i++) {
    cin >> humanId >> humanX >> humanY; cin.ignore();
    Coord human(humanX, humanY);
    humans[humanId] = human;
  }
  zombies.clear();
  int zombieCount;
  cin >> zombieCount; cin.ignore();
  int zombieId, zombieX, zombieY, zombieXNext, zombieYNext;
  for (int i = 0; i < zombieCount; i++) {
    cin >> zombieId >> zombieX >> zombieY >> zombieXNext >> zombieYNext; cin.ignore();
    Coord cur(zombieX, zombieY), next(zombieXNext, zombieYNext);
    auto attackingPair = findAttacking(cur, next);
    Coord target = attackingPair.second;
    int turnsToKill = attackingPair.first;
    Zombie curZombie(cur, next, target, turnsToKill);
    cerr << "ZOMBIE: " << cur.x << ", " << cur.y << endl
         << "  next: " << next.x << ", " << next.y << endl
         << "  attacking: " << target.x << ", " << target.y << endl;
    zombies[zombieId] = curZombie;
  }
}

Zombie findClosestAttacker() {
  cerr << "finding closest killable zombie" << endl;
  int turnsToKill = 100;
  Zombie closestAttacker;
  list<Coord> deadHumans;
  for (auto mapPair : zombies) {
    Zombie& zombie = mapPair.second;
    /*cerr << "ZOMBIE: " << zombie.cur.x << ", " << zombie.cur.y << endl
         << "  next: " << zombie.next.x << ", " << zombie.next.y << endl
         << "  attacking: " << zombie.target.x << ", " << zombie.target.y << endl;
    */
    if (calcDist(zombie.target, me)/1000 - 2 > zombie.turnsToKill) {
      //cerr << calcDist(zombie.target, me) << "/1000 = " << calcDist(zombie.target, me)/1000 << endl;
      cerr << "turns to kill: " << zombie.turnsToKill << endl;
      cerr << "can't save this one" << endl;
      deadHumans.push_back(zombie.target);
    }
    else if (zombie.turnsToKill < turnsToKill && find(deadHumans.begin(), deadHumans.end(), zombie.target) == deadHumans.end()) {
      turnsToKill = zombie.turnsToKill;
      closestAttacker = zombie;
    }
  }
  return closestAttacker;
}

Coord middleOfZombies() {
  Coord center(0,0);
  for (auto mapPair : zombies) {
    Zombie& zombie = mapPair.second;
    center += zombie.next;
    cerr << "center: " << center.x << ", " << center.y << endl;
  }
  center /= zombies.size();
  return center;
}

int main()
{

  while (1) {
    cerr << "new turn" << endl;
    getInData();
    Zombie zombie = findClosestAttacker();
    /*cerr << "Closet Killable Zombie: " << zombie.cur.x << ", " << zombie.cur.y << endl
         << "  next: " << zombie.next.x << ", " << zombie.next.y << endl
         << "  attacking: " << zombie.target.x << ", " << zombie.target.y << endl;
    */
    Coord destination;
    if (zombie.turnsToKill == 1 || zombie.target != me) destination = zombie.target;
    else destination = middleOfZombies();
    cout << min(max(destination.x, 0), 16000) << " " << min(max(destination.y,0),9000) << endl;
  }

}
