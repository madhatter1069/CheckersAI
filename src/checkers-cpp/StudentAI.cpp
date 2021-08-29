#include "StudentAI.h"
#include <random>

//The following part should be completed by students.
//The students can modify anything except the class name and exisiting functions and varibles.
struct Node{
    Node* parent; //parent that points to this node
    vector<Node*>childNodes;//list of all moves made into nodes that stem from this current game state
    vector<Node*> untried;
    Move move;//move that got to this game state
    double wins = 0;
    int visitCount = 0;
    int playerNO = 0;
    bool visited = false;
};

StudentAI::StudentAI(int col,int row,int p)
        :AI(col, row, p)
{
    board = Board(col,row,p);
    board.initializeGame();
    player = 2;
    tree = new Node();//beginning of game node empty
    timePassed = 0;
    timePerMove = 8;
}

Move StudentAI::GetMove(Move move)
{
    if (move.seq.empty())
    {
        player = 1;
    } else{
        board.makeMove(move,player == 1?2:1);
    }

    if(timePassed > 475)
    {
        vector<vector<Move> > m = board.getAllPossibleMoves(player);
        int i = rand() % (m.size());
        vector<Move> checker_moves = m[i];
        int j = rand() % (checker_moves.size());
        Move random = checker_moves[j];
        board.makeMove(random,player);
        return random;
    }

    //slowing down near end so dont overshoot 8 mins check
    if(timePassed > 470)
    {
        timePerMove = 3;
    }


    if (tree->playerNO == 0){//beginning tree
        tree->visited = true;//set it to visited
        MakeallNodes(tree, player);//make all posisble moves for player as a node in untried
    }


    bool found = false;
    if(tree->playerNO > 0 && tree->playerNO == player){//advance tree to new node by finding child that has the move the opponent made
        for (int i = 0; i< tree->childNodes.size();++i){
            if(tree->childNodes[i]->move.toString() == move.toString()){
                tree = tree->childNodes[i];
                found = true;
                break;
            }
        }
        if (!found){
            for (int j = 0; j< tree->untried.size();++j){
                if(tree->untried[j]->move.toString() == move.toString()){
                    tree = tree->untried[j];
                    found = true;
                    break;
                }}}
        if (!found){//invalidMoveError fix
            Node* next = makeNode(tree, move, 3-tree->playerNO);
            tree->childNodes.push_back(next);
            tree = next;
        }}
    if(tree->visited == false){//future iterations will make all nodes in untried like above
        MakeallNodes(tree, 3-tree->playerNO);
        tree->visited = true;
    }


    clock_t t = clock();//clock to time each turn
    Node* buddy;
    int iterations = 0;
    double end = timePerMove; //determine how many seconds to iterare through possible moves

    //if there is only one move
    vector<vector<Move>> Moves = board.getAllPossibleMoves(player);
    if(Moves.size() == 1 && Moves[0].size() == 1)
    {
        end = 0.5;
    }

    while( ( (clock()-t) /CLOCKS_PER_SEC) < end && iterations < 2500){//loop for an amount of time
        buddy = tree;//temp node named buddy

        while(board.isWin(player)==0 || board.isWin(3-player)==0){//loop while the game still has moves
            if(buddy->untried.size() > 0){//untried -> child nodes that havent been tested yet
                buddy = randomMove(buddy);//randomly pick one
                board.makeMove(buddy->move, buddy->playerNO);
                break;//break to simulate it
            }
            else {
                buddy = BestNode(buddy);//choose best uct node
                board.makeMove(buddy->move, buddy->playerNO);
                if(buddy->visited == false)//if havent visited make all moves a node and put in untried
                    MakeallNodes(buddy, 3-buddy->playerNO);
                buddy->visited = true;
            }
        }
        Board boardCopy = board;
        int sim = SimulatePlay(boardCopy, buddy->playerNO);//simulate to end game
        while(buddy!=tree){//back prop to tree    //try adding if sim equals that nodes number and subtract otherwise
            board.Undo();
            if (sim == buddy->playerNO)
            {
                buddy->wins+=1;   //if win for node add
            }
            else if(sim == -1){
                buddy->wins += 0.5; //if tie
            }


            buddy->visitCount++;
            buddy = buddy->parent;//back up to parent
        }
        if (sim == buddy->playerNO)
        {
            buddy->wins+=1;   //if win for node add
        }
        else if(sim == -1){
            buddy->wins += 0.5; //if tie
        }

        buddy->visitCount++;
        iterations++;
    }

    if(iterations > 500)//change timePerMove once reached certain iteration number
    {
        timePerMove = 6;
    }
    Node *best = MostSimulationsNode(tree);//pick best node based off value
    tree = best;//tree now points to chosen node so the tree moves forward with the game
    board.makeMove(best->move,player);//make the move on the board
    timePassed += (clock()-t)/CLOCKS_PER_SEC;

//    MakeallNodes(tree, 3-tree->playerNO);//this line and the next line is what i changed to make it work
//    tree->visited = true;
    //cout << "ITERATIONS: " << iterations << endl;
    return best->move;//return the move done by selected node
}


