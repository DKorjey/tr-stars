#include <iostream>
#include <array>
#include <thread>
#include <chrono>
#include <random>
#include <ncurses.h>

#define MAX_STAR_TICKS 20 // How many ticks need for star to grow. Default: 20
#define MS_PER_TICK 50 // Miliseconds per tick. Default: 50
#define MAX_STARS 25 // Max amount of stars on screen. Default: 25
#define MIN_GESTAGE 20 // Min value that makes not appear a star at least N ticks before born. Default: 20
#define MAX_GESTAGE 100 // Same as above, but max value. Default: 100

inline void mvsquare(int y, int x, int clr)
{
  if (y >= 0 && y < getmaxy(stdscr) && x >= -1 && x <= getmaxx(stdscr))
  {
    attron(COLOR_PAIR(clr + 1));
    if (x >= 0)
      mvaddch(y, x, ' ');
    if (x <= getmaxx(stdscr))
      mvaddch(y, x + 1, ' ');
    attroff(COLOR_PAIR(clr + 1));
  }
}

int randint(int min, int max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> uni(min, max);
    return uni(rng);
}

namespace tr
{
  typedef uint8_t u8;

  enum star_color
  {
    cyan = 0,
    magenta,
    yellow
  };
  enum star_life
  {
    embrion = 0,
    born,
    teen,
    adult,
    old,
    oldf
  };

  class Star
  {
  private:
    int x, y;
    star_color clr;
    star_life state = embrion;
    u8 lifetime = 0;
    u8 gestage;
    bool is_alive = true;

  public:
    Star()
    {
      this->x = randint(0, getmaxx(stdscr));
      this->y = randint(0, getmaxy(stdscr));
      this->clr = static_cast<tr::star_color>(randint(0, 2));
      this->gestage = randint(MIN_GESTAGE, MAX_GESTAGE);
    }

    void reborn()
    {
      this->x = randint(0, getmaxx(stdscr));
      this->y = randint(0, getmaxy(stdscr));
      this->clr = static_cast<tr::star_color>(randint(0, 2));
      this->state = embrion;
      this->is_alive = true;
      this->lifetime = 0;
      this->gestage = randint(MIN_GESTAGE, MAX_GESTAGE);
    }

    void live()
    {
      if (!this->is_alive) return;

      if (this->gestage != 0)
      {
        if (this->lifetime < this->gestage) this->lifetime++;
        else if (this->lifetime == this->gestage)
        {
          this->lifetime = 0;
          this->gestage = 0;
          this->state = tr::star_life::born;
        }
      } else {

      if ((this->lifetime + 1) % MAX_STAR_TICKS != 0)
        this->lifetime++;
      else
      {
        this->lifetime = (this->lifetime + 1) % MAX_STAR_TICKS;
        if (static_cast<u8>(state) + 1 > oldf)
        {
          this->is_alive = false;
          return;
        }
        this->state = static_cast<star_life>(static_cast<u8>(this->state) + 1);
      }
      }
    }

    bool is_dead() const
    {
      return !this->is_alive;
    }

    void render()
    {
      switch (this->state)
      {
      case born:
      case oldf:
        mvsquare(this->y, this->x, this->clr);
        break;
      case teen:
      case old:
        mvsquare(this->y - 1, this->x, this->clr);
        mvsquare(this->y, this->x - 2, this->clr);
        mvsquare(this->y, this->x, this->clr);
        mvsquare(this->y, this->x + 2, this->clr);
        mvsquare(this->y + 1, this->x, this->clr);
        break;
      case adult:
        mvsquare(this->y - 2, this->x, this->clr);
        mvsquare(this->y - 1, this->x - 2, this->clr);
        mvsquare(this->y - 1, this->x + 2, this->clr);
        mvsquare(this->y, this->x - 4, this->clr);
        mvsquare(this->y, this->x + 4, this->clr);
        mvsquare(this->y + 1, this->x - 2, this->clr);
        mvsquare(this->y + 1, this->x + 2, this->clr);
        mvsquare(this->y + 2, this->x, this->clr);
        break;
      }
    }
  };
}

int main()
{
  initscr();
  noecho();
  cbreak();
  nodelay(stdscr, TRUE);
  curs_set(0);

  start_color();

  init_pair(1, COLOR_CYAN, COLOR_CYAN);
  init_pair(2, COLOR_MAGENTA, COLOR_MAGENTA);
  init_pair(3, COLOR_YELLOW, COLOR_YELLOW);

  std::array<tr::Star, MAX_STARS> stars{ {} };

  int ch;

  while (1)
  {
    ch = getch();
    if (ch != ERR) break;

    clear();
    for (auto &star : stars)
    {
      star.render();
      star.live();
      if (star.is_dead()) star.reborn();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_TICK));
  }

  endwin();
}
