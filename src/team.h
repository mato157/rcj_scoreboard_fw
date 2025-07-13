#ifndef TEAM_H
#define TEAM_H

#include <stdint.h>



// Declaration of the Team class
class Team {
public:
    Team(uint8_t initPosition);
    void setScore(uint8_t newScore);
    void setColor(uint32_t newColor);

private:
    uint8_t position; // Position of the team (0-1)
    uint8_t score;
    uint32_t color;
};



extern Team team1;
extern Team team2;

#endif // TEAM_H