Node* StudentAI::randomMove(Node *node){//picks random move and makes it a node that hasnt been tried
    int i = rand() % (node->untried.size());
    Node* want = node->untried[i];
    node->untried.erase(node->untried.begin()+i);
    node->childNodes.push_back(want);
    return want;//make the move into a node and a child of current node
}

void StudentAI::MakeallNodes(Node *node, int play){//make all moves a node and child of parent
    vector<vector<Move>> Moves = board.getAllPossibleMoves(play);//put moves in the node
    for(int i = 0; i<Moves.size(); ++i){//make all possible moves nodes
        for(int j = 0; j<Moves[i].size();++j){
            node->untried.push_back(makeNode(node, Moves[i][j], play));
        }
    }
}

Node* StudentAI::BestNode(Node *node){//this func is to choose the best node through uct
    int i;
    int index = 0;
    double lastUCT = -1;
    double newUCT;
    double visits;
    int sp = node->visitCount;
    double val;
    double c = sqrt(2);

    for (i = 0; i<node->childNodes.size();++i){

        visits = node->childNodes[i]->visitCount;
        if(visits == 0){
            visits = .0001;//set visit to small number since division by 0 not allowed
            val = 0;
        }
        else{
            val = node->childNodes[i]->wins / visits;    }
        newUCT = val + (c * sqrt( log(sp) / visits ) );
        if (newUCT > lastUCT){
            lastUCT = newUCT;
            index = i;
        }
    }
    return node->childNodes[index];//choose and return a node with best winrate
}

int StudentAI::SimulatePlay(Board & board, int play){//simulates game to end and backpropogates
    int q;
    if (board.isWin(play) != 0){
        q = board.isWin(play);
        board.Undo();
        return q;//return who won the game or if a tie
    }

    vector<vector<Move> > m = board.getAllPossibleMoves(3-play);
    int i = rand() % (m.size());
    vector<Move> checker_moves = m[i];
    int j = rand() % (checker_moves.size());
    Move res = checker_moves[j];


    board.makeMove(res, 3-play);//make the move on the board
    q = SimulatePlay(board, 3-play);//recursion of play to end

    return q;  //return the value of who won
}

Node* StudentAI::makeNode(Node *node, Move move, int play){//make a node out of a move and sets its parent
    Node *wanted = new Node();//make a new node
    wanted->parent = node;
    wanted->move = move;
    wanted->playerNO = play;
    //node->untried.push_back(wanted);
    return wanted;//return the node
}


Node* StudentAI::HighestWinRateNode(Node *node){//this func chooses highest winrate node
    int index = 0;
    double currentHighestWinRate = -1;
    double newHighestWinRate;
    double totalSimulations;
    double wins;

    for (int i = 0; i<node->childNodes.size();++i)
    {
        totalSimulations = node->childNodes[i]->visitCount;
        wins = node->childNodes[i]->wins;
        if(totalSimulations == 0){
            totalSimulations = .000001;//set visit to small number since division by 0 not allowed
        }
        newHighestWinRate =  wins / totalSimulations ;
        if (newHighestWinRate > currentHighestWinRate){
            currentHighestWinRate = newHighestWinRate;
            index = i;
        }
    }
    return node->childNodes[index];//choose and return a node with best winrate
}

Node* StudentAI::MostSimulationsNode(Node *node){//this func chooses highest winrate node
    int index = 0;
    double highestSims = -1;
    double sims;

    for (int i = 0; i<node->childNodes.size();++i)
    {
        sims = node->childNodes[i]->visitCount;
        if (sims > highestSims){
            highestSims = sims;
            index = i;
        }
    }
    return node->childNodes[index];//choose and return a node with best winrate
}

// ./main 10 10 3 m 0

//python3 AI_Runner.py 10 10 3 l /Users/jaredclark/Desktop/CheckersAI/src/checkers-cpp/main /Users/jaredclark/Desktop/CheckersAI/Tools/Sample_AIs/Poor_AI_368/main.py

//python3 AI_Runner.py 7 7 2 l /Users/jaredclark/Desktop/CheckersAI/Tools/Sample_AIs/Poor_AI_368/main.py /home/jaredac1/171checkers/Checkers_Student-master/src/checkers-cpp/main

//   module load python/3.5.2

// python3 AI_Runner.py 7 7 2 l /home/shcherbm/CS-171/Checkers_Student-master/src/checkers-cpp/main /home/shcherbm/CS-171/Checkers_Student-master/Tools/Sample_AIs/Poor_AI_368/main.py

// python3 AI_Runner.py 7 7 2 l /home/shcherbm/CS-171/Checkers_Student-master/Tools/Sample_AIs/Poor_AI_368/main.py /home/shcherbm/CS-171/Checkers_Student-master/src/checkers-cpp/main