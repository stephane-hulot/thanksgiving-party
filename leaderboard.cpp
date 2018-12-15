#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "leaderboard.h"

Leaderboard::Leaderboard() : scores(NULL)
{	
    scores = new int[5];
    read_file();
}

void Leaderboard::read_file()
{
    std::ifstream read_flow(scores_file.c_str());

    if(read_flow)
    {
        int i = 0;
        std::string line;
        std::cout<<"Scores :"<<std::endl;
        while(std::getline(read_flow, line) && i < 5)
        {
            int value = 0;
            try
            {
                value = std::stoi(line);
            }
            catch(const std::exception& e)
            {
                value = 300000; //5min
            }
            scores[i++] = value;
            std::cout<<scores[i-1]<<std::endl;
        }
        read_flow.close();
        sort_scores(); //avoids cheating
    }
    else //creates scores file if doesn't exist
    {
        std::ofstream write_flow(scores_file.c_str());

        for(int i = 0; i < 5; i++)
        {
            scores[i] = 300000; //5min
            write_flow<<scores[i]<<std::endl;
        }
        write_flow.close();
        read_flow.close(); 
    }
}

void Leaderboard::add_score(int score)
{
    if(score < scores[4])
        scores[4] = score;
    sort_scores();
    write_file();
}

void Leaderboard::sort_scores()
{
    int tmp;
 
    for (int i = 0; i < 5; i++)
    {
        for(int j = i; j < 5; j++)
        {
            if(scores[j] < scores[i])
            {
                tmp = scores[i];
                scores[i] = scores[j];
                scores[j] = tmp;
            }
        }
    }
}

void Leaderboard::write_file()
{
    std::ofstream write_flow(scores_file.c_str());
    for(int i = 0 ; i < 5 ; i++)
    {
        write_flow << std::to_string(scores[i]) << std::endl;
    }
    write_flow.close();
}

Leaderboard::~Leaderboard()
{
    delete scores;
    std::cout<<"Leaderboard deleted"<<std::endl;
}