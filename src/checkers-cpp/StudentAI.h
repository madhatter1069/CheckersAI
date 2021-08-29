#ifndef STUDENTAI_H
#define STUDENTAI_H
#include "AI.h"
#include "Board.h"
#include <chrono>
#include <algorithm>
#include <iostream>
#include <list>
#pragma once

//The following part should be completed by students.
//Students can modify anything except the class name and exisiting functions and varibles.
struct Node;
class StudentAI :public AI
{
public:
    Board board;
    Node* tree;
    double timePassed;
    double timePerMove;
    StudentAI(int col, int row, int p);
    virtual Move GetMove(Move board);

    Node *BestNode(Node *node);
    Node* makeNode(Node *node, Move move, int play);
    int SimulatePlay(Board & board, int play);
    Node* randomMove(Node *node);
    void MakeallNodes(Node *node, int play);
    Node* HighestWinRateNode(Node *node);
    Node* MostSimulationsNode(Node *node);
};


#endif //STUDENTAI_H